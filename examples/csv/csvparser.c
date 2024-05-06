/* C standard library includes */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/* POSIX  includes */
#include <time.h>
#include <sys/types.h>

/* external lib includes */
#include <logger.h>

/* local includes */
#include "csvparser.h"

struct CSVData empty_csv = {0};

void CSVData_dest(CSVData * csv_data) {
    if (csv_data->data) {
        free(csv_data->data);
        csv_data->data = NULL;
    }
    if (csv_data->offsets) {
        free(csv_data->offsets);
        csv_data->offsets = NULL;
    }
}

typedef struct CSVParser {
    Parser Parser;
    CSVData csv;
} CSVParser;

err_type CSVParser_init(CSVParser * parser, char const * name, size_t name_length,
                         char const * string, size_t string_length, char * log_file, unsigned char log_level) {
    parser->csv = (CSVData) {
        .data = NULL,
        .offsets = NULL,
        .ncols = 0,
        .nrows = 0,
        .nbytes = 0
    };
    parser->Parser._class->init(&parser->Parser, name, name_length, string, string_length, (Rule *)&csv_token, (Rule *)&csv_csv, CSV_NRULES, 0, 0, 0, log_file, log_level);
    return PEGGY_SUCCESS;
}

/* does not destroy the CSVData, which is expected to be returned/used elsewhere */
void CSVParser_dest(CSVParser * parser) {
    parser->Parser._class->dest(&parser->Parser);
}

CSVParser csv = {
    .Parser = {
        ._class = &Parser_class,
        .logger = DEFAULT_LOGGER_INIT,
    }, 
    .csv = {0}
};

ASTNode * process_string(Production * string_prod, Parser * parser, ASTNode * node) {
    ((CSVParser *) parser)->csv.nbytes += node->str_length - 1; // remove the encompassing double quotes but add a null terminator
    return build_action_default(string_prod, parser, node);
}

ASTNode * process_nonstring(Production * nonstring_prod, Parser * parser, ASTNode * node) {
    ((CSVParser *) parser)->csv.nbytes += node->str_length + 1; // add a null-terminator
    return build_action_default(nonstring_prod, parser, node);
}

ASTNode * process_record(Production * record_prod, Parser * parser, ASTNode * node) {
    CSVParser * csv = (CSVParser *) parser;
    size_t ncols = (node->nchildren + 1) >> 1;
    
    if (!csv->csv.ncols) {
        csv->csv.ncols = ncols;
    } else if (csv->csv.ncols != ncols) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - csv parsing error: found a different number of columns in row %zu compared to prior rows - found %zu, expected %zu\n", __func__, csv->csv.nrows, ncols, csv->csv.ncols);
        return &ASTNode_fail;
    }

    csv->csv.nrows++;
    return build_action_default(record_prod, parser, node);
}

void handle_field(CSVParser * csv_parser, ASTNode * node, size_t index) {
    Token tok;
    csv_parser->Parser._class->get((Parser *)csv_parser, node->token_key, &tok);
    char * dest = csv_parser->csv.data + csv_parser->csv.offsets[index];
    char const * src;
    size_t N = 0;
    if (node->children[0]->rule->id == NONSTRING_FIELD) {
        src = tok.string + tok.start;
        N = node->str_length;
    } else { // STRING
        src = tok.string + tok.start + 1;
        N = node->str_length - 2;
    }
    memcpy(dest, src, N);
    dest[N] = '\0';
    csv_parser->csv.offsets[index + 1] = csv_parser->csv.offsets[index] + N + 1;
}

void handle_record(CSVParser * csv_parser, ASTNode * node, size_t row) {
    size_t Nchild = node->nchildren;
    size_t j = row * csv_parser->csv.ncols;
    for (size_t i = 0; i < Nchild; i += 2) {
        handle_field(csv_parser, node->children[i], j);
        j++;
    }
}

