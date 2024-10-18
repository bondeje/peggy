#include <ctype.h>

#include <peggystring.h>

#define ELEMENT_TYPE char
#include <peggy/stack.h>

typedef struct STACK(char) StringBuilder;

const PeggyString NULL_STRING = {.str = NULL, .len = 0};

int StringBuilder_comp(StringBuilder a, StringBuilder b) {
    if (b.fill != a.fill) {
        return 1;
    }
    return strncmp(a.bins, b.bins, a.fill);
}

_Bool PeggyString_startswith(PeggyString a, PeggyString prefix) {
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

void PeggyString_fwrite(PeggyString a, FILE * output, unsigned int flags) {
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
        PeggyString_fwrite((PeggyString) {.str = a.str + start, .len = end-start}, output, (flags & ~PSFO_STRIP));
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

int PeggyString_comp(PeggyString a, PeggyString b) {
    if (a.len != b.len) {
        return 1;
    }
    int res = strncmp(a.str, b.str, a.len);
    return res;
}

size_t PeggyString_hash(PeggyString a, size_t bin_size) {
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

