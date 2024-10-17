#include <string.h>

#include "cstring.h"

int CString_comp(CString a, CString b) {
    if (a.len < b.len) {
        return 1;
    } else if (a.len > b.len) {
        return -1;
    }
    return strncmp(a.str, b.str, a.len);
}

size_t CString_hash(CString a, size_t bin_size) {
    unsigned long long hash = 5381;
    size_t i = 0;
    unsigned char * str = (unsigned char *) a.str;

    while (i < a.len) {
        hash = ((hash << 5) + hash) + *str; /* hash * 33 + c */
        str++;
        i++;
    }
    return hash % bin_size;
}
