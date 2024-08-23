#ifndef REDFA_H
#define REDFA_H

#include <stddef.h>
#include "peggy/mempool.h"
#include "nfa.h"

typedef struct DFAState DFAState;
typedef struct DFATransition DFATransition;
typedef struct DFA DFA;

struct DFAState {
    DFATransition * trans; // array of transitions out of state
    _Bool accepting;
};

BUILD_ALIGNMENT_STRUCT(DFAState)

struct DFATransition {
    Symbol * sym;
    size_t final_state;
    DFATransition * next;
};

BUILD_ALIGNMENT_STRUCT(DFATransition)

// regex structures would be a copy of the singleton constructed from this
struct DFA {
    MemPoolManager * pool;  // pool for all allocated data in dfa. if null, symbols and states are not owned by DFA
    Symbol * symbols;       // list of symbols in DFA language. owned by DFA if pool is non-null
    DFAState * states;      // list of states in DFA. owned by DFA if pool is non-null
    char const * regex_s;   // this is just for convenience. owned by DFA if pool is non-null
    int regex_len;          // this is just for convenience.
    int nstates;            // number of states, also capacity. NOTE: regex has to keep track of current one    
    int nsymbols;           // number of symbols, also capacity
};

// takes DFA and index and returns a const pointer to a state
#define DFA_get_state(pDFA, index) ((DFAState const *)((pDFA)->states + (index)))

// an out-of bounds is an accepting state that is an internal error
#define DFA_is_accepting(pDFA, index) ((pDFA)->states + index)->accepting

// returns 0 on success, else error
int DFA_check(DFAState const * cur_state, char const * str, int const len, int * final);
void DFA_dest(DFA * dfa);

#endif
