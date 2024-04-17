
/* for inclusion without implementation */
#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <peggy/utils.h>

#define STACK_INIT(k) CAT(k, Stack_init)

#define STACK(k) CAT(k, Stack)

#endif // STACK_H

#if defined(ELEMENT_TYPE)

#ifndef STACK_DEFAULT_SIZE
#define STACK_DEFAULT_SIZE MAXIMUM(128 / sizeof(ELEMENT_TYPE), 16)
#endif // STACK_DEFAULT_SIZE

#ifndef STACK_DEFAULT_SCALE
#define STACK_DEFAULT_SCALE 2
#endif // STACK_DEFAULT_SCALE

#define STACK_TYPE STACK(ELEMENT_TYPE)

#ifndef ELEMENT_COMP
static int CAT(ELEMENT_TYPE, _comp)(ELEMENT_TYPE key1, ELEMENT_TYPE key2) {
    char bytes1[sizeof(ELEMENT_TYPE)+1];
    char bytes2[sizeof(ELEMENT_TYPE)+1];
    bytes1[sizeof(ELEMENT_TYPE)] = '\0', bytes2[sizeof(ELEMENT_TYPE)] = '\0';
    memcpy(bytes1, &key1, sizeof(ELEMENT_TYPE));
    memcpy(bytes2, &key2, sizeof(ELEMENT_TYPE));
    return strcmp(bytes1, bytes2);                              
}
#define ELEMENT_COMP CAT(ELEMENT_TYPE, _comp)
#endif

typedef struct STACK_TYPE STACK_TYPE;

typedef struct CAT(STACK_TYPE, _Type) {
    bool (*in)(STACK_TYPE * stack, ELEMENT_TYPE el);
    size_t (*len)(STACK_TYPE * stack);
    err_type (*get)(STACK_TYPE * stack, size_t index, ELEMENT_TYPE * value);
    err_type (*set)(STACK_TYPE * stack, size_t index, ELEMENT_TYPE value);
    void (*dest)(STACK_TYPE * stack);
    err_type (*pop)(STACK_TYPE * stack, ELEMENT_TYPE * value);
    err_type (*push)(STACK_TYPE * stack, ELEMENT_TYPE value);
    err_type (*peek)(STACK_TYPE * stack, ELEMENT_TYPE * value);
    void (*for_each)(STACK_TYPE * stack, int (*handle_item)(void * data, ELEMENT_TYPE value), void * data);
} CAT(STACK_TYPE, _Type);

struct STACK_TYPE {
    CAT(STACK_TYPE, _Type) const * _class;
    ELEMENT_TYPE * bins;
    size_t capacity;
    size_t fill;
};

static err_type CAT(STACK_TYPE, _pop)(STACK_TYPE * stack, ELEMENT_TYPE * value);
static err_type CAT(STACK_TYPE, _peek)(STACK_TYPE * stack, ELEMENT_TYPE * value);
static err_type CAT(STACK_TYPE, _push)(STACK_TYPE * stack, ELEMENT_TYPE value);
static err_type CAT(STACK_TYPE, _get)(STACK_TYPE * stack, size_t key, ELEMENT_TYPE * value);
static bool CAT(STACK_TYPE, _in)(STACK_TYPE * stack, ELEMENT_TYPE el);
static size_t CAT(STACK_TYPE, _len)(STACK_TYPE * stack);
static err_type CAT(STACK_TYPE, _set)(STACK_TYPE * stack, size_t key, ELEMENT_TYPE value);
static void CAT(STACK_TYPE, _dest)(STACK_TYPE * stack);
static void CAT(STACK_TYPE, _for_each)(STACK_TYPE * stack, int (*handle_item)(void * data, ELEMENT_TYPE value), void * data);
static err_type CAT(STACK_TYPE, _resize)(STACK_TYPE * stack, size_t new_capacity);
static err_type CAT(STACK_TYPE, _init)(STACK_TYPE * stack, size_t init_capacity);

static const CAT(STACK_TYPE, _Type) CAT(STACK_TYPE, _class) = {
    .in = &(CAT(STACK_TYPE, _in)),
    .len = &(CAT(STACK_TYPE, _len)),
    .dest = &(CAT(STACK_TYPE, _dest)),
    .get = &(CAT(STACK_TYPE, _get)),
    .set = &(CAT(STACK_TYPE, _set)),
    .pop = &(CAT(STACK_TYPE, _pop)),
    .push = &(CAT(STACK_TYPE, _push)),
    .peek = &(CAT(STACK_TYPE, _peek)),
    .for_each = &(CAT(STACK_TYPE, _for_each)),
};

