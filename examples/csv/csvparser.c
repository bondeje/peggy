#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// _POSIX_C_SOURCE is needed on gcc in linux when compiling with -std=c99 or really anything less than c11
//#if defined (__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ < 201112L)
//#define _POSIX_C_SOURCE 199506L
//#define _XOPEN_SOURCE 500
//#endif

#include <time.h>
#include <sys/types.h>

#include <logger.h>

#include "csvparser.h"

typedef struct CSVData {
    char * data; // holds all the string data (with null terminators)
    size_t * offsets;
    size_t ncols;
    size_t nrows;
    size_t nbytes; // accumulated during parsing. number of bytes allocated for data
} CSVData;

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

/*
err_type from_string(char const * string, size_t string_length, char const * name, size_t name_length, size_t * n_elem, double * time) {
    //printf("processing string:\n%s", string);
    if (time) {
        
        struct timespec t0, t1;
        clockid_t clk = CLOCK_MONOTONIC;
        //double clock_conversion = 1.0e-6;
        clock_gettime(clk, &t0);
        CSVParser_init(&csv, name, name_length, string, string_length);
       
        clock_gettime(clk, &t1);

        if (t1.tv_nsec < t0.tv_nsec) {
            *time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
        } else {
            *time = ((1 + 1.0e-9 * t1.tv_nsec) - 1.0e-9 * t0.tv_nsec) + t1.tv_sec - 1.0 - t0.tv_sec;
        }
        
    } else {
        CSVParser_init(&csv, name, name_length, string, string_length);
    }
    printf("number of nodes found: %zu\n", csv.Parser.node_list.fill);
    
    csv.Parser._class->dest((Parser *)&csv);
    if (n_elem) {
        *n_elem = csv.ncols * csv.nrows;
    }
    return PEGGY_SUCCESS;
}
*/

bool timeit = false;

err_type from_string(char * string, size_t string_length, char * name, size_t name_length, char * log_file, unsigned char log_level) {
    if (!timeit) {
        return CSVParser_init(&csv, name, name_length, string, string_length, log_file, log_level);
    }
    static char const * const record_format = "%zu, %10.8lf, %s\n";
    char buffer[1024] = {'\0'};
    FILE * time_records = fopen("times.csv", "ab");
    
    struct timespec t0, t1;
    double time;
    clockid_t clk = CLOCK_MONOTONIC;
    //double clock_conversion = 1.0e-6;
    clock_gettime(clk, &t0);
    err_type status = CSVParser_init(&csv, name, name_length, string, string_length, log_file, log_level);
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
    return status;
}

err_type from_file(char * filename, char * log_file, unsigned char log_level) {
    FILE * pfile = fopen(filename, "rb");
    if (!pfile) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - failed to open file %s\n", __func__, filename);
    }
    fseek(pfile, 0, SEEK_END);
    long file_size = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);

    char * string = malloc(file_size + 1);
    if (!string) {
        return PEGGY_MALLOC_FAILURE;
    }
    size_t nbytes = fread(string, 1, file_size, pfile);
    if (ferror(pfile)) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - failed to read file: %s\n", __func__, filename);
        free(string);
        return PEGGY_FILE_IO_ERROR;
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
    err_type status = from_string(string, (size_t) file_size, filename, name_length, log_file, log_level);
    if (status) {
        return status;
    }

    free(string);
    return PEGGY_SUCCESS;
}

int main(int narg, char ** args) {
    //printf("built!\n");
    char * input_file = NULL;
    char * log_file = NULL;
    unsigned char log_level = LOG_LEVEL_INFO;
    int iarg = 1;
    while (iarg < narg) {
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
        from_file(input_file, log_file, log_level);
        //printf("destroying CSVParser\n");
        //CSVParser_dest(&csv);
    }
    //printf("destroying csv\n");
    //csv_dest();
    //printf("tear down\n");
    //Logger_tear_down();
    return 0;
}