/* C standard library includes */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* POSIX  includes */
#include <time.h>
#include <sys/types.h>

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

err_type CSVParser_init(CSVParser * parser, char const * string, size_t string_length) {
    parser->csv.data = string;
    parser->csv.offsets = NULL;
    parser->csv.ncols = 0;
    parser->csv.nrows = 0;
    parser->csv.noffsets = 0;
    parser->csv.nbytes = string_length;
    Parser_init((Parser *)parser, csvrules, CSV_NRULES, CSV_TOKEN, CSV_CSV, 0);
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
        return Parser_fail_node(parser);
    }

    csv->csv.nrows++;
    return build_action_default(record_prod, parser, node);
}

void handle_field(CSVParser * csv_parser, ASTNode * node, size_t index) {
    Token * tok = node->token_start;
    size_t N = 0;
    CSVData * data = &csv_parser->csv;
    if (node->children[0]->rule == CSV_NONSTRING_FIELD) {
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
    for (size_t i = 0; i < node->nchildren; i += 2) {
        handle_field(csv_parser, node->children[i], index++);
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

    ASTNode * record_list = node->children[0];
    size_t Nchild = record_list->nchildren;
    size_t row = 0;
    for (size_t i = 0; i < record_list->nchildren; i += 2) {
        handle_record(csv, record_list->children[i], row++);
    }
    return node;
}

/***************** globals and driver *******************/

CSVParser csv = {
    .Parser = {
        ._class = &Parser_class,
    }, 
    .csv = {0}
};
struct CSVData empty_csv = {0};
bool timeit = false;

CSVData from_string(char * string, size_t string_length) {
    err_type status = PEGGY_SUCCESS;
    if (!timeit) {
        if ((status = CSVParser_init(&csv, string, string_length))) {
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
    status = CSVParser_init(&csv, string, string_length);
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
    fseek(pfile, 0, SEEK_END);
    long file_size = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);

    char * string = malloc(file_size + 1);
    if (!string) {
        return empty_csv;
    }
    size_t nbytes = fread(string, 1, file_size, pfile);
    if (ferror(pfile)) {
        free(string);
        return empty_csv;
    }
    string[file_size] = '\0';

    fclose(pfile);

    csv.csv.isalloc = true;
    CSVData csv_data = from_string(string, (size_t) file_size);
    return csv_data;
}

void print_row(CSVData * data, size_t row) {
    size_t index = row * data->ncols;
    printf("row %zu: = \"%.*s\"", row, (int)(data->offsets[index + data->noffsets] - data->offsets[index]), data->data + data->offsets[index]);
    for (size_t i = 1; i < data->ncols; i++) {
        index++;
        printf(", \"%.*s\"", (int)(data->offsets[index + data->noffsets] - data->offsets[index]), data->data + data->offsets[index]);
    }
    printf("\n");
}

void print_row_col(CSVData * data, size_t row, size_t col) {
    size_t index = row * data->ncols + col;
    printf("row, col: %zu, %zu = \"%.*s\"\n", row, col, (int)(data->offsets[index + data->noffsets] - data->offsets[index]), data->data + data->offsets[index]);
}

void print_first_last_row(CSVData * data) {
    size_t index = 0;
    
    index = (data->nrows - 1) * data->ncols;
    printf("\nlast row: %.*s", (int)(data->offsets[index + data->noffsets] - data->offsets[index]), data->data + data->offsets[index]);
    for (size_t i = 1; i < data->ncols; i++) {
        index++;
        printf(", %.*s", (int)(data->offsets[index + data->noffsets] - data->offsets[index]), data->data + data->offsets[index]);
    }

    printf("\n");
}

int main(int narg, char ** args) {
    long long int row = -1;
    long long int col = -1;
    int iarg = 1;
    while (iarg < narg) {
        //printf("arg %d: %s\n", iarg, args[iarg]);
        if (!strcmp(args[iarg], "--timeit")) {
            timeit = true;
        } else if (!strncmp(args[iarg], "-r=", 3)) {
            row = atoll(args[iarg] + 3);
        } else if (!strncmp(args[iarg], "-c=", 3)) {
            col = atoll(args[iarg] + 3);
        } else {
            csv.csv.isalloc = false;
            printf("processing file %s\n", args[iarg]);
            CSVData csv_data = from_file(args[iarg]);
            if (row >= 0) {
                if ((size_t)row >= csv_data.nrows) {
                    printf("row out of bounds. max = %zu\n", csv_data.nrows-1);
                } else {
                    if (col >= 0) {
                        if ((size_t)col < csv_data.ncols) {
                            print_row_col(&csv_data, (size_t)row, (size_t)col);
                        } else {
                            printf("col out of bounds. max = %zu\n", csv_data.ncols-1);
                        }
                        
                    } else {
                        print_row(&csv_data, (size_t)row);
                    }
                }
                
            }
            CSVParser_dest(&csv);
            CSVData_dest(&csv_data);
        }
        iarg++;
    }
    csv_dest();
    return 0;
}

