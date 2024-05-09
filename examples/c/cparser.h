#ifndef CPARSER_H
#define CPARSER_H 

#include <peggy/astnode.h>
#include <peggy/parser.h>
#include <peggy/rule.h>


#include "cparser.h"
#include "c.h"

ASTNode * handle_c(Production *, Parser *, ASTNode *);
ASTNode * directive_line(Production *, Parser *, ASTNode *);

#endif