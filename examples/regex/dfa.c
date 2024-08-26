#include "dfa.h"

int DFA_check(DFAState const * cur_state, char const * str, size_t const len, 
    size_t * cursor, int * final) {
    
    DFATransition * trans = cur_state->trans;
    while (trans) {
        Symbol * sym = trans->sym;
        if (sym->match(sym, str, len, cursor)) {
            if (final) {
                *final = trans->final_state;
            }
            return 0;
        }
        trans = trans->next;
    }
    // if we reach this far, cannot advance from the current state. failure
    return 1;
}

void DFA_dest(DFA * dfa) {
    MemPoolManager_del(dfa->pool);
    *dfa = (DFA){0};
}

int DFATransition_fprint(FILE * stream, DFATransition * trans, HASH_MAP(pSymbol, pSymbol) * sym_map) {
    Symbol * sym_trans = trans->sym;
    Symbol * sym_name = NULL;
    if (sym_map->_class->get(sym_map, sym_trans, &sym_name)) {
        printf("symbol %.*s not found in map\n", sym_trans->sym_len, sym_trans->sym);
        return 0;
    }
    int n = fprintf(stream, "{.sym = &%.*s, .final_state = %d, .next = %s", sym_name->sym_len, sym_name->sym, trans->final_state, trans->next ? "&(DFATransition)" : NULL);
    if (trans->next) {
        n += DFATransition_fprint(stream, trans->next, sym_map);
    }
    return n + fprintf(stream, "}");
}

int DFATransition_print(DFATransition * trans) {
    int n = printf("{.sym = ");
    n += Symbol_print(trans->sym);
    n += printf(", .final_state = %d, .next = %s", trans->final_state, trans->next ? "&(DFATransition)" : NULL);
    if (trans->next) {
        n += DFATransition_print(trans->next);
    }
    return n + printf("}");
}

int DFAState_fprint(FILE * stream, DFAState * state, HASH_MAP(pSymbol, pSymbol) * sym_map) {
    int n = fprintf(stream, "{.accepting = %s, .trans = %s", state->accepting ? "true" : "false", state->trans ? "&(DFATransition)" : "NULL");
    if (state->trans) {
        n += DFATransition_fprint(stream, state->trans, sym_map);
    }
    return n + fprintf(stream, "}");
}

int DFAState_print(DFAState * state) {
    int n = printf("{.accepting = %s, .trans = %s", state->accepting ? "true" : "false", state->trans ? "&(DFATransition)" : "NULL");
    if (state->trans) {
        n += DFATransition_print(state->trans);
    }
    return n + printf("}");
}

int DFA_fprint(FILE * stream, DFA * dfa, HASH_MAP(pSymbol, pSymbol) * sym_map) {
    int n = fprintf(stream, "{.nstates = %d, .regex_len = %d, .regex_s = \"%.*s\", .states = &(DFAState[]){", dfa->nstates, dfa->regex_len, dfa->regex_len, dfa->regex_s);
    for (int i = 0; i < dfa->nstates; i++) {
        n += DFAState_fprint(stream, dfa->states + i, sym_map);
    }
    return n + fprintf(stream, "}}");
}

int DFA_print(DFA * dfa) {
    int n = printf("{.nstates = %d, .regex_len = %d, .regex_s = \"%.*s\", .states = &(DFAState[]){", dfa->nstates, dfa->regex_len, dfa->regex_len, dfa->regex_s);
    for (int i = 0; i < dfa->nstates; i++) {
        n += DFAState_print(dfa->states + i);
    }
    return n + printf("}}");
}
