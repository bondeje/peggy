#ifndef CAT
    #define CAT_(x, y) x##y
    #define CAT(x, y) CAT_(x, y)
#endif

#ifndef _Alignof
    #define _Alignof(type) offsetof(struct CAT(type, _ALIGNMENT), type)
#endif