#ifndef P4CTRANSFORM_H
#define P4CTRANSFORM_H

#include "peg4cparser.h"
#include "peg4cstring.h"

P4CString get_rule_pointer(P4CParser * parser, P4CString name);

void handle_terminal(P4CParser * parser, ASTNode * node, const P4CString parent_id);

void handle_lookahead_rule(P4CParser * parser, ASTNode * node, P4CString name);

void handle_list_rule(P4CParser * parser, ASTNode * node, P4CString name);

void handle_repeated_rule(P4CParser * parser, ASTNode * node, P4CString name);

void handle_sequence(P4CParser * parser, ASTNode * node, P4CString name);

void handle_choice(P4CParser * parser, ASTNode * node, P4CString name);

void handle_base_rule(P4CParser * parser, ASTNode * node, const P4CString parent_id, P4CString name);

void handle_simplified_rules(P4CParser * parser, ASTNode * node, const P4CString parent_id, P4CString name);

void handle_production_(P4CParser * parser, ASTNode * id, ASTNode * transforms, ASTNode * rule_def, unsigned int flags);

void handle_production(P4CParser * parser, ASTNode * node);

P4CString format_regex(P4CParser * parser, char const * src, size_t src_length);

void handle_string_literal(P4CParser * parser, ASTNode * node, const P4CString parent_id);

void handle_regex_literal(P4CParser * parser, ASTNode * node, const P4CString parent_id);

void handle_punctuator_keyword(P4CParser * parser, ASTNode * node);

void handle_special_production(P4CParser * parser, ASTNode * node);

void handle_import(P4CParser * parser, ASTNode * node);

void handle_export(P4CParser * parser, ASTNode * node);

void handle_config(P4CParser * parser, ASTNode * node);

ASTNode * handle_peg4c(Production * peg4c_prod, Parser * parser, ASTNode * node);

ASTNode * simplify_rule(Production * simplifiable_rule, Parser * parser, ASTNode * node);

#endif

