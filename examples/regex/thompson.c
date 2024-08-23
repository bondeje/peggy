#include <stdlib.h>

#include "peggy/hash_utils.h"
#include "thompson.h"

void StateSet_clear(StateSet * set) {
    for (size_t i = 0; i < set->states_size; i++) {
        set->states[i] = (STATE_BUF_TYPE)0;
    }
}

void StateSet_init(StateSet * set, size_t nstates) {
    set->states_size = ((nstates >> STATE_BUF_TYPE_LOG2) + 1) << STATE_BUF_TYPE_LOG2;
    set->states = calloc(set->states_size, sizeof(STATE_BUF_TYPE));
}

void StateSet_dest(StateSet * set) {
    free(set->states);
}

// returns non-zero on a failure condition
int StateSet_copy_into(StateSet * dest, StateSet * src) {
    size_t max_n = dest->states_size >= src->states_size ? dest->states_size : src->states_size;
    for (size_t i = 0; i < max_n; i++) {
        dest->states[i] = src->states[i];
    }
    for (size_t i = max_n; i < dest->states_size; i++) {
        dest->states[i] = (STATE_BUF_TYPE){0};
    }
    
    if (max_n < dest->states_size) {
        return 1;   
    }
    return 0;
}

_Bool StateSet_is_empty(StateSet * set) {
    for (size_t i = 0; i < set->states_size; i++) {
        if (set->states[i]) {
            return false;
        }
    }
    return true;
}

int StateSet_comp(StateSet set1, StateSet set2) {
    if (set1.states_size != set2.states_size) {
        return 1;
    }
    for (size_t i = 0; i < set1.states_size; i++) {
        if (set1.states[i] < set2.states[i]) {
            return -1;
        } else if (set1.states[i] > set2.states[i]) {
            return 1;
        }
    }
    return 0;
}

int StateSet_hash(StateSet set, size_t bin_size) {
    static unsigned long long hash = 5381;
    for (size_t i = 0; i < set.states_size; i++) {
        hash = ((hash << 5) + hash) + set.states[i]; /* hash * 33 + c */
    }

    return hash % bin_size;
}

#define ELEMENT_TYPE size_t
#define ELEMENT_COMP size_t_comp
#include "peggy/stack.h"

void fa_move(NFA * nfa, StateSet * set_in, Symbol * sym, StateSet * set_out) {
    StateSet_clear(set_out);

    size_t state_index = 0;
    for (size_t i = 0; i < set_in->states_size; i++) {
        if (set_in->states[i]) { // at least one state in 
            size_t max_state_index = (i + 1) * sizeof(STATE_BUF_TYPE);
            for (; state_index < max_state_index; state_index++) {
                if (StateSet_is_set(set_in, state_index)) {
                    NFATransition * trans = nfa->states[state_index]->out;
                    size_t nout = nfa->states[state_index]->n_out;
                    for (size_t j = 0; j < nout; j++) {
                        if (trans->symbol == sym) {
                            StateSet_set(set_out, trans->final->id);
                        }
                        trans = trans->next_out;
                    }
                }
            }
            // state_index == (i + 1) * sizeof(STATE_BUF_TYPE) at this point
        } else {
            state_index += sizeof(STATE_BUF_TYPE);
            // state_index == (i + 1) * sizeof(STATE_BUF_TYPE) at this point
        }
    }
}

// return non-zero on failure
int eclosure(NFA * nfa, StateSet * set_io) {
    // nfa provides context for NFAStates, set_in are the initial states for traversal of e-transitions
    // set_out are the final states of the e-transitions

    // following SS: 3.7.1 in Compilers: Principles, Techniques, and Tools
    // set_in is the "T" initially, but also the output of e-closure(T)

    // initialize a stack
    size_t * stack = malloc(sizeof(size_t) * nfa->nstates);
    size_t stack_size = 0;

    // push all states of T onto stack
    size_t state_index = 0;
    for (size_t i = 0; i < set_io->states_size; i++) {
        if (set_io->states[i]) { // at least one state in 
            size_t max_state_index = (i + 1) * sizeof(STATE_BUF_TYPE);
            for (; state_index < max_state_index; state_index++) {
                if (StateSet_is_set(set_io, state_index)) {
                    stack[stack_size++] = state_index;
                }
            }
            // state_index == (i + 1) * sizeof(STATE_BUF_TYPE) at this point
        } else {
            state_index += sizeof(STATE_BUF_TYPE);
            // state_index == (i + 1) * sizeof(STATE_BUF_TYPE) at this point
        }
    }

    // while stack is not empty
    while (stack_size) {
        // pop NFAState off stack. retrieving from list of states in NFA
        size_t t = stack[--stack_size];
        NFAState * tstate = nfa->states[t]; 
        NFATransition * trans = tstate->out;
        // for each transition from t to another state u labeled empty
        for (size_t i = 0; i < tstate->n_out; i++) {
            // if u is not in e-closure(T)
            if (StateSet_is_set(set_io, trans->final->id)) {
                // add u to e-closure(T)
                StateSet_set(set_io, trans->final->id);
                // push u onto stack
                stack[stack_size++] = trans->final->id;
            }
            trans = trans->next_out;
        }
    }

    // release stack
    free(stack);
    return 0;
}

