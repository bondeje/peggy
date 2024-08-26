#include <string.h>

#include "fa.h"

struct Symbol sym_empty = {.match = reChar_empty_match, .match_name = "reChar_empty_match", .sym = "", .sym_len = 0};
struct Symbol sym_any = {.match = reChar_any_match, .match_name = "reChar_any_match", .sym = "any", .sym_len = 3};
struct Symbol sym_any_nonl = {.match = reChar_any_nonl_match, .match_name = "reChar_any_nonl_match", .sym = "", .sym_len = 0};
struct Symbol sym_eos = {.match = reChar_eos_match, .match_name = "reChar_eos_match", .sym = "eos", .sym_len = 3};
struct Symbol sym_bos = {.match = reChar_bos_match, .match_name = "reChar_bos_match", .sym = "bos", .sym_len = 3};

int Symbol_fprint(FILE * stream, Symbol * sym) {
    return fprintf(stream, "{.match = %s, .match_name = \"%s\", .sym = \"%.*s\", .sym_len = %d}", sym->match_name, sym->match_name, sym->sym_len, sym->sym, sym->sym_len);
}

int Symbol_print(Symbol * sym) {
    return printf("{.match = %s, .match_name = \"%s\", .sym = \"%.*s\", .sym_len = %d}", sym->match_name, sym->match_name, sym->sym_len, sym->sym, sym->sym_len);
}

// returns positive value on success, 0 on failure
// matches a single byte character
int reChar_match(Symbol * sym, char const * str, size_t len, size_t * cursor) {
    if (len == *cursor) {
        return 0;
    }
    int status = (str[*cursor] == *sym->sym);
    if (status) {
        (*cursor)++;
        return 1;
    }
    return 0;
}

int reChar_match_multi_byte(Symbol * sym, char const * str, size_t len, size_t * cursor) {
    size_t offset = *cursor;
    if (len - offset < sym->sym_len) {
        return 0;
    }
    for (size_t i = 0; i < sym->sym_len; i++) {
        if (str[offset] != sym->sym[i]) {
            return 0;
        }
        offset++;
    }
    *cursor += sym->sym_len;
    return 1;
}

int reChar_empty_match(Symbol * sym, char const * str, size_t len, size_t * cursor) {
    return 0; // no position update
}

int reChar_any_match(Symbol * sym, char const * str, size_t len, size_t * cursor) {
    if (len == *cursor) {
        return 0;
    }
    (*cursor)++;
    return 1;
}

int reChar_any_nonl_match(Symbol * sym, char const * str, size_t len, size_t * cursor) {
    if (len == *cursor || str[*cursor] == '\n') {
        return 0;
    }
    *cursor++;
    return 1;
}

int reChar_eos_match(Symbol * sym, char const * str, size_t len, size_t * cursor) {
    return len == *cursor; // no position update
}

int reChar_bos_match(Symbol * sym, char const * str, size_t len, size_t * cursor) {
    return *cursor == 0; // no position update
}

// internal to char class. DOES NOT UPDATE "cursor"
static inline int reRange_match(char const lower, char const upper, char const * str, size_t len, size_t const * cursor) {
    size_t offset = *cursor;
    if (len == offset || str[offset] < lower || str[offset] > upper) {
        return 0;
    }
    return 1;
}

// need a reRange_match_multi_byte for unicode range support

// returns positive value on success, 0 on failure
int reCharClass_match(Symbol * sym, char const * str, size_t len, size_t * cursor) { 
    if (len == *cursor) {
        return 0;
    }
    char const * start = sym->sym + 1;
    char const * end = start + sym->sym_len - 2;
    int status = 0;
    while (start != end) {
        // NOTE: when reRange_match_multi_byte is available, need to handle those types of ranges
        if (start + 1 != end && *(start + 1) == '-') {
            status = reRange_match(*start, *(start + 2), str, len, cursor);
        } else {
            status = (*start == str[*cursor]);
        }
        if (status) {
            (*cursor)++;
            return 1;
        }
        start++;
    }
    return 0;
}

int reCharClass_inv_match(Symbol * sym, char const * str, size_t len, size_t * cursor) { 
    if (len == *cursor) {
        return 0;
    }
    char const * start = sym->sym + 2;
    char const * end = start + sym->sym_len - 3;
    int status = 0;
    while (start != end) {
        // NOTE: when reRange_match_multi_byte is available, need to handle those types of ranges
        if (start + 1 != end && *(start + 1) == '-') {
            status = reRange_match(*start, *(start + 2), str, len, cursor); 
        } else {
            status = (*start == str[*cursor]);
        }
        if (status) {
            return 0;
        }
        start++;
    }
    (*cursor)++;
    return 1;
}

// never update cursor
// only works for sequence of single-byte matches
int rePositiveLookahead_match(Symbol * sym, char const * str, size_t const len, size_t * cursor) {
    // if there's not enough characters left to possibly match the lookaround sequence
    if (len - *cursor + 4 < sym->sym_len) {
        return 0;
    }

    char const * start = sym->sym + 3;
    char const * end = sym->sym + sym->sym_len - 1;
    char const check = *(str + *cursor);
    while (start != end) {
        if (check != *start) {
            return 0;
        }
        start++;
    }

    return 1;
}

// never update cursor
int reNegativeLookahead_match(Symbol * sym, char const * str, size_t const len, size_t * cursor) {
    // if there's not enough characters left to possibly match the lookaround sequence
    if (len - *cursor + 4 < sym->sym_len) { // end of string will be hit, negative lookahead succeeds
        if (len == *cursor) {
            (*cursor)++;
        }
        return 1;
    }

    char const * start = sym->sym + 3;
    char const * end = sym->sym + sym->sym_len - 1;
    char const check = *(str + *cursor);
    while (start != end) {
        if (check != *start) {
            return 1;
        }
        start++;
    }

    return 0; // fails if it makes it to the end of the symbol characters
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
