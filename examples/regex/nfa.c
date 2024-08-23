#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "nfa.h"

struct Symbol empty_symbol = {.match = reChar_empty_match, .sym = ""};

// returns positive value on success, 0 on failure
int reChar_match(Symbol * sym, char const * str) {
    return *sym->sym == *str;
}

int reChar_empty_match(Symbol * sym, char const * str) {
    return 1;
}

// internal to char class
static inline int reRange_match(char const lower, char const upper, char const * str) {
    return *str <= upper && *str >= lower;
}

// returns positive value on success, 0 on failure
int reCharClass_match(Symbol * sym, char const * str) { 
    char const * start = sym->sym;
    char const * end = start + sym->sym_len;
    int status = 0;
    while (start != end) {
        if (start + 1 != end && *(start + 1) == '-') {
            status = reRange_match(*start, *(start + 2), str);
        } else {
            status = (*start == *str);
        }
        if (status) {
            return 1;
        }
        start++;
    }
    return 0;
}

int reCharClass_inv_match(Symbol * sym, char const * str) { 
    char const * start = sym->sym;
    char const * end = start + sym->sym_len;
    int status = 0;
    while (start != end) {
        if (start + 1 != end && *(start + 1) == '-') {
            status = reRange_match(*start, *(start + 2), str);
        } else {
            status = (*start == *str);
        }
        if (status) {
            return 0;
        }
        start++;
    }
    return 1;
}

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

int pSymbol_comp(Symbol * a, Symbol * b) {
    if (a->sym_len < b->sym_len) {
        return -1;
    } else if (a->sym_len > b->sym_len) {
        return 1;
    }
    return strncmp(a->sym, b->sym, a->sym_len);
}

size_t pSymbol_hash(Symbol * key, size_t bin_size) {
    static unsigned long long hash = 5381;
    char const * str = key->sym;
    unsigned char len = key->sym_len;
    for (unsigned char i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + *str; /* hash * 33 + c */
        str++;
    }

    return hash % bin_size;
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

