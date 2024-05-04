#ifndef CSVPARSER_H
#define CSVPARSER_H

#include "csv.h"

ASTNode * handle_csv(Production * csv_prod, Parser * parser, ASTNode * node);

err_type from_string(char const * string, size_t string_length, char const * name, size_t name_length, size_t * n_elements, double * time);
err_type from_file(char const * filename, size_t * n_elements, double * time);

#endif // CSVPARSER_H

