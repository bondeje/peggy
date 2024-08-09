#ifndef CPARSER_H
#define CPARSER_H 

#include <peggy/astnode.h>
#include <peggy/parser.h>
#include <peggy/rule.h>


#include "cparser.h"
#include "c.h"

ASTNode * simplify_binary_op(Production *, Parser *, ASTNode *);
ASTNode * _open_scope(Production * rule, Parser * parser, ASTNode * node);
ASTNode * _close_scope(Production * rule, Parser * parser, ASTNode * node);
ASTNode * c_process_declaration(Production * decl, Parser * parser, ASTNode * node);
ASTNode * c_process_declaration_specifiers(Production * decl_specs, Parser * parser, ASTNode * node);
ASTNode * c_check_typedef(Production * decl_specs, Parser * parser, ASTNode * node);
#endif

