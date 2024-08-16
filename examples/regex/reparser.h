#ifndef REPARSER_H
#define REPARSER_H

#include "re.h"
#include "reNFA.h"

ASTNode * re_pass(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_symbol(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_char_class(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_choice_states(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_repeat_states(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_sequence_states(Production * prod, Parser * parser, ASTNode * node);

typedef struct RegexBuilder {
    Parser parser;
    reNFA nfa; // nfa is built while parser
} RegexBuilder;

void RegexBuilder_init(RegexBuilder * reb);

void RegexBuilder_build(RegexBuilder * reb, char * regex_s, size_t regex_len);

void RegexBuilder_dest(RegexBuilder * reb);

#endif

