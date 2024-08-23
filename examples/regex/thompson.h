#ifndef THOMPSON_H
#define THOMPSON_H

#include <stddef.h>
#include <stdint.h>

#include "nfa.h"
#include "dfa.h"

#define STATE_BUF_TYPE unsigned char
#define STATE_BUF_TYPE_LOG2 3
typedef struct StateSet {
    STATE_BUF_TYPE * states;
    size_t states_size;             // this is the size of the array states, not the number of states
} StateSet;

#define StateSet_set(pStateSet, id) \
    ((pStateSet)->states[id >> STATE_BUF_TYPE_LOG2] |= (1 << (id % sizeof(STATE_BUF_TYPE))))

#define StateSet_is_set(pStateSet, id) \
    (((pStateSet)->states[id >> STATE_BUF_TYPE_LOG2] & (1 << (id % sizeof(STATE_BUF_TYPE)))) != 0)

int NFA_to_DFA(NFA * nfa, DFA * dfa);

#endif

