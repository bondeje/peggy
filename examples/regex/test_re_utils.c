#include <string.h>
#include <stdio.h>

#include "test_utils.h"
#include "test_re_utils.h"

int check_Transition(NFATransition * trans, TestTransition * ref_trans, size_t ref_state_start) {
    int nerrors = 0;
    nerrors += check_Symbol(trans->symbol, &(Symbol){.sym = ref_trans->symbol, .sym_len = ref_trans->symbol_len}, 0);
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

int check_Symbol(Symbol * uut, Symbol * ref, int i) {
    int nerrors = 0;

    nerrors += CHECK(uut->sym_len == ref->sym_len, "symbol length mismatch in DFA symbol transition #%d. expected: %zu, found: %zu\n", i, ref->sym_len, uut->sym_len);
    if (!nerrors) {
        nerrors += CHECK(!strncmp(uut->sym, ref->sym, ref->sym_len), "symbol mismatch in DFA symbol transition #%d. expected: %.*s, found: %.*s\n", i, ref->sym_len, ref->sym, uut->sym_len, uut->sym);
    }
    if (ref->match) { // don't test if reference doesn't have it
        nerrors += CHECK(uut->match && uut->match == ref->match, "match functions do not match in DFA symbol transition #%d\n", i);
    }

    return nerrors;
}

int check_DFATransition(DFATransition ** uut, DFATransition ** ref, int i) {
    int nerrors = 0;
    
    if (CHECK(*uut, "expected transition in DFA state #%d but did not find any\n", i)) {
        nerrors++;
        *ref = NULL;
        return nerrors;
    }

    nerrors += CHECK((*uut)->final_state == (*ref)->final_state, "final states do not match in DFA state #%d transition. expected: %d, found: %d\n", i, (*ref)->final_state, (*uut)->final_state);
    nerrors += check_Symbol((*uut)->sym, (*ref)->sym, i);

    *uut = (*uut)->next;
    *ref = (*ref)->next;

    return nerrors;
}

int check_DFA(DFA * uut, DFA * ref) {
    int nerrors = 0;

    nerrors += CHECK(uut->nstates == ref->nstates, "mismatch in number of states in DFA. expected: %d, found: %d\n", ref->nstates, uut->nstates);
    if (!nerrors) {
        for (int i = 0; i < ref->nstates; i++) {
            DFATransition * ref_trans = ref->states[i].trans;
            DFATransition * uut_trans = uut->states[i].trans;
            while (ref_trans) {
                nerrors += check_DFATransition(&uut_trans, &ref_trans, i);
            }
            nerrors += CHECK(uut->states[i].accepting == ref->states[i].accepting, "accepting property does not match in DFA state #%d. expected: %i, found: %i\n", i, ref->states[i].accepting, uut->states[i].accepting);
        }
    }
    
    return nerrors;
}

int check_regex(struct avramT3 * av, TestString * test) {
    DFA * dfa = (DFA *)av;
    int nerrors = 0;
    int status = are_match(av, test->cstr, strlen(test->cstr), 0);
    if (test->match.str) {
        nerrors += CHECK(!(status < test->match.len || status > test->match.len), "does not match expected length for regex %.*s and input %s. expected: %d, found %d\n", dfa->regex_len, dfa->regex_s, test->cstr, test->match.len, status);
        if (!nerrors) {
            struct MatchString match;
            are_get_match(av, &match, NULL);
            nerrors += CHECK(!strncmp(match.str, test->match.str, test->match.len), "unexpected match value for regex %.*s and input %s. expected: %.*s, found: %.*s\n", dfa->regex_len, dfa->regex_s, test->cstr, test->match.len, test->match.str, match.len, match.str);
        }
    } else {
        nerrors += CHECK(status == -REGEX_FAIL, "unexpected status of failure test. expected: %d, found %d\n", -REGEX_FAIL, status);
    }
    return nerrors;
}

