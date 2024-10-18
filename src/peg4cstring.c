#include <ctype.h>

#include "peg4cstring.h"

#define ELEMENT_TYPE char
#include "peg4c/stack.h"

typedef struct STACK(char) StringBuilder;

const P4CString NULL_STRING = {.str = NULL, .len = 0};

int StringBuilder_comp(StringBuilder a, StringBuilder b) {
    if (b.fill != a.fill) {
        return 1;
    }
    return strncmp(a.bins, b.bins, a.fill);
}

_Bool P4CString_startswith(P4CString a, P4CString prefix) {
    if (a.len < prefix.len) {
        return false;
    }
    size_t i = 0;
    while (i < prefix.len) {
        if (a.str[i] != prefix.str[i]) {
            return false;
        }
        i++;
    }
    return true;
}

void P4CString_fwrite(P4CString a, FILE * output, unsigned int flags) {
    if (!flags) {
        fwrite(a.str, 1, a.len, output);
        return;
    }
    size_t start = PSFO_GET_LOFFSET(flags);
    size_t end = a.len;
    if (PSFO_STRIP & flags) {
        while (start < end && isspace((unsigned char)a.str[start])) {
            start++;
        }
        while (end > start && isspace((unsigned char)a.str[end-1])) {
            end--;
        }
        P4CString_fwrite((P4CString) {.str = a.str + start, .len = end-start}, output, (flags & ~PSFO_STRIP));
        return;
    }
    if (end <= start) {
        return;
    }
    if (flags & PSFO_LOWER) {
        for (size_t i = start; i < end; i++) {
            fputc(tolower((unsigned char)a.str[i]), output);
        }
    } else {
        for (size_t i = start; i < end; i++) {
            fputc(toupper((unsigned char)a.str[i]), output);
        }
    }
    
}

int P4CString_comp(P4CString a, P4CString b) {
    if (a.len != b.len) {
        return 1;
    }
    int res = strncmp(a.str, b.str, a.len);
    return res;
}

size_t P4CString_hash(P4CString a, size_t bin_size) {
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

