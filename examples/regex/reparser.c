#include <string.h>

#include "reparser.h"
//#include "reNFA.h"

typedef struct NFANode {
    ASTNode node;
    NFAState * start;   // the initial state of the subNFA
    NFAState * final;     // the final state of the subNFA
} NFANode;

#ifdef __STDC_VERSION__ < 201112L
struct NFANode_ALIGNMENT {
    char a;
    NFANode b;
};
#endif

ASTNode * re_pass(Production * prod, Parser * parser, ASTNode * node) {
    if (node->nchildren > 1) {
        return node->children[1];
    }
    return node->children[0];
}

ASTNode * re_build_symbol(Production * prod, Parser * parser, ASTNode * node) {
    static char const reESCAPED_CHARS[] = "'\"?\\abfnrtv";
    static char const ESCAPED_CHARS[] = "\'\"\?\\\a\b\f\n\r\t\v";
    RegexBuilder * reb = (RegexBuilder *)parser;
    reSymbol * new_sym;
    // build a new symbol
    if (node->children[0]->rule->id == CHAR_CLASS) {
        reCharClass * class = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(*class), _Alignof(reCharClass));
        class->sym.match = reCharClass_match;
        class->sym.next = reb->nfa.symbols;
        class->inv = node->token_start->string[1] == '^';
        class->sym.sym = node->token_start->string + (class->inv ? 2 : 1);
        class->sym_length = (size_t)(node->token_end->string - class->sym.sym) + node->token_end->length - 1; 
        reb->nfa.symbols = (reSymbol *)class;
        new_sym = (reSymbol *)class;
    } else { // single_char
        reChar * ch = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(*ch), _Alignof(reChar));
        ch->match = reChar_match;
        ch->next = reb->nfa.symbols;
        reb->nfa.symbols = (reSymbol *)ch;
        if (node->children[0]->token_start->string[0] == '\\') { // escaped character
            char const * loc = strchr(reESCAPED_CHARS, node->children[0]->token_start->string[1]);
            if (loc) {
                ch->sym = ESCAPED_CHARS + (loc - reESCAPED_CHARS);
            } else {
                ch->sym = node->children[0]->token_start->string + 1;
            }
        } else {
            ch->sym = node->children[0]->token_start->string;
        }
        new_sym = (reSymbol *)ch;
    }
    // record signal in nfa
    reb->nfa.symbols = new_sym;
    reb->nfa.nstates++;

    NFATransition * trans = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(NFATransition), _Alignof(NFATransition));
    
    // build and initialize start and final state for a transition using the symbol
    NFAState * final = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(NFAState), _Alignof(NFAState));
    NFAState * start = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(NFAState), _Alignof(NFAState));
    *start = (NFAState) {.n_out = 1, .n_in = 0, .out = trans};
    *final = (NFAState) {.n_out = 0, .n_in = 1, .in = trans};
    reb->nfa.nstates += 2;

    // initialize transition for start to final state
    *trans = (NFATransition) {.final = final, .next_in = NULL, .next_out = NULL, .start = start, .symbol = new_sym};

    NFANode * subnfa = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(NFANode), _Alignof(NFANode));
    *subnfa = (NFANode) {.node = *node, .start = start, .final = final};
    return (ASTNode *)subnfa;
}

NFATransition * re_build_empty_transition(RegexBuilder * reb, NFAState * start, NFAState * final) {
    NFATransition * trans = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(NFATransition), _Alignof(NFATransition));
    *trans = (NFATransition) {.final = final, .start = start, .next_in = final->in, .next_out = start->out, .symbol= &empty_symbol};
    final->in = trans;
    final->out = trans;
    return trans;
}

ASTNode * re_build_choice_states(Production * prod, Parser * parser, ASTNode * node) {
    if (node->nchildren == 1) {
        return node->children[0];
    }

    RegexBuilder * reb = (RegexBuilder *)parser;
    NFAState * final = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(NFAState), _Alignof(NFAState));
    NFAState * start = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(NFAState), _Alignof(NFAState));
    *start = (NFAState) {0};
    *final = (NFAState) {0};

    size_t i = node->nchildren;
    NFATransition * cur_trans = NULL;
    while (i--) {
        NFANode * subnfa = (NFANode *)node;
        cur_trans = re_build_empty_transition(reb, start, subnfa->start, cur_trans);
        re_build_empty_transition(reb, subnfa->final, final);
        i--;
    }
    reb->nfa.nstates += 2;
}

