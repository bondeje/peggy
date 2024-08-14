#ifndef CPARSER_H
#define CPARSER_H 

#include <peggy/astnode.h>
#include <peggy/parser.h>
#include <peggy/rule.h>


#include "cparser.h"
#include "c.h"

ASTNode * nc1_pass0(Production * rule, Parser * parser, ASTNode * node);
ASTNode * c0nc0_pass1(Production * rule, Parser * parser, ASTNode * node);
ASTNode * simplify_binary_op(Production * binary_op, Parser * parser, ASTNode * node);
//ASTNode * b_to_end_of_line(Production * rule, Parser * parser, ASTNode * node);
ASTNode * _open_scope(Production * rule, Parser * parser, ASTNode * node);
ASTNode * _close_scope(Production * rule, Parser * parser, ASTNode * node);
ASTNode * c_process_declaration(Production * decl, Parser * parser, ASTNode * node);
ASTNode * c_process_declaration_specifiers(Production * decl_specs, Parser * parser, ASTNode * node);
ASTNode * c_check_typedef(Production * decl_specs, Parser * parser, ASTNode * node);
#endif

