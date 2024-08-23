#include <string.h>
#include <stdio.h>

#include "test_utils.h"
#include "test_re_utils.h"

int check_Symbol(Symbol * sym, char const * str, size_t len) {
    int nerrors = 0;
    nerrors += CHECK(sym->sym_len == len, "symbol length mismatch. expected: %zu, found: %zu\n", sym->sym_len, len);
    nerrors += CHECK(!strncmp(sym->sym, str, len), "symbol mismatch. expected: %.*s, found: %.*s\n", (int)len, str, (int)sym->sym_len, sym->sym);
    return nerrors;
}

int check_Transition(NFATransition * trans, TestTransition * ref_trans, size_t ref_state_start) {
    int nerrors = 0;
    nerrors += check_Symbol(trans->symbol, ref_trans->symbol, ref_trans->symbol_len);
    CHECK(!nerrors, "transition with symbol %.*s from state %zu to state %zu\b", (int)ref_trans->symbol_len, ref_trans->symbol, ref_state_start, ref_trans->state_end);
    return nerrors;
}

int check_State(NFAState * state, TestState ** ref_states, size_t ref_cur) {
    int nerrors = 0;
    TestState * ref_state = ref_states[ref_cur];
    nerrors += CHECK(ref_state->n_in == state->n_in, "number of 'in' transitions does not match in state %zu. expected: %zu, found: %zu\n", ref_cur, ref_state->n_in, state->n_in);
    nerrors += CHECK(ref_state->n_out == state->n_out, "number of 'out' transitions does not match in state %zu. expected: %zu, found: %zu\n", ref_cur, ref_state->n_out, state->n_out);
    //printf("state id %zu: n_in = %zu, n_out = %zu\n", ref_cur, ref_state->n_in, ref_state->n_out);
    return nerrors;
}


#define MAX_STATES 1024
int check_NFA(NFA * nfa, TestState ** ref_states) {
    int nerrors = 0;
    size_t ref_cur = 0;
    size_t ref_stack[MAX_STATES] = {0};
    NFAState * stack[MAX_STATES];
    NFAState * cur = NULL;
    size_t visited[MAX_STATES] = {0};
    size_t stack_size = 0;
    stack[stack_size] = nfa->start;
    ref_stack[stack_size++] = ref_cur;
    while (stack_size) {
        ref_cur = ref_stack[--stack_size];
        cur = stack[stack_size];
        if (ref_cur >= MAX_STATES) {
            nerrors += 1;
            printf("%s-%s-%d: testing framework error: exceeded maximum number of states %d\n", __FILE__,  __func__, __LINE__, MAX_STATES);
            return nerrors;
        }
        nerrors += check_State(cur, ref_states, ref_cur);
        visited[ref_cur] = 1;
        // TODO: should also account for checking input transitions, but not really required. All transitions will be checked by out alone
        NFATransition * trans = cur->out;
        for (size_t i = 0; i < ref_states[ref_cur]->n_out; i++) {
            TestTransition * ref_trans = ref_states[ref_cur]->t_out[i];
            nerrors += check_Transition(trans, ref_trans, ref_cur);
            if (ref_trans->state_end >= MAX_STATES) {
                nerrors += 1;
                printf("%s-%s-%d: testing framework error: exceeded maximum number of states %d\n", __FILE__,  __func__, __LINE__, MAX_STATES);
                return nerrors;
            }
            if (!visited[ref_trans->state_end]) {
                stack[stack_size] = trans->final;
                ref_stack[stack_size++] = ref_trans->state_end;
            }
            trans = trans->next_out;
        }
    }   

    return nerrors;
}

// eventually switch this to fully testing the regex from an element of TEST_REGEX[]
int check_regex(DFA * dfa, TestString * test) {
    int nerrors = 0;
    static char buffer[1024] = {'\0'};
    struct avramT3 av = {.buffer = buffer, .buffer_size = 1024, .dfa = *dfa, .flags = test->flags, .end = -1};
    int status = are_match(&av, test->cstr, strlen(test->cstr), 0);
    if (test->match.str) {
        nerrors += CHECK(!(status < test->match.len || status > test->match.len), "does not match expected length for input %s. expected: %d, found %d\n", test->cstr, test->match.len, status);
        if (!nerrors) {
            struct MatchString match;
            are_get_match(&av, &match, NULL);
            nerrors += CHECK(!strncmp(match.str, test->match.str, test->match.len), "unexpected match value for input %s. expected: %.*s, found: %.*s\n", test->cstr, test->match.len, test->match.str, match.len, match.str);
        }
    } else {
        nerrors += CHECK(status == -REGEX_FAIL, "unexpected status of failure test. expected: %d, found %d\n", -REGEX_FAIL, status);
    }
    return nerrors;
}

