/* C standard library includes */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* POSIX  includes */
#include <time.h>
#include <sys/types.h>

/* external lib includes */
#include <logger.h>

/* local includes */
#include "csvparser.h"
#include "csv.h"

void CSVData_dest(CSVData * csv_data) {
    if (csv_data->isalloc && csv_data->data) {
        free((void *)csv_data->data);
        csv_data->data = NULL;
        csv_data->isalloc = false;
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
    parser->csv.data = string;
    parser->csv.offsets = NULL;
    parser->csv.ncols = 0;
    parser->csv.nrows = 0;
    parser->csv.noffsets = 0;
    parser->csv.nbytes = string_length;
    Parser_init((Parser *)parser, name, name_length, (Rule *)&csv_token, (Rule *)&csv_csv, CSV_NRULES, 0, log_file, log_level);
    Parser_parse((Parser *)parser, string, string_length);
    return 0;
}

/* does not destroy the CSVData, which is expected to be returned/used elsewhere */
void CSVParser_dest(CSVParser * parser) {
    Parser_dest((Parser *)parser);
}

ASTNode * process_record(Production * record_prod, Parser * parser, ASTNode * node) {
    CSVParser * csv = (CSVParser *) parser;
    size_t ncols = (node->nchildren + 1) >> 1;
    
    if (!csv->csv.ncols) {
        csv->csv.ncols = ncols;
    } else if (csv->csv.ncols != ncols) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - csv parsing error: found a different number of columns in row %zu compared to prior rows - found %zu, expected %zu\n", __func__, csv->csv.nrows, ncols, csv->csv.ncols);
        return Parser_fail_node(parser);
    }

    csv->csv.nrows++;
    return build_action_default(record_prod, parser, node);
}

void handle_field(CSVParser * csv_parser, ASTNode * node, size_t index) {
    Token * tok = node->token_start;
    size_t N = 0;
    CSVData * data = &csv_parser->csv;
    if (node->child->rule->id == NONSTRING_FIELD) {
        data->offsets[index] = tok->string - data->data;
        data->offsets[index + data->noffsets] = data->offsets[index] + node->str_length;
    } else { // STRING
        data->offsets[index] = (tok->string - data->data) + 1;
        data->offsets[index + data->noffsets] = data->offsets[index] + node->str_length - 2;
    }
}

void handle_record(CSVParser * csv_parser, ASTNode * node, size_t row) {
    size_t Nchild = node->nchildren;
    size_t index = row * csv_parser->csv.ncols;
    for (ASTNode * child = node->child; child ; child = child->next ? child->next->next : NULL) {
        handle_field(csv_parser, child, index++);
    }
}

ASTNode * handle_csv(Production * csv_prod, Parser * parser, ASTNode * node) {
    CSVParser * csv = (CSVParser *) parser;
    csv->csv.noffsets = (csv->csv.ncols * csv->csv.nrows + 1);
    csv->csv.offsets = malloc(sizeof(*csv->csv.offsets) * csv->csv.noffsets * 2); // add one to be able to set one past
    if (!csv->csv.offsets) {
        return Parser_fail_node(parser);
    }

    csv->csv.offsets[0] = 0;

    ASTNode * record_list = node->child;
    size_t Nchild = record_list->nchildren;
    size_t row = 0;
    for (ASTNode * child = record_list->child; child; child = child->next ? child->next->next : NULL) {
        handle_record(csv, child, row++);
    }
    return node;
}

/***************** globals and driver *******************/

CSVParser csv = {
    .Parser = {
        ._class = &Parser_class,
        .logger = DEFAULT_LOGGER_INIT,
    }, 
    .csv = {0}
};
struct CSVData empty_csv = {0};
bool timeit = false;

CSVData from_string(char * string, size_t string_length) {
    err_type status = PEGGY_SUCCESS;
    if (!timeit) {
        if ((status = CSVParser_init(&csv, NULL, 0, string, string_length, NULL, LOG_LEVEL_DISABLE))) {
            return empty_csv;
        }
        return csv.csv;
    }
#ifdef CLOCK_MONOTONIC
    static char const * const record_format = "%zu, %10.8lf\n";
    char buffer[1024] = {'\0'};
    FILE * time_records = fopen("times.csv", "ab");
    
    struct timespec t0, t1;

    double time;
    clockid_t clk = CLOCK_MONOTONIC;
    //double clock_conversion = 1.0e-6;
    clock_gettime(clk, &t0);
    status = CSVParser_init(&csv, NULL, 0, string, string_length, NULL, LOG_LEVEL_DISABLE);
    clock_gettime(clk, &t1);

    if (t1.tv_nsec < t0.tv_nsec) {
        time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
    } else {
        time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
    }

    snprintf(buffer, 1024, record_format, csv.csv.ncols*csv.csv.nrows, time);
    fprintf(time_records, "%s", buffer);
    printf("%s", buffer);    
    fclose(time_records);
#endif
    return csv.csv;
}

CSVData from_file(char * filename) {
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

    /*
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
    */
    csv.csv.isalloc = true;
    CSVData csv_data = from_string(string, (size_t) file_size);
    return csv_data;
}

void print_first_last_row(CSVData * data) {
    size_t index = 0;
    printf("first row: %.*s", (int)(data->offsets[index + data->noffsets] - data->offsets[index]), data->data + data->offsets[index]);
    for (size_t i = 1; i < data->ncols; i++) {
        index++;
        printf(", %.*s", (int)(data->offsets[index + data->noffsets] - data->offsets[index]), data->data + data->offsets[index]);
    }
    index = (data->nrows - 1) * data->ncols;
    printf("\nlast row: %.*s", (int)(data->offsets[index + data->noffsets] - data->offsets[index]), data->data + data->offsets[index]);
    for (size_t i = 1; i < data->ncols; i++) {
        index++;
        printf(", %.*s", (int)(data->offsets[index + data->noffsets] - data->offsets[index]), data->data + data->offsets[index]);
    }

    printf("\n");
}

int main(int narg, char ** args) {
    char * log_file = NULL;
    unsigned char log_level = LOG_LEVEL_INFO;
    int iarg = 1;
    while (iarg < narg) {
        printf("arg %d: %s\n", iarg, args[iarg]);
        if (!strcmp(args[iarg], "--timeit")) {
            timeit = true;
        } else if (!strncmp(args[iarg], "--log=", 6)) {
            log_file = args[iarg] + 6;
        } else if (!strncmp(args[iarg], "--log_level=", 12)) {
            log_level = Logger_level_to_uchar(args[iarg] + 12, strlen(args[iarg] + 12));
        } else {
            csv.csv.isalloc = false;
            printf("processing file %s\n", args[iarg]);
            CSVData csv_data = from_file(args[iarg]);
            //print_first_last_row(&csv_data);
            /* insert any code you want to handle the csv here */
            CSVParser_dest(&csv);
            CSVData_dest(&csv_data);
        }
        iarg++;
    }
    csv_dest();
    Logger_tear_down();
    return 0;
}

