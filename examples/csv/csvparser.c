#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

// _POSIX_C_SOURCE is needed on gcc in linux when compiling with -std=c99 or really anything less than c11
//#if defined (__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ < 201112L)
//#define _POSIX_C_SOURCE 199506L
//#define _XOPEN_SOURCE 500
//#endif

#include <time.h>
#include <sys/types.h>

#include "csvparser.h"

typedef struct Cell {
    char * start;
    unsigned short len;
} Cell;

typedef struct CSVParser {
    Parser Parser;
    Cell * data;
    size_t ncols;
    size_t nrows;
} CSVParser;

err_type CSVParser_init(CSVParser * parser, char const * name, size_t name_length,
                         char const * string, size_t string_length) {
    parser->data = NULL;
    parser->ncols = 0;
    parser->nrows = 0;
    parser->Parser._class->init(&parser->Parser, name, name_length, string, string_length, &csv_token.AnonymousProduction.DerivedRule.Rule, &csv_csv.AnonymousProduction.DerivedRule.Rule, CSV_NRULES, 0, 0, false, NULL);
    return PEGGY_SUCCESS;
}

void CSVParser_dest(CSVParser * parser) {
    free(parser->data);
    parser->data = NULL;
    parser->ncols = 0;
    parser->nrows = 0;
    parser->Parser._class->dest(&parser->Parser);
}

CSVParser csv = {
    .Parser = {
        ._class = &Parser_class,
        .token_rule = NULL,
        .root_rule = NULL,
        .name = "",
        .log_buffer = NULL,
        .log_buffer_length = 0,
        .log_file = NULL,
        .loc_ = 0,
        .disable_cache_check = false,
        .ast = NULL
    }, 
    .data = NULL, 
    .ncols = 0, 
    .nrows = 0};

void handle_field(CSVParser * parser, ASTNode * node, size_t row, size_t col) {
    size_t ntokens;
    Token ** toks = parser->Parser._class->get_tokens(&parser->Parser, node, &ntokens);

    if (node->children[0]->rule->id == STRING) {
        parser->data[row * parser->ncols + col] = (Cell){.start = (char *)(toks[0]->string + toks[0]->start + 1), toks[ntokens-1]->end - toks[0]->start - 2};
    } else {
        parser->data[row * parser->ncols + col] = (Cell){.start = (char *)(toks[0]->string + toks[0]->start), toks[ntokens-1]->end - toks[0]->start};
    }
}

void handle_record(CSVParser * parser, ASTNode * node, size_t row) {
    size_t N = node->nchildren;
    size_t j = 0;
    for (size_t i = 0; i < N; i += 2) {
        handle_field(parser, node->children[i], row, j);
        j++;
    }
}

ASTNode * handle_csv(Parser * parser, ASTNode * node) {
    
    if (node == &ASTNode_fail) {
        return node;
    }
    CSVParser * csv = (CSVParser *) parser;
    ASTNode * record_list = node->children[0];
    csv->nrows = (record_list->nchildren + 1) / 2;
    csv->ncols = (record_list->children[0]->nchildren + 1) / 2;
    /*
    csv->data = malloc(sizeof(*(csv->data)) * csv->nrows * csv->ncols);
    
    size_t N = record_list->nchildren;
    size_t j = 0;
    for (size_t i = 0; i < N; i += 2) {
        handle_record(csv, record_list->children[i], j);
        j++;
    }
    printf("succeeded parsing %s\n", parser->name);
    */
    return node;
}

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
    

    if (n_elem) {
        *n_elem = csv.ncols * csv.nrows;
    }
    //printf("done\n");
    return PEGGY_SUCCESS;
}

err_type from_file(char const * filename, size_t * n_elem, double * time) {
    //printf("processing from file %s\n", filename);
    FILE * pfile = fopen(filename, "rb");
    if (!pfile) {
        printf("failed to open file: %s\n", filename);
    }
    fseek(pfile, 0, SEEK_END);
    long file_size = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);

    char * string = malloc(file_size + 1);
    if (!string) {
        printf("failed to malloc sufficient data\n");
        return PEGGY_MALLOC_FAILURE;
    }
    size_t nbytes = fread(string, 1, file_size, pfile);
    if (ferror(pfile)) {
        printf("error occurred in reading file: %s\n", filename);
        free(string);
        return PEGGY_FILE_IO_ERROR;
    }
    string[file_size] = '\0';

    fclose(pfile);

    size_t name_length = strlen(filename);
    char const * name = filename;
    if (strchr(name, '/')) {
        name = strrchr(name, '/');
    }
    if (strchr(name, '\\')) {
        name = strrchr(name, '\\');
    }
    if (strstr(name, ".grmr")) {
        name_length = (size_t)(strstr(name, ".grmr") - name);
    }
    err_type status = from_string(string, (size_t) file_size, filename, name_length, n_elem, time);
    if (status) {
        return status;
    }

    free(string);
    return PEGGY_SUCCESS;
}


int main(int narg, char ** args) {
    char ** filenames = NULL;
    unsigned int nfiles = 0;
    bool timeit = false;
    FILE * time_records;
    if (narg > 1) {
        if (!strcmp("--timeit", args[1])) {
            timeit = true;
            if (narg > 2) {
                filenames = args + 2;
                nfiles = narg - 2;
            }            
        } else {
            filenames = args + 1;
            nfiles = narg - 1;
        }
        
    }

    nfiles = (nfiles < 18) ? nfiles : 18;
    if (timeit) {
        char buffer[1026] = {'\0'};
        int length = 0;
        FILE * time_records = fopen("times.csv", "w");
        for (unsigned int i = 0; i < nfiles; i++) {
            
            //char buffer[1026];
            printf("processing %s...", filenames[i]);
            double time;
            size_t n_elements;
            from_file(filenames[i], &n_elements, &time);
            //CSVParser_dest(&csv);

            if ((length = snprintf(buffer, 1026, "%zu, %10.8lf, %s\n", n_elements, time, filenames[i])) > 0) {
                fwrite(buffer, sizeof(char), length, time_records);
            }
            printf("%s", buffer);

        }
        
        fclose(time_records);
    } else {
        for (unsigned int i = 0; i < nfiles; i++) {
            printf("processing %s...", filenames[i]);
            size_t n_elements;
            from_file(filenames[i], &n_elements, NULL);
            printf("\tn_elements %zu\n", n_elements);
            //CSVParser_dest(&csv);
        }
    }
    csv_dest();
    return 0;
}