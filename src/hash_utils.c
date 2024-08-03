#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <peggy/hash_map.h>

#define HASH_MAP_N_MESSAGES 4
char const * const hm_messages[HASH_MAP_N_MESSAGES] = {
    "No error",
    "key not found in search",
    "failure to malloc the map storage",
    "failure to malloc a key-value pair",
};

#define HASH_SET_N_MESSAGES 3
char const * const hs_messages[HASH_SET_N_MESSAGES] = {
    "No error",
    "value not found in search",
    "failure to malloc the set storage"
};

int uintptr_comp(uintptr_t a, uintptr_t b) {
    if (a > b) {
        return 1;
    } else if (a < b) {
        return -1;
    }
    return 0;
}

/* from: https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key */
uintptr_t uintptr_hash(uintptr_t a, uintptr_t hash_size) {
    a = ((a >> 16) ^ a) * 0x45d9f3b;
    a = ((a >> 16) ^ a) * 0x45d9f3b;
    a = (a >> 16) ^ a;
    return a % hash_size;
}

size_t cstr_hash(char const * key, size_t bin_size) {
    static unsigned long long hash = 5381;
    int c;
    unsigned char * str = (unsigned char *) key;

    while ((c = *str)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        str++;
    }

    return hash % bin_size;
}

size_t address_hash(void const * val, size_t bin_size) {
    return ((uintptr_t)(void *)val) % bin_size; // subtracting (void*)0 so that it at least *looks* like I'm dealing with a number
}

int cstr_comp(char const * a, char const * b) {
    return strcmp(a, b);
}

int address_comp(void const * a, void const * b) {
    if (a > b) {
        return 1;
    } else if (a < b) {
        return -1;
    }
    return 0;
}

void hm_print_error_message(int status) {
    if (status < 0 || status > HASH_MAP_N_MESSAGES) {
        printf("error message %d not understood\n", status);
    } else {
        printf("error code %d: %s\n", status, hm_messages[status]);
    }
}

void hs_print_error_message(int status) {
    if (status < 0 || status > HASH_SET_N_MESSAGES) {
        printf("error message %d not understood\n", status);
    } else {
        printf("error code %d: %s\n", status, hs_messages[status]);
    }
}

