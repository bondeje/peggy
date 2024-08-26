#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "nfa.h"

#define DEFAULT_NFA_POOL_CT 128
#define DEFAULT_NFA_SYMBOL_CT 32

void NFA_init(NFA * nfa) {
    *nfa = (NFA){0};
    size_t size = sizeof(NFATransition) > sizeof(NFAState) ? sizeof(NFATransition) : sizeof(NFAState);
    nfa->nfa_pool = MemPoolManager_new(DEFAULT_NFA_POOL_CT, size, 1);
    HASH_MAP_INIT(pSymbol, pSymbol)(&nfa->symbol_map, DEFAULT_NFA_SYMBOL_CT);
    nfa->states_cap = DEFAULT_NFA_POOL_CT;
    nfa->states = MemPoolManager_aligned_alloc(nfa->nfa_pool, nfa->states_cap * sizeof(NFAState *), sizeof(NFAState *));
}

void NFA_dest(NFA * nfa) {
    MemPoolManager_del(nfa->nfa_pool);
    nfa->symbol_map._class->dest(&nfa->symbol_map);
    *nfa = (NFA){0};
}

NFAState * NFA_new_state(NFA * nfa) {
    NFAState * out = MemPoolManager_aligned_alloc(nfa->nfa_pool, sizeof(NFAState), _Alignof(NFAState));
    if (nfa->nstates >= nfa->states_cap) {
        NFAState ** new_states = MemPoolManager_aligned_alloc(nfa->nfa_pool, 2 *nfa->states_cap * sizeof(NFAState *), sizeof(NFAState *));
        if (!new_states) {
            return NULL;
        }
        memcpy(new_states, nfa->states, sizeof(NFAState *) * nfa->states_cap);
        nfa->states_cap *= 2;
        nfa->states = new_states;
    }
    out->id = nfa->nstates;
    nfa->states[nfa->nstates++] = out;
    return out;
}

NFATransition * NFA_new_transition(NFA * nfa) {
    return MemPoolManager_aligned_alloc(nfa->nfa_pool, sizeof(NFATransition), _Alignof(NFATransition));
}

Symbol * NFA_get_symbol(NFA * nfa, char const * sym, unsigned char len) {
    Symbol temp = {.sym = sym, .sym_len = len};
    Symbol * ptemp = &temp;
    Symbol * out = NULL;
    if (!nfa->symbol_map._class->get(&nfa->symbol_map, ptemp, &out)) {
        return out;
    }
    out = MemPoolManager_aligned_alloc(nfa->nfa_pool, sizeof(Symbol), _Alignof(Symbol));
    *out = temp;
    nfa->symbol_map._class->set(&nfa->symbol_map, out, out);
    return out;
}

