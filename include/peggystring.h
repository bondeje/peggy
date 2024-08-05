#ifndef PEGGYSTRING_H
#define PEGGYSTRING_H

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

#define PEGGYSTRING_BUFFER_SIZE 128
#define PEGGYSTRING_MEMPOOL_COUNT 128

typedef struct PeggyString {
    char * str;
    size_t len;
} PeggyString;

int PeggyString_comp(PeggyString a, PeggyString b);
size_t PeggyString_hash(PeggyString a, size_t bin_size);

#define ELEMENT_TYPE PeggyString
#define ELEMENT_COMP PeggyString_comp
#include <peggy/stack.h>

void PeggyString_fwrite(PeggyString a, FILE * output, unsigned int flags);
_Bool PeggyString_startswith(PeggyString a, PeggyString prefix);

extern const PeggyString NULL_STRING;

#endif

