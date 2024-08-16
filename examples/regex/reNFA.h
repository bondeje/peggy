#ifndef RENFA_H
#define RENFA_H

#include <stddef.h>
#include "mempool.h"

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

// should never actually be allocated. more of an interface/abstract base class
struct reSymbol {
    int (*match)(reSymbol * sym, char const c);
    char const * sym;
    reSymbol * next; // next in linked list of symbols
};

// returns positive value on success, 0 on failure
int reChar_match(reSymbol * sym, char const c) {
    return *sym->sym == c;
}

int reChar_empty_match(reSymbol * sym, char const c) {
    return 1;
}

// file level
extern struct reSymbol empty_symbol;

// internal to char class
static inline int reRange_match(char const lower, char const upper, char const c) {
    return c <= upper && c >= lower;
}

// returns positive value on success, 0 on failure
int reCharClass_match(reSymbol * sym, char const c) { 
    reCharClass * class = (reCharClass *)sym;
    char const * start = sym->sym;
    char const * end = sym->sym + class->sym_length;
    int status = 0;
    while (start != end) {
        if (start + 1 != end && *(start + 1) == '-') {
            status = reRange_match(*start, *(start + 2), c);
        } else {
            status = (*start == c);
        }
        if (status) {
            return ~class->inv;
        }
        start++;
    }
    return class->inv;
}

struct reCharClass {
    reSymbol sym;
    _Bool inv;
    size_t sym_length; // char const * sym is interpreted as string between []
};

struct NFAState {
    NFATransition * out; // linked list of transitions out of the state
    NFATransition * in;
    size_t n_out;
    size_t n_in;
};

struct reNFA {
    MemPoolManager * pool;
    reSymbol * symbols; // linked list of symbols
    size_t nsymbols;
    size_t nstates;
    NFAState * start; // the initial state
    NFAState * final;
    unsigned int flags;
};

void reNFA_init(reNFA * nfa);

void reNFA_dest(reNFA * nfa);

#ifdef __STDC_VERSION__ < 201112L
struct NFATransition_ALIGNMENT {
    char a;
    NFATransition b;
};

struct reChar_ALIGNMENT {
    char a;
    reChar b;
};

struct reCharClass_ALIGNMENT {
    char a;
    reCharClass b;
};

struct NFAState_ALIGNMENT {
    char a;
    NFAState b;
};
#endif

#endif

