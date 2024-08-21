#ifndef RENFA_H
#define RENFA_H

#include <stddef.h>
#include "reutils.h"
#include "peggy/mempool.h"

typedef struct NFATransition NFATransition;

typedef unsigned int NFA_state;

typedef struct reSymbol reSymbol;
typedef reSymbol reChar;
typedef struct reCharClass reCharClass;

typedef struct reNFA reNFA;

typedef struct NFAState NFAState;

struct NFATransition {
    NFAState * start; // not sure this one is necessary
    NFAState * final; // the final state of the transition
    reSymbol * symbol;
    NFATransition * next_in; // next in linked list of transitions in a given state
    NFATransition * next_out; // next in linked list of transition out of a 
};

BUILD_ALIGNMENT_STRUCT(NFATransition)

// should never actually be allocated. more of an interface/abstract base class
struct reSymbol {
    int (*match)(reSymbol * sym, char const c);
    char const * sym;
    size_t sym_len;
    reSymbol * next; // next in linked list of symbols
};

BUILD_ALIGNMENT_STRUCT(reSymbol)
BUILD_ALIGNMENT_STRUCT(reChar)

// returns positive value on success, 0 on failure
int reChar_match(reSymbol * sym, char const c);
int reChar_empty_match(reSymbol * sym, char const c);

// file level
extern struct reSymbol empty_symbol;

// internal to char class
static inline int reRange_match(char const lower, char const upper, char const c);
// returns positive value on success, 0 on failure
int reCharClass_match(reSymbol * sym, char const c);

struct reCharClass {
    reSymbol sym;
    _Bool inv;
};

BUILD_ALIGNMENT_STRUCT(reCharClass)

struct NFAState {
    NFATransition * out; // linked list of transitions out of the state
    NFATransition * in;
    size_t n_out;
    size_t n_in;
};

BUILD_ALIGNMENT_STRUCT(NFAState)

struct reNFA {
    MemPoolManager * pool;
    reSymbol * symbols; // linked list of symbols
    char const * regex_s;
    size_t regex_len;
    size_t nsymbols;
    size_t nstates;
    NFAState * start; // the initial state
    NFAState * final;
    unsigned int flags;
};

BUILD_ALIGNMENT_STRUCT(reNFA)

void reNFA_init(reNFA * nfa);

void reNFA_dest(reNFA * nfa);

#endif

