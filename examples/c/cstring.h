#ifndef CSTRING_H
#define CSTRING_H

#include <stddef.h>

typedef struct CString {
    char const * str;
    size_t len;
} CString;


int CString_comp(CString a, CString b);
size_t CString_hash(CString a, size_t bin_size);

#endif
