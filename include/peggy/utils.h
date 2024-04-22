#ifndef PEGGY_UTILS_H
#define PEGGY_UTILS_H

#include <stddef.h>
#include <stdbool.h>

#define CAT_(A, B) A##B
#define CAT(A, B) CAT_(A, B)

#define CAT3_(A, B, C) A##B##C
#define CAT3(A, B, C) CAT3_(A, B, C)

#define EXPAND(A, ...) A(__VA_ARGS__)
#define DELAY(A) A

#define VARIADIC_SIZE(...) VARIADIC_SIZE_(__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, UNUSED)
#define VARIADIC_SIZE_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, size, ...) size

#define MINIMUM_(A, B) ((A) > (B) ? (B) : (A))
#define MINIMUM(A, B) MINIMUM_(A, B)

#define MAXIMUM_(A, B) ((A) < (B) ? (B) : (A))
#define MAXIMUM(A, B) MAXIMUM_(A, B)

#define DEBUG_ASSERT(X, ...) assert((X) || !printf(__VA_ARGS__))

/*
typedef struct StructInfo {
    char const * const * const attrs;
    size_t * const offsets;
    size_t n_attrs;
} StructInfo;
*/
typedef enum err_type {
    PEGGY_SUCCESS = 0,
    PEGGY_FAILURE,
    PEGGY_MALLOC_FAILURE,
    PEGGY_NOT_IMPLEMENTED,
    PEGGY_INDEX_OUT_OF_BOUNDS,
    PEGGY_MALLOC_MAP_FAILURE,
    PEGGY_MALLOC_PAIR_FAILURE,
    PEGGY_EMPTY_STACK,
    PEGGY_EMPTY_DEQUE,
    PEGGY_REGEX_FAILURE,
    PEGGY_FILE_IO_ERROR
} err_type;

typedef enum iter_status {
    PEGGY_ITER_GO = 0,
    PEGGY_ITER_STOP
} iter_status;

typedef int rule_id_type;

typedef unsigned long long hash_type;

typedef struct Rule Rule;
typedef struct Parser Parser;

#define Rule_DEFAULT_ID -1

// inspired by Implementation 5 https://stackoverflow.com/questions/4475996/given-prime-number-n-compute-the-next-prime
bool is_prime(size_t x);

// inspired by Implementation 5 https://stackoverflow.com/questions/4475996/given-prime-number-n-compute-the-next-prime
// but reconfigured to handle some of the swtich cases and reflect more the is_prime function
size_t next_prime(size_t x);

#endif // PEGGY_UTILS_H