#define KEY_TYPE StateSet
#define VALUE_TYPE size_t
#define KEY_COMP StateSet_comp
#define HASH_FUNC StateSet_hash
#include "peggy/hash_map.h"

typedef struct DFAState_map {
    DFAState state;
    StateSet nfa_set;
} DFAState_map;

#define ELEMENT_TYPE DFAState_map
#include "peggy/stack.h"

int NFA_to_DFA_clean(void * data, DFAState_map el) {
    StateSet_dest(&el.nfa_set);
    return 0;
}

int NFASymbols_to_DFA(void * dfa_, Symbol * key, Symbol * value) {
    DFA * dfa = dfa_;
    Symbol * dfa_sym = dfa->symbols + dfa->nsymbols++;
    *dfa_sym = *key;
    // allocate new copy of symbol
    dfa_sym->sym = MemPoolManager_malloc(dfa->pool, dfa_sym->sym_len);
    memcpy((void *)dfa_sym->sym, key->sym, dfa_sym->sym_len);
    return 0;
}

int DFAStates_to_DFA(void * dfa_, DFAState_map el) {
    DFA * dfa = dfa_;
    dfa->states[dfa->nstates++] = el.state;
    return 0;
}

// returns non-zero on error
int NFA_to_DFA(NFA * nfa, DFA * dfa) {
    int status = 0;

    // initialize dfa and regex reference
    *dfa = (DFA) {
        .flags = nfa->flags,
        .pool = MemPoolManager_new(nfa->nstates >> 1, sizeof(DFAState), _Alignof(DFAState)),
        .regex_len = nfa->regex_len,
        .regex_s = nfa->regex_s
    };
    size_t accepting_state = nfa->final->id;

    // copy the symbols from nfa to dfa
    dfa->symbols = MemPoolManager_aligned_alloc(dfa->pool, sizeof(Symbol) * nfa->symbol_map.fill, _Alignof(Symbol));
    nfa->symbol_map._class->for_each(&nfa->symbol_map, NFASymbols_to_DFA, dfa);

    // following SS: 3.7.1 in Compilers: Principles, Techniques, and Tools

    /** data structures needed:
     *      a hash_map from e-closure of NFA state indices (represented as StateSet) to DFA state index
     *      a stack of DFAStates
     */
    HASH_MAP(StateSet, size_t) nfa2dfa;
    HASH_MAP_INIT(StateSet, size_t)(&nfa2dfa, (nfa->nstates * (nfa->nstates + 1)) >> 1);

    // add a new state id (index 0) to dfa

    STACK(DFAState_map) dfa_states;
    STACK_INIT(DFAState_map)(&dfa_states, nfa->nstates >> 1);

    StateSet sset;
    StateSet_init(&sset, nfa->nstates);
    if (eclosure(nfa, &sset)) {
        status = 1;
        goto cleanup;
    }
    nfa2dfa._class->set(&nfa2dfa, sset, dfa_states.fill);  
    dfa_states._class->push(&dfa_states, (DFAState_map){.state = {.accepting = StateSet_is_set(&sset, accepting_state), .trans = NULL}, .nfa_set = sset}); 

    // initialize dfa with a single state as the e-closure of the start state in nfa.
    size_t dfa_index = 0;
    while (dfa_index < dfa_states.fill /* there is another state to process in dfa */) {
        DFAState_map * dfa_state_item = dfa_states.bins + dfa_index;
        // increment processing state
        for (size_t i = 0; i < dfa->nsymbols; i++) {
            // U = e-closure(move(T, a))
            StateSet a_trans;
            StateSet_init(&a_trans, nfa->nstates);
            fa_move(nfa, &dfa_state_item->nfa_set, dfa->symbols + i, &a_trans);
            if (StateSet_is_empty(&a_trans)) {
                continue; // move to next symbol
            }
            if (eclosure(nfa, &a_trans)) {
                status = 1;
                goto cleanup;
            }

            // U state
            size_t dfa_state;
            if (!nfa2dfa._class->get(&nfa2dfa, sset, &dfa_state)) {
                // add U as an unmarked state to Dstates
                dfa_state = dfa_states.fill;
                nfa2dfa._class->set(&nfa2dfa, a_trans, dfa_state);
                dfa_states._class->push(&dfa_states, (DFAState_map){.state = {.accepting = StateSet_is_set(&a_trans, accepting_state), .trans = NULL}, .nfa_set = a_trans}); 
            }
            // add a new transition to dfa_state_item->state {dfa->symbols + i, dfa_state}
            DFATransition * trans = MemPoolManager_aligned_alloc(dfa->pool, sizeof(DFATransition), _Alignof(DFATransition));
            trans->next = ((DFAState *)(dfa_states.bins + dfa_state))->trans;
            ((DFAState *)(dfa_states.bins + dfa_state))->trans = trans;
            trans->final_state = dfa_state;
            trans->sym = dfa->symbols + i;
        }
        dfa_index++;
    }

    dfa->states = MemPoolManager_aligned_alloc(dfa->pool, sizeof(DFAState) * dfa_states.fill, _Alignof(DFAState));
    dfa_states._class->for_each(&dfa_states, DFAStates_to_DFA, dfa);
cleanup:
    dfa_states._class->for_each(&dfa_states, NFA_to_DFA_clean, NULL);
    dfa_states._class->dest(&dfa_states);
    nfa2dfa._class->dest(&nfa2dfa);
    return status;
}

