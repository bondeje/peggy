#ifndef FA_H
#define FA_H

#include <stddef.h>
#include <stdio.h>
#include "reutils.h"

typedef struct Symbol Symbol;
typedef Symbol reChar;
typedef Symbol reCharClass;

// should never actually be allocated. more of an interface/abstract base class
struct Symbol {
    int (*match)(Symbol * sym, char const * str, size_t len, size_t * cursor);
    char const * match_name; // null-terminated string giving the name of the match function
    char const * sym;
    unsigned char sym_len;
};

BUILD_ALIGNMENT_STRUCT(Symbol)
BUILD_ALIGNMENT_STRUCT(reCharClass)

// file level
extern struct Symbol sym_empty;
extern struct Symbol sym_any;
extern struct Symbol sym_any_nonl;
extern struct Symbol sym_eos;
extern struct Symbol sym_bos;

int Symbol_fprint(FILE * stream, Symbol * sym);
int Symbol_print(Symbol * sym);

// returns positive value on success, 0 on failure
int reChar_match(Symbol * sym, char const * str, size_t len, size_t * cursor);
int reChar_empty_match(Symbol * sym, char const * str, size_t len, size_t * cursor);
int reChar_any_match(Symbol * sym, char const * str, size_t len, size_t * cursor);
int reChar_any_nonl_match(Symbol * sym, char const * str, size_t len, size_t * cursor);
int reChar_eos_match(Symbol * sym, char const * str, size_t len, size_t * cursor);
int reChar_bos_match(Symbol * sym, char const * str, size_t len, size_t * cursor);

// internal to char class
static inline int reRange_match(char const lower, char const upper, char const * str, size_t len, size_t const * cursor);
// returns positive value on success, 0 on failure
int reCharClass_match(Symbol * sym, char const * str, size_t len, size_t * cursor);
int reCharClass_inv_match(Symbol * sym, char const * str, size_t len, size_t * cursor);
// never update cursor
int rePositiveLookahead_match(Symbol * sym, char const * str, size_t const len, size_t * cursor);
// never update cursor
int reNegativeLookahead_match(Symbol * sym, char const * str, size_t const len, size_t * cursor);

int pSymbol_comp(Symbol * a, Symbol * b);
size_t pSymbol_hash(Symbol * key, size_t hash);

#endif

