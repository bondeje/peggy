#ifndef REPARSER_H
#define REPARSER_H

#include <string.h>

#include "re.h"
#include "reutils.h"
#include "nfa.h"
#include "dfa.h"

typedef struct RegexBuilder {
    Parser parser;
    NFA * nfa; // nfa is built while parser
} RegexBuilder;

typedef struct NFANode {
    ASTNode node;
    NFAState * start;   // the initial state of the subNFA
    NFAState * final;     // the final state of the subNFA
} NFANode;

BUILD_ALIGNMENT_STRUCT(NFANode)

ASTNode * re_pass(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_symbol(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_lookahead(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_check_range(Production * prod, Parser * parser, ASTNode * node);
NFATransition * re_build_empty_transition(RegexBuilder * reb, NFAState * start, NFAState * final);
ASTNode * re_build_choice(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_optional(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_kleene(Production * prod, Parser * parser, ASTNode * node);
void re_merge_states(NFAState * start, NFAState * final);
ASTNode * re_build_sequence(Production * prod, Parser * parser, ASTNode * node);
ASTNode * re_build_nfa(Production * prod, Parser * parser, ASTNode * node);

char * RegexBuilder_realloc_buffer(RegexBuilder * reb, char * buf, size_t * buf_len, size_t new_size);
char * RegexBuilder_preprocess_repeatopt(RegexBuilder * reb, char * buf, size_t * buf_len, char const * base, size_t base_length, size_t nrepeats, size_t * loc);
char * RegexBuilder_preprocess_repeat(RegexBuilder * reb, char * buf, size_t * buf_len, char const * base, size_t base_length, size_t nrepeats, size_t * loc);
void RegexBuilder_preprocess(RegexBuilder * reb, char const * regex_s, size_t regex_len);

void RegexBuilder_init(RegexBuilder * reb, NFA * nfa);
NFA * RegexBuilder_build_NFA(RegexBuilder * reb, char const * regex_s, size_t regex_len);
int RegexBuilder_build(RegexBuilder * reb, char const * regex_s, size_t regex_len, DFA * dfa);
void RegexBuilder_dest(RegexBuilder * reb);

#endif

