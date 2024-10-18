#ifndef P4CSTRING_H
#define P4CSTRING_H

#include <stddef.h>
#include <stdio.h>

#define PSFO_NONE 0
#define PSFO_LOWER 1
#define PSFO_UPPER 2
#define PSFO_CASE_MASK = 3
#define PSFO_STRIP 4
#define PSFO_RESERVED_BITS 4
#define PSFO_LOFFSET(x) ((x) << PSFO_RESERVED_BITS)
#define PSFO_GET_LOFFSET(flags) ((flags) >> PSFO_RESERVED_BITS)

#define P4CSTRING_BUFFER_SIZE 128
#define P4CSTRING_MEMPOOL_COUNT 128

typedef struct P4CString {
    char * str;
    size_t len;
} P4CString;

int P4CString_comp(P4CString a, P4CString b);
size_t P4CString_hash(P4CString a, size_t bin_size);

#define ELEMENT_TYPE P4CString
#define ELEMENT_COMP P4CString_comp
#include "peg4c/stack.h"

void P4CString_fwrite(P4CString a, FILE * output, unsigned int flags);
_Bool P4CString_startswith(P4CString a, P4CString prefix);

extern const P4CString NULL_STRING;

#endif

