#ifndef CSVPARSER_H
#define CSVPARSER_H

#include "csv.h"

typedef struct CSVData {
    char * data; // holds all the string data (with null terminators)
    size_t * offsets;
    size_t ncols;
    size_t nrows;
    size_t nbytes; // accumulated during parsing. number of bytes allocated for data
} CSVData;

ASTNode * handle_csv(Production * csv_prod, Parser * parser, ASTNode * node);
ASTNode * process_string(Production * string_prod, Parser * parser, ASTNode * node);
ASTNode * process_nonstring(Production * nonstring_prod, Parser * parser, ASTNode * node);
ASTNode * process_record(Production * record_prod, Parser * parser, ASTNode * node);

CSVData from_string(char * string, size_t string_length, char * name, size_t name_length, char * log_file, unsigned char log_level);
CSVData from_file(char * filename, char * log_file, unsigned char log_level);

#endif // CSVPARSER_H

