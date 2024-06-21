
/**
 * This is an implementation of a type-safe unordered hash set using open 
 * addressing with linear probing and a simple sentinel for indicating 
 * removed items
 *      FEATURES:
 *          perfectly fine to store const values (uses memcpy)
 *          minimal requirements for simple templating:
 *              declaration: HASH_SET(element_type) set;
 *              initialization: HASH_SET_INIT(element_type)(&set, 0)
 *              no other accesses require specifying the element_type. 
 *                  All other functionality are in member function 
 *                  calls to "set"
 *      WARNINGS:
 *          insertion order is NOT maintained
 *          element_type must be a single identifier (structs, enums, 
 *              unions and base C types must be typedef'd)
 *          to any handlers or setting values, data is copied by value so 
 *              anything larger than a pointer should probably just be a 
 *              pointer
 *          if HASH_FUNC or ELEMENT_COMP are not specified, default will compare 
 *              directly by value meaning pointers will be compared by address 
 *              and not by value. This is similar to C++ unordered_set 
 *              especially where passing a raw c-string (char *), the contents 
 *              are not used, i.e. different pointers to the same string will 
 *              not match
 *          if either HASH_FUNC or ELEMENT_COMP are implemented, the other should 
 *              be as well so that:
 *              ELEMENT_COMP(el1, el2) == 0 implies 
 *              HASH_FUNC(el1, any_value) == HASH_FUNC(el2, any_value)
 * 
 * To include in a header and get access to macros and declarations and NO 
 *      implementations, simply do not define ELEMENT_TYPE
 * 
 * To use an implementation: 
 *  set #defines
 *  include this header
 * 
 * the following #defines configure the hash set:
 *      required:
 *          ELEMENT_TYPE
 *      optional:
 *          ELEMENT_COMP - The function used to compare two different ELEMENT_TYPE 
 *              values. If not specified a default is built
 *              required function prototype:
 *                  int ELEMENT_COMP(ELEMENT_TYPE el1, ELEMENT_TYPE el2)
 *                  returns 0 if el1 == el2 and non-zero if el1 != el2
 *          HASH_FUNC - The function used to compute hashes of the ELEMENT_TYPE 
 *              values. If not specified a default is built
 *              required function prototype:
 *                  size_t HASH_FUNC(ELEMENT_TYPE el, size_t hash_size)
 *                  returns a value in range [0, hash_size)
 *          HASH_SET_FILL_RESIZE_RATIO - ratio above which the hash_set will 
 *              re-size and re-insert all entries into hash set. Defaults to 2 / 5
 * 
 */

#ifndef HASH_SET_H
#define HASH_SET_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // for debug print statements
#include <stdbool.h>
#include <peggy/utils.h>

typedef enum hash_set_err {
    HS_SUCCESS = 0,
    HS_ELEMENT_NOT_FOUND,
    HS_MALLOC_SET_FAILURE,
    HS_INSERTION_FAILURE
} hash_set_err;

#define HASH_DEFAULT_SIZE 127

#define HASH_SET_INIT(k) CAT3(k, Set, _init)

#define HASH_FUNCTION(type) CAT(type, _hash)
#define ELEMENT_COMPARE(type) CAT(type, _comp)
#define HASH_SET(k) CAT(k, Set)
#define HASH_SET_CLASS(k) CAT3(k, Set, _class)

#include <peggy/hash_utils.h>

#endif // HASH_SET_H

/* generate the implementation */
#if defined(ELEMENT_TYPE)

#ifndef HASH_SET_FILL_RESIZE_RATIO
#define HASH_SET_FILL_RESIZE_RATIO .4
#endif

#define HASH_COMBO CAT3(ELEMENT_TYPE, Set, )
#define HASH_SET_TYPE HASH_COMBO

#ifndef HASH_FUNC
static size_t CAT(ELEMENT_TYPE, _hash)(ELEMENT_TYPE el, size_t bin_size) {
    char bytes[sizeof(ELEMENT_TYPE)+1];
    bytes[sizeof(ELEMENT_TYPE)] = '\0';
    memcpy(bytes, &el, sizeof(ELEMENT_TYPE));
    return cstr_hash(bytes, bin_size);
}
#define HASH_FUNC HASH_FUNCTION(ELEMENT_TYPE)
#endif

#ifndef ELEMENT_COMP
static int CAT(ELEMENT_TYPE, _comp)(ELEMENT_TYPE el1, ELEMENT_TYPE el2) {
    char bytes1[sizeof(ELEMENT_TYPE)];
    char bytes2[sizeof(ELEMENT_TYPE)];
    memcpy(bytes1, &el1, sizeof(ELEMENT_TYPE));
    memcpy(bytes2, &el2, sizeof(ELEMENT_TYPE));
    return strncmp(bytes1, bytes2, sizeof(ELEMENT_TYPE));
}
#define ELEMENT_COMP ELEMENT_COMPARE(ELEMENT_TYPE)
#endif

