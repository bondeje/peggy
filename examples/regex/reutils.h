#ifndef CAT
    #define CAT_(x, y) x##y
    #define CAT(x, y) CAT_(x, y)
#endif

#if __STDC_VERSION__ < 201112L
#ifndef _Alignof
    #define _Alignof(type) offsetof(struct CAT(type, _ALIGNMENT), type)
#endif
    #define BUILD_ALIGNMENT_STRUCT(type) \
    struct CAT(type, _ALIGNMENT) {\
        char a;\
        type type;\
    };
#else
    #define BUILD_ALIGNMENT_STRUCT(type)
#endif

#define REGEX_ALLOC_BUFFER 1
#define REGEX_ALLOC_BUFFER_SIZE 128
#define REGEX_NO_GREEDY 2
#define REGEX_DOT_NEWLINE 4

#define REGEX_MATCH 0
#define REGEX_FAIL 1
#define REGEX_ERROR 2
#define REGEX_WAIT 3
#define REGEX_WAIT_MATCH 4
