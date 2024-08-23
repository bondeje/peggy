#include "dfa.h"

int DFA_check(DFAState const * cur_state, char const * str, int const len, 
    int * final) {
    
    DFATransition * trans = cur_state->trans;
    while (trans) {
        Symbol * sym = trans->sym;
        if (sym->match(sym, str)) {
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

