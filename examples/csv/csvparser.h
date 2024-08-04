#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <stddef.h>
#include <stdint.h>

#include <peggy/parser.h>
#include <peggy/rule.h>
#include <peggy/astnode.h>

typedef struct CSVData {
    char const * data; // holds all the string data (with null terminators)
    ptrdiff_t * offsets;
    size_t ncols;
    size_t nrows;
    size_t noffsets;
    size_t nbytes; // accumulated during parsing. number of bytes allocated for data
    _Bool isalloc;
} CSVData;

ASTNode * handle_csv(Production * csv_prod, Parser * parser, ASTNode * node);
ASTNode * process_record(Production * record_prod, Parser * parser, ASTNode * node);

CSVData from_string(char * string, size_t string_length);
CSVData from_file(char * filename);

#endif // CSVPARSER_H

