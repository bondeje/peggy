#ifndef PEGGY_UTILS_H
#define PEGGY_UTILS_H

#if defined(__GNUC__) || defined(__clang__)
    #define FALLTHROUGH __attribute__((fallthrough));
#else
    #define FALLTHROUGH
#endif

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

extern unsigned char const CHAR_BIT_DIV_SHIFT;
extern unsigned char const CHAR_BIT_MOD_MASK;

#ifndef CAT
    #define CAT_(A, B) A##B
    #define CAT(A, B) CAT_(A, B)
#endif

#ifndef STRINGIFY
    #define STRINGIFY_(X) #X
    #define STRINGIFY(X) STRINGIFY_(X)
#endif

#ifndef CAT3
    #define CAT3_(A, B, C) A##B##C
    #define CAT3(A, B, C) CAT3_(A, B, C)
#endif

#define EXPAND(A, ...) A(__VA_ARGS__)
#define DELAY(A) A

#ifndef VARIADIC_SIZE
    #define VARIADIC_SIZE(...) VARIADIC_SIZE_(__VA_ARGS__, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, UNUSED)
    #define VARIADIC_SIZE_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, size, ...) size
#endif

#define MINIMUM_(A, B) ((A) > (B) ? (B) : (A))
#define MINIMUM(A, B) MINIMUM_(A, B)

#define MAXIMUM_(A, B) ((A) < (B) ? (B) : (A))
#define MAXIMUM(A, B) MAXIMUM_(A, B)

#define DEBUG_ASSERT(X, ...) assert((X) || !printf(__VA_ARGS__))

typedef enum err_type {
    PEGGY_SUCCESS = 0,
    PEGGY_FAILURE,
    PEGGY_MALLOC_FAILURE,
    PEGGY_INIT_FAILURE,
    PEGGY_NOT_IMPLEMENTED,
    PEGGY_INDEX_OUT_OF_BOUNDS,
    PEGGY_MALLOC_MAP_FAILURE,
    PEGGY_MALLOC_PAIR_FAILURE,
    PEGGY_EMPTY_STACK,
    PEGGY_EMPTY_DEQUE,
    PEGGY_REGEX_FAILURE,
    PEGGY_FILE_IO_ERROR,
    PEGGY_NO_ROOT_PRODUCTION_FOUND,
    PEGGY_NO_TOKENIZER_FOUND,
    PEGGY_TOKENIZE_FAILURE,
    PEGGY_PARSE_FAILURE,
} err_type;

typedef enum RuleTypeID {
    PEG_NOTRULE = -1,
    PEG_RULE,
    PEG_CHAIN,
    PEG_SEQUENCE,
    PEG_CHOICE,
    PEG_LITERAL,
    PEG_DERIVED,
    PEG_LIST,
    PEG_REPEAT,
    PEG_POSITIVELOOKAHEAD,
    PEG_NEGATIVELOOKAHEAD,
    PEG_PRODUCTION
} RuleTypeID;

// needed because of circular includes
typedef int rule_id_type;
typedef struct Rule Rule;
typedef struct Parser Parser;

#define Rule_DEFAULT_ID -1

/**
 * @brief utility to get a string from the enum identifiers for RuleTypeID above
 */
char const * get_type_name(RuleTypeID type);

/**
 * @brief check if a rule instance given by RuleTypeID is within the types 
 * supplied
 * @param[in] type the type of the rule: RuleType.id
 * @param[in] types the types to check against
 * @returns true if type is in types else false
 */
bool isinstance(RuleTypeID const type, RuleTypeID const * types);

unsigned char size_t_strlen(size_t val);

#endif // PEGGY_UTILS_H
