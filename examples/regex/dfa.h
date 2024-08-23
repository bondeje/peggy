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
    Symbol * symbols;       // list of symbols in DFA language. owned by DFA if pool is non-null
    size_t nsymbols;        // number of symbols, also capacity
    DFAState * states;      // list of states in DFA. owned by DFA if pool is non-null
    size_t nstates;         // number of states, also capacity. NOTE: regex has to keep track of current one
    char const * regex_s;   // this is just for convenience. owned by DFA if pool is non-null
    size_t regex_len;       // this is just for convenience.
    MemPoolManager * pool;  // pool for all allocated data in dfa. if null, symbols and states are not owned by DFA

    // these should be for regex struct, not DFA
    char * buffer;          // buffer of characters passed to DFA
    size_t ibuffer;         // location of cursor in buffer
    size_t buffer_size;     // size of buffer
    size_t start;           // start of match
    size_t end;             // end of match (one past)
    unsigned int flags;     // flags for operation of the DFA. flags should account for the properties of the buffer (user provided)
};

#endif

