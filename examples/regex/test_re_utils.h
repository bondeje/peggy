#include <stddef.h>
#include <stdlib.h>

#include "nfa.h"
#include "dfa.h"
#include "avram.h"

typedef struct TestTransition {
    char const * symbol;
    size_t symbol_len;
    size_t state_end;
} TestTransition;

typedef struct TestState {
    size_t n_in;
    size_t n_out;
    struct TestTransition ** t_out;
} TestState;

typedef struct TestString {
    struct MatchString match;
    char const * cstr;
    unsigned int flags;
} TestString;

#define TEST_TRANSITION(symbol_, symbol_len_, state_end_) (struct TestTransition){ \
    .symbol = symbol_, \
    .symbol_len = symbol_len_,\
    .state_end = state_end_ \
}

#define TEST_STATE(n_in_, n_out_, ...) (struct TestState) {\
    .n_in = n_in_,\
    .n_out = n_out_,\
    .t_out = &(struct TestTransition * []){__VA_ARGS__}[0]\
}

int check_Transition(NFATransition * trans, TestTransition * ref_trans, size_t ref_state_start);

int check_State(NFAState * state, TestState ** ref_states, size_t ref_cur);

int check_NFA(NFA * nfa, TestState ** ref_states);

int check_regex(DFA * dfa, TestString * test);
