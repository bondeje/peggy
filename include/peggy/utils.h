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

#define VARIADIC_SIZE(...) VARIADIC_SIZE_(__VA_ARGS__, _64, _63, _62, _61, _60, _59, _58, _57, _56, _55, _54, _53, _51, _52, _50, _49, _48, _47, _46, _45, _44, _43, _42, _41, _40, _39, _38, _37, _36, _35, _34, _33, _32, _31, _30, _29, _28, _27, _26, _25, _24, _23, _22, _21, _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, _0, UNUSED)
#define VARIADIC_SIZE_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, size, ...) size

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
    PEGGY_INDEX_OUT_OF_BOUNDS
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