ASTNode * handle_csv(Production * csv_prod, Parser * parser, ASTNode * node) {
    CSVParser * csv = (CSVParser *) parser;
    csv->csv.data = malloc(sizeof(*csv->csv.data) * csv->csv.nbytes);
    if (!csv->csv.data) {
        return &ASTNode_fail;
    }
    csv->csv.offsets = malloc(sizeof(*csv->csv.offsets) * (csv->csv.ncols * csv->csv.nrows + 1)); // add one to be able to set one past
    if (!csv->csv.offsets) {
        free(csv->csv.data);
        csv->csv.data = NULL;
        return &ASTNode_fail;
    }

    csv->csv.offsets[0] = 0;

    ASTNode * record_list = node->children[0];
    size_t Nchild = record_list->nchildren;
    size_t j = 0;
    for (size_t i = 0; i < Nchild; i += 2) {
        handle_record(csv, record_list->children[i], j);
        j++;
    }
    return node;
}

bool timeit = false;

CSVData from_string(char * string, size_t string_length, char * name, size_t name_length, char * log_file, unsigned char log_level) {
    err_type status = PEGGY_SUCCESS;
    if (!timeit) {
        if ((status = CSVParser_init(&csv, name, name_length, string, string_length, log_file, log_level))) {
            return empty_csv;
        }
        return csv.csv;
    }
    static char const * const record_format = "%zu, %10.8lf, %s\n";
    char buffer[1024] = {'\0'};
    FILE * time_records = fopen("times.csv", "ab");
    
    struct timespec t0, t1;
    double time;
    clockid_t clk = CLOCK_MONOTONIC;
    //double clock_conversion = 1.0e-6;
    clock_gettime(clk, &t0);
    status = CSVParser_init(&csv, name, name_length, string, string_length, log_file, log_level);
    clock_gettime(clk, &t1);

    if (t1.tv_nsec < t0.tv_nsec) {
        time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
    } else {
        time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
    }

    snprintf(buffer, 1024, record_format, csv.csv.ncols*csv.csv.nrows, time, name);
    fprintf(time_records, "%s", buffer);
    printf("%s", buffer);
    
    fclose(time_records);
    return csv.csv;
}

CSVData from_file(char * filename, char * log_file, unsigned char log_level) {
    FILE * pfile = fopen(filename, "rb");
    if (!pfile) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - failed to open file %s\n", __func__, filename);
    }
    fseek(pfile, 0, SEEK_END);
    long file_size = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);

    char * string = malloc(file_size + 1);
    if (!string) {
        return empty_csv;
    }
    size_t nbytes = fread(string, 1, file_size, pfile);
    if (ferror(pfile)) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - failed to read file: %s\n", __func__, filename);
        free(string);
        return empty_csv;
    }
    string[file_size] = '\0';

    fclose(pfile);

    size_t name_length = strlen(filename);
    char const * name = filename;

    // set name of the parser
    if (strchr(name, '/')) {
        name = strrchr(name, '/');
    }
    if (strchr(name, '\\')) {
        name = strrchr(name, '\\');
    }
    if (strstr(name, ".grmr")) {
        name_length = (size_t)(strstr(name, ".grmr") - name);
    }
    CSVData csv_data = from_string(string, (size_t) file_size, filename, name_length, log_file, log_level);

    free(string);
    return csv_data;
}

int main(int narg, char ** args) {
    //printf("built!\n");
    char * input_file = NULL;
    char * log_file = NULL;
    unsigned char log_level = LOG_LEVEL_INFO;
    int iarg = 1;
    while (iarg < narg) {
        printf("arg %d: %s\n", iarg, args[iarg]);
        if (!timeit && !strcmp(args[iarg], "--timeit")) {
            timeit = true;
            iarg++;
            continue;
        }
        if (!input_file) {
            input_file = args[iarg++];
            continue;
        }
        if (!log_file) {
            log_file = args[iarg++];
            continue;
        }
        log_level = Logger_level_to_uchar(args[iarg], strlen(args[iarg]));
        iarg++;
    }
    if (input_file) {
        printf("processing file %s\n", input_file);
        CSVData csv_data = from_file(input_file, log_file, log_level);

        /* insert any code you want to handle the csv here */

        CSVParser_dest(&csv);
        CSVData_dest(&csv_data);
    }
    csv_dest();
    Logger_tear_down();
    return 0;
}