#ifndef CSVPARSER_H
#define CSVPARSER_H

#include "csv.h"

ASTNode * handle_csv(Production * csv_prod, Parser * parser, ASTNode * node);
ASTNode * process_string(Production * string_prod, Parser * parser, ASTNode * node);
ASTNode * process_nonstring(Production * nonstring_prod, Parser * parser, ASTNode * node);
ASTNode * process_record(Production * record_prod, Parser * parser, ASTNode * node);

err_type from_string(char * string, size_t string_length, char * name, size_t name_length, char * log_file, unsigned char log_level);
err_type from_file(char * filename, char * log_file, unsigned char log_level);

#endif // CSVPARSER_H

