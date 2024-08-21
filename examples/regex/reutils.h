#ifndef CAT
    #define CAT_(x, y) x##y
    #define CAT(x, y) CAT_(x, y)
#endif

#if __STDC_VERSION__ < 201112L
    #define _Alignof(type) offsetof(struct CAT(type, _ALIGNMENT), type)
    #define BUILD_ALIGNMENT_STRUCT(type) \
    struct CAT(type, _ALIGNMENT) {\
        char a;\
        type type;\
    };
#else
    #define BUILD_ALIGNMENT_STRUCT(type)
#endif