void re_build_optional(RegexBuilder * reb, NFAState * start, NFANode * subnfa, NFAState * final) {
    re_build_empty_transition(reb, start, final);
    re_build_empty_transition(reb, start, subnfa->start);
    re_build_empty_transition(reb, subnfa->final, final);
    subnfa->start = start;
    subnfa->final = final;
}

NFAState * re_copy_transition(RegexBuilder * reb, NFATransition * src) {
    // TODO
    return NULL;
}

NFAState * re_copy_state(RegexBuilder * reb, NFAState * src) {
    // TODO
    return NULL;
}

NFANode * re_copy_nfa(RegexBuilder * reb, NFAState * start, NFANode * nfa, NFAState * final) {
    // TODO
    return NULL;
}

void re_build_kleene(RegexBuilder * reb, NFAState * start, NFANode * subnfa, NFAState * final) {
    re_build_empty_transition(reb, start, final);
    re_build_empty_transition(reb, start, subnfa->start);
    re_build_empty_transition(reb, subnfa->final, final);
    re_build_empty_transition(reb, subnfa->final, subnfa->start);
    subnfa->start = start;
    subnfa->final = final;
}

void re_merge_states(NFAState * start, NFAState * final) {
    // the start is the final node in a subNFA, which should have no transitions out
    // the final is the start node of a subsequence NFA, which should have all the transitions
    // for each transition in final, set the start state to start

    // handle final->out by setting the starts to start
    NFATransition * trans = final->out;
    NFATransition * trans_start = trans;
    trans->start = start;
    while (trans->next_out) {
        trans = trans->next_out;
        trans->start = start;        
    }
    trans->next_out = start->out;
    start->out = trans_start;
    start->n_out += final->n_out;

    // handle final->in by setting the finals to start
    trans = final->in;
    trans_start = trans;
    trans->final = start;
    while (trans->next_in) {
        trans = trans->next_in;
        trans->final = start;
    }
    trans->next_in = start->in;
    start->in = trans_start;
    start->n_in += final->n_in;
}

ASTNode * re_build_repeat_states(Production * prod, Parser * parser, ASTNode * node) {
    if (node->children[1]->nchildren) {
        return node->children[0];
    }

    ASTNode * operand = node->children[1]->children[0];

    RegexBuilder * reb = (RegexBuilder *)parser;
    
    size_t i = node->nchildren;
    NFATransition * cur_trans = NULL;

    if (operand->rule->id == ASTERISK 
        || (operand->children[2]->nchildren && !operand->children[1]->nchildren && !operand->children[3]->nchildren)) {

        NFAState * final = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(NFAState), _Alignof(NFAState));
        NFAState * start = MemPoolManager_aligned_alloc(reb->nfa.pool, sizeof(NFAState), _Alignof(NFAState));
        *start = (NFAState) {0};
        *final = (NFAState) {0};

        re_build_kleene(reb, start, (NFANode *)node->children[0], final);

    } else if (operand->rule->id == PLUS 
        || (operand->children[2]->nchildren && !operand->children[3]->nchildren)) {

        // TODO sequence and then kleene star

    } else { // sequence

    }

    return node;
}

ASTNode * re_build_sequence_states(Production * prod, Parser * parser, ASTNode * node) {
    if (node->nchildren == 1) {
        return node->children[0];
    }

    RegexBuilder * reb = (RegexBuilder *)parser;
    
    NFANode * subnfa = NULL;
    NFANode * nextnfa = (NFANode *)node->children[0];
    NFAState * start = nextnfa->start;
    for (size_t i = 1; i < node->nchildren; i++) {
        subnfa = nextnfa;
        NFANode * nextnfa = (NFANode *)node->children[i];
        re_merge_states(subnfa->final, nextnfa->start);
    }

    ((NFANode *)node)->start = start;
    ((NFANode *)node)->final = nextnfa->final;
    return node;
}

void RegexBuilder_init(RegexBuilder * reb) {
    // build and initialize an empy symbol transition
}



void RegexBuilder_build(RegexBuilder * reb, char * regex_s, size_t regex_len);

void RegexBuilder_dest(RegexBuilder * reb);