static err_type CAT(STACK_TYPE, _pop)(STACK_TYPE * stack, ELEMENT_TYPE * value) {
    err_type status = CAT(STACK_TYPE, _peek)(stack, value);
    if (status) {
        return status;
    }
    stack->fill--;
    return PEGGY_SUCCESS;
}
static err_type CAT(STACK_TYPE, _peek)(STACK_TYPE * stack, ELEMENT_TYPE * value) {
    if (!stack->fill) {
        return PEGGY_EMPTY_STACK;
    }
    memcpy((void *)value, (void *)(stack->bins + stack->fill - 1), sizeof(ELEMENT_TYPE));
    return PEGGY_SUCCESS;
}
static err_type CAT(STACK_TYPE, _push)(STACK_TYPE * stack, ELEMENT_TYPE value) {
    err_type status = PEGGY_SUCCESS;
    if (stack->fill == stack->capacity) {
        if (status = CAT(STACK_TYPE, _resize)(stack, stack->capacity * STACK_DEFAULT_SCALE)) {
            return status;
        }
    }
    stack->fill++; // before set to ensure it succeeds
    return CAT(STACK_TYPE, _set)(stack, stack->fill - 1, value);
}
static err_type CAT(STACK_TYPE, _get)(STACK_TYPE * stack, size_t key, ELEMENT_TYPE * value) {
    if (key < stack->fill) {
        memcpy((void *)value, (void *)(stack->bins + key), sizeof(ELEMENT_TYPE));
        return PEGGY_SUCCESS;
    }
    return PEGGY_INDEX_OUT_OF_BOUNDS;
}
static bool CAT(STACK_TYPE, _in)(STACK_TYPE * stack, ELEMENT_TYPE el) {
    for (size_t i = 0; i < stack->fill; i++) {
        if (!ELEMENT_COMP(stack->bins[i], el)) {
            return true;
        }
    }
    return false;
}
static size_t CAT(STACK_TYPE, _len)(STACK_TYPE * stack) {
    return stack->fill;
}
static err_type CAT(STACK_TYPE, _set)(STACK_TYPE * stack, size_t key, ELEMENT_TYPE value) {
    if (key < stack->fill) {
        memcpy((void *)(stack->bins + key), (void *)&value, sizeof(ELEMENT_TYPE));
        return PEGGY_SUCCESS;
    }
    return PEGGY_INDEX_OUT_OF_BOUNDS;
}
static void CAT(STACK_TYPE, _dest)(STACK_TYPE * stack) {
    free(stack->bins);
    stack->bins = NULL;
    stack->capacity = 0;
    stack->fill = 0;
}
static void CAT(STACK_TYPE, _for_each)(STACK_TYPE * stack, int (*handle_item)(void * data, ELEMENT_TYPE value), void * data) {
    for (size_t i = 0; i < stack->fill; i++) {
        if (handle_item(data, stack->bins[i])) {
            break;
        }
    }
}
static err_type CAT(STACK_TYPE, _resize)(STACK_TYPE * stack, size_t new_capacity) {
    ELEMENT_TYPE * new_bins = realloc(stack->bins, sizeof(*stack->bins) * new_capacity);
    if (!new_bins) {
        return PEGGY_MALLOC_FAILURE;
    }
    stack->bins = new_bins;
    stack->capacity = new_capacity;
    return PEGGY_SUCCESS;
}
static err_type CAT(STACK_TYPE, _init)(STACK_TYPE * stack, size_t init_capacity) {
    if (!init_capacity) {
        init_capacity = STACK_DEFAULT_SIZE;
    }
    stack->bins = NULL;
    stack->capacity = 0;
    stack->fill = 0;
    stack->_class = &(CAT(STACK_TYPE, _class));
    return CAT(STACK_TYPE, _resize)(stack, init_capacity);
}

#undef STACK_TYPE
#undef ELEMENT_COMP
#undef ELEMENT_TYPE
#undef STACK_DEFAULT_SIZE
#undef STACK_DEFAULT_SCALE

#endif // defined(ELEMENT_TYPE)