typedef struct HASH_SET_TYPE HASH_SET_TYPE;

typedef struct CAT(HASH_COMBO, _Type) {
    bool (*in)(HASH_SET_TYPE * set, ELEMENT_TYPE el);
    hash_set_err (*add)(HASH_SET_TYPE * set, ELEMENT_TYPE el);
    void (*dest)(HASH_SET_TYPE * set);
    hash_set_err (*remove)(HASH_SET_TYPE * set, ELEMENT_TYPE el);
    void (*for_each)(HASH_SET_TYPE * set, int (*handle_item)(void * data, ELEMENT_TYPE el), void * data);
} CAT(HASH_COMBO, _Type);

#define IS_REMOVED(removed, index) (((removed)[index >> CHAR_BIT_DIV_SHIFT] >> (index & CHAR_BIT_MOD_MASK)) & 1)
#define IS_SET(set, index) (((set)[index >> CHAR_BIT_DIV_SHIFT] >> (index & CHAR_BIT_MOD_MASK)) & 1)
#define MARK_SET(set, index) ((set)[index >> CHAR_BIT_DIV_SHIFT] |= (1 << (index & CHAR_BIT_MOD_MASK))
#define MARK_REMOVED(removed, index) ((removed)[index >> CHAR_BIT_DIV_SHIFT] |= (1 << (index & CHAR_BIT_MOD_MASK)))

struct HASH_SET_TYPE {
    CAT(HASH_COMBO, _Type) const * _class;
    ELEMENT_TYPE * bins;
    unsigned char * removed;
    unsigned char * set;
    size_t capacity;
    size_t fill;
};

static hash_set_err CAT(HASH_COMBO, _remove)(HASH_SET_TYPE * set, ELEMENT_TYPE el);
static bool CAT(HASH_COMBO, _in)(HASH_SET_TYPE * set, ELEMENT_TYPE el);
static hash_set_err CAT(HASH_COMBO, _add)(HASH_SET_TYPE * set, ELEMENT_TYPE el);
static void CAT(HASH_COMBO, _dest)(HASH_SET_TYPE * set);
static void CAT(HASH_COMBO, _for_each)(HASH_SET_TYPE * set, int (*handle_item)(void * data, ELEMENT_TYPE el, VALUE_TYPE value), void * data);
static hash_set_err CAT(HASH_COMBO, _resize)(HASH_SET_TYPE * set, size_t new_capacity);
static hash_set_err CAT(HASH_COMBO, _init)(HASH_SET_TYPE * set, size_t init_capacity);

static const CAT(HASH_COMBO, _Type) CAT(HASH_COMBO, _class) = {
    .in = &(CAT(HASH_COMBO, _in)),
    .dest = &(CAT(HASH_COMBO, _dest)),
    .add = &(CAT(HASH_COMBO, _set)),
    .remove = &(CAT(HASH_COMBO, _remove)),
    .for_each = &(CAT(HASH_COMBO, _for_each)),
};

static hash_set_err CAT(HASH_COMBO, _remove)(HASH_SET_TYPE * set, ELEMENT_TYPE el) {
    size_t capacity = set->capacity;
    size_t start = HASH_FUNC(el, capacity);
    size_t index = start;
    if (!IS_SET(set->set, index)) {
        return HS_ELEMENT_NOT_FOUND;
    }
    if (!ELEMENT_COMP(el, set->bins[index])) {
        MARK_REMOVED(set->removed, index);
        return HS_SUCCESS;
    }
    index = (index + 1) % capacity;
    while (index != start && IS_SET(set->set, index)) {
        if (!IS_REMOVED(set->removed, index) && !ELEMENT_COMP(el, set->bins[index])) {
            MARK_REMOVED(set->removed, index);
            return HS_SUCCESS;
        }
        index = (index + 1) % capacity;
    }
    return HS_ELEMENT_NOT_FOUND;
}

/* a good reason for doing it by passing a pointer to VALUE_TYPE * value is that it acts as a default as well. If not found, the value is not changed */
static hash_set_err CAT(HASH_COMBO, _add)(HASH_SET_TYPE * set, ELEMENT_TYPE el) {
    hash_set_err status = HS_SUCCESS;
    if (set->fill > HASH_SET_FILL_RESIZE_RATIO * set->capacity) {
        if ((status = CAT(HASH_COMBO, _resize)(set, next_prime(2*set->capacity)))) {
            return status;
        }
    }
    size_t capacity = set->capacity;
    size_t start = HASH_FUNC(el, capacity);
    size_t index = start;
    while (IS_SET(set->set, index) && !IS_REMOVED(set->removed, index)) {
        if (!ELEMENT_COMP(el, set->bins[index])) {
            return HS_SUCCESS;
        }
        index = (index + 1) % capacity;
    }
    // an empty value or one that has been removed is at index
    // if it has been removed, search until an empty one to check if value is in set
    size_t check = (index + 1) % capacity;
    while (check != start && IS_SET(set->set, check)) {
        if (!ELEMENT_COMP(el, set->bins[check])) {
            return HS_SUCCESS;
        }
        check = (check + 1) % capacity;
    }

    memcpy((void*) set->bins + index, &el, sizeof(ELEMENT_TYPE));
    MARK_SET(set->set, index);
    return HS_SUCCESS;
}

