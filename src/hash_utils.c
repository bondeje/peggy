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

int uintptr_t_comp(uintptr_t a, uintptr_t b) {
    if (a > b) {
        return 1;
    } else if (a < b) {
        return -1;
    }
    return 0;
}

/* from: https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key */
size_t uintptr_t_hash(uintptr_t a, size_t hash_size) {
    a = ((a >> 16) ^ a) * 0x45d9f3b;
    a = ((a >> 16) ^ a) * 0x45d9f3b;
    a = (a >> 16) ^ a;
    return a % hash_size;
}

int size_t_comp(size_t a, size_t b) {
    if (a > b) {
        return 1;
    } else if (a < b) {
        return -1;
    }
    return 0;
}

/* from: https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key */
size_t size_t_hash(size_t a, size_t hash_size) {
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


// inspired by Implementation 5 https://stackoverflow.com/questions/4475996/given-prime-number-n-compute-the-next-prime
_Bool is_prime(size_t x) {
    if (x == 2 || x == 3) {
        return true;
    } else if (!(x & 1) || x < 2) {
        return false;
    }
    size_t i = 2;
    do {
        if (!(x % ++i)) {
            return false;
        }
    } while (x / i >= i);
    return true;
}

// inspired by Implementation 5 https://stackoverflow.com/questions/4475996/given-prime-number-n-compute-the-next-prime
// but reconfigured to handle some of the swtich cases and reflect more the is_prime function
size_t next_prime(size_t x) {
    switch (x) {
        case 0:
        case 1:
            return 2;
        case 2:
            return 3;
        case 3:
            return 5;
    }
    size_t o;
    if (!(x % 6)) { // x = 6*k, start at 6*k+1
        o = 2;
        x += 1;
    } else if (!((x+1) % 6)) { // x = 6*k + 5, start at 6*(k+1) + 1
        o = 2;
        x += 2;
    } else { // 6*k+1 <= x < 6*k + 5, start at 6*k + 5
        o = 4;
        x = 6*(x/6) + 5;
    }
    for (; !is_prime(x); x += o) {
        o ^= 6;
    }
    return x;
}

