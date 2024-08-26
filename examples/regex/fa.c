#include <string.h>

#include "fa.h"

struct Symbol sym_empty = {.match = reChar_empty_match, .match_name = "reChar_empty_match", .sym = "", .sym_len = 0};
struct Symbol sym_any = {.match = reChar_any_match, .match_name = "reChar_any_match", .sym = "", .sym_len = 0};
struct Symbol sym_any_nonl = {.match = reChar_any_nonl_match, .match_name = "reChar_any_nonl_match", .sym = "", .sym_len = 0};
struct Symbol sym_eos = {.match = reChar_eos_match, .match_name = "reChar_eos_match", .sym = "", .sym_len = 0};

int Symbol_fprint(FILE * stream, Symbol * sym) {
    return fprintf(stream, "{.match = %s, .match_name = \"%s\", .sym = \"%s\", .sym_len = %d}", sym->match_name, sym->match_name, sym->sym, sym->sym_len);
}

// returns positive value on success, 0 on failure
int reChar_match(Symbol * sym, char const * str) {
    return *sym->sym == *str;
}

int reChar_empty_match(Symbol * sym, char const * str) {
    return 0;
}

int reChar_any_match(Symbol * sym, char const * str) {
    return 1;
}

int reChar_any_nonl_match(Symbol * sym, char const * str) {
    return *str != '\n';
}

int reChar_eos_match(Symbol * sym, char const * str) {
    return *str == '\0';
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

int pSymbol_comp(Symbol * a, Symbol * b) {
    if (a->sym_len < b->sym_len) {
        return -1;
    } else if (a->sym_len > b->sym_len) {
        return 1;
    }
    return strncmp(a->sym, b->sym, a->sym_len);
}

size_t pSymbol_hash(Symbol * key, size_t bin_size) {
    unsigned long long hash = 5381;
    char const * str = key->sym;
    unsigned char len = key->sym_len;
    for (unsigned char i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + *str; /* hash * 33 + c */
        str++;
    }

    return hash % bin_size;
}