static bool CAT(HASH_COMBO, _in)(HASH_SET_TYPE * set, ELEMENT_TYPE el) {
    size_t capacity = set->capacity;
    size_t start = HASH_FUNC(el, capacity);
    size_t index = start;
    if (!IS_SET(set->set, index)) {
        return false;
    }
    if (!IS_REMOVED(set->removed, index) && !ELEMENT_COMP(el, set->bins[index])) {
        return true;
    }
    index = (index + 1) % capacity;
    while (index != start && IS_SET(set->set, index) && !IS_REMOVED(set->removed, index)) {
        if (!ELEMENT_COMP(el, set->bins[index])) {
            return true;
        }
        index = (index + 1) % capacity;
    }
    return false;
}

/* probably encapsulate this is some iteration, but save that for cexpress */
static void CAT(HASH_COMBO, _dest)(HASH_SET_TYPE * set) {
    //printf("clearing set\n");
    if (set->bins) {
        free(set->bins);
        set->bins = NULL;
        free(set->set);
        set->set = NULL;
        set->removed = NULL; // don't have to free because set and removed come from same allocated pool
    }
    set->fill = 0;
    set->capacity = 0;
}

static void CAT(HASH_COMBO, _for_each)(HASH_SET_TYPE * set, int (*handle_item)(void * data, ELEMENT_TYPE el), void * data) {
    int status = 0;
    size_t i = 0;
    for (i = 0; i < set->capacity; i++) {
        if (IS_SET(set, i) && !IS_REMOVED(set, i)) {
            if ((status = handle_item(data, set->bins[i]))) {
                break;
            }
        }
    }
}

static hash_set_err CAT(HASH_COMBO, _init)(HASH_SET_TYPE * set, size_t init_capacity) {
    if (!init_capacity) {
        init_capacity = HASH_DEFAULT_SIZE;
    }
    //printf("initializing set to %zu\n", init_capacity);
    set->bins = NULL;
    set->capacity = 0;
    set->fill = 0;
    set->removed = NULL;
    set->set = NULL;
    set->_class = &(CAT(HASH_COMBO, _class));
    return CAT(HASH_COMBO, _resize)(set, init_capacity);
}

static hash_set_err CAT(HASH_COMBO, _resize)(HASH_SET_TYPE * set, size_t new_capacity) {
    //printf("resizing set from %zu to %zu\n", set->capacity, new_capacity);
    if (!new_capacity) {
        CAT(DELAY(HASH_COMBO), _dest)(set);
        return HS_SUCCESS;
    }
    size_t nbytes_new = (new_capacity >> CHAR_BIT_DIV_SHIFT) + 1;
    ELEMENT_TYPE * old_bins = set->bins;
    size_t old_capacity = set->capacity;
    size_t nbytes_old = (old_capacity >> CHAR_BIT_DIV_SHIFT) + 1;
    set->bins = malloc(new_capacity * sizeof(*(set->bins)));
    if (!set->bins) {
        goto bin_malloc_fail;
    }
    unsigned char * old_bytes = set->set;
    set->set = calloc(nbytes_new * 2, sizeof(unsigned char));
    if (!set->set) {
        goto bytes_malloc_fail;
    }
    set->fill = 0;
    set->capacity = new_capacity;
    set->removed = set->set + nbytes_new;
    for (size_t i = 0; i < old_capacity; i++) {
        if (IS_SET(old_bytes, i) && !IS_REMOVED(old_bytes + nbytes_old, i)) {
            CAT(HASH_COMBO, _add)(set, old_bins[i]);
        }
    }
    free(old_bins); // need to free old bins. OK if old_bins is also NULL
    free(old_bytes);
    return HS_SUCCESS;
bytes_malloc_fail:
    free(set->bins);
bin_malloc_fail:
    set->bins = old_bins;
    return HS_MALLOC_SET_FAILURE;
}

#undef HASH_COMBO
#undef HASH_SET_TYPE
#undef HASH_FUNC
#undef HASH_SET_FILL_RESIZE_RATIO
#undef ELEMENT_COMP
#undef ELEMENT_TYPE

#endif // if defined(ELEMENT_TYPE) && defined(VALUE_TYPE)