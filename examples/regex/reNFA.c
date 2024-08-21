#include "reNFA.h"

struct reSymbol empty_symbol = {.match = reChar_empty_match, .sym = ""};

// returns positive value on success, 0 on failure
int reChar_match(reSymbol * sym, char const c) {
    return *sym->sym == c;
}

int reChar_empty_match(reSymbol * sym, char const c) {
    return 1;
}

// internal to char class
static inline int reRange_match(char const lower, char const upper, char const c) {
    return c <= upper && c >= lower;
}

// returns positive value on success, 0 on failure
int reCharClass_match(reSymbol * sym, char const c) { 
    reCharClass * class = (reCharClass *)sym;
    char const * start = sym->sym + (class->inv ? 1 : 0); // skip '^' if inverting
    char const * end = start + class->sym.sym_len;
    int status = 0;
    while (start != end) {
        if (start + 1 != end && *(start + 1) == '-') {
            status = reRange_match(*start, *(start + 2), c);
        } else {
            status = (*start == c);
        }
        if (status) {
            return !class->inv;
        }
        start++;
    }
    return class->inv;
}

void reNFA_init(reNFA * nfa) {
    *nfa = (reNFA){0};
    nfa->pool = MemPoolManager_new(4096, 1, 1); // string memory pool
}

void reNFA_dest(reNFA * nfa) {
    MemPoolManager_del(nfa->pool);
    *nfa = (reNFA){0};
}
