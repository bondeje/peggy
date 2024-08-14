#ifndef PEGGYTRANSFORM_H
#define PEGGYTRANSFORM_H

#include "peggyparser.h"
#include "peggystring.h"

PeggyString get_rule_pointer(PeggyParser * parser, PeggyString name);

unsigned char size_t_strlen(size_t val);

void handle_terminal(PeggyParser * parser, ASTNode * node, const PeggyString parent_id);

void handle_lookahead_rule(PeggyParser * parser, ASTNode * node, PeggyString name);

void handle_list_rule(PeggyParser * parser, ASTNode * node, PeggyString name);

void handle_repeated_rule(PeggyParser * parser, ASTNode * node, PeggyString name);

void handle_sequence(PeggyParser * parser, ASTNode * node, PeggyString name);

void handle_choice(PeggyParser * parser, ASTNode * node, PeggyString name);

void handle_base_rule(PeggyParser * parser, ASTNode * node, const PeggyString parent_id, PeggyString name);

void handle_simplified_rules(PeggyParser * parser, ASTNode * node, const PeggyString parent_id, PeggyString name);

void handle_production_(PeggyParser * parser, ASTNode * id, ASTNode * transforms, ASTNode * rule_def, unsigned int flags);

void handle_production(PeggyParser * parser, ASTNode * node);

PeggyString format_regex(PeggyParser * parser, char const * src, size_t src_length);

void handle_string_literal(PeggyParser * parser, ASTNode * node, const PeggyString parent_id);

void handle_regex_literal(PeggyParser * parser, ASTNode * node, const PeggyString parent_id);

void handle_punctuator_keyword(PeggyParser * parser, ASTNode * node);

void handle_special_production(PeggyParser * parser, ASTNode * node);

void handle_import(PeggyParser * parser, ASTNode * node);

void handle_export(PeggyParser * parser, ASTNode * node);

void handle_config(PeggyParser * parser, ASTNode * node);

ASTNode * handle_peggy(Production * peggy_prod, Parser * parser, ASTNode * node);

ASTNode * simplify_rule(Production * simplifiable_rule, Parser * parser, ASTNode * node);

#endif

