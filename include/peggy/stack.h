
/**
 * This is an implementation of a type-safe stack
 *      FEATURES:
 *          perfectly fine to store struct with const values (uses memcpy)
 *          minimal requirements for simple templating:
 *              declaration: STACK(element_type) stack;
 *              initialization: STACK_INIT(element_type)(&stack, 0)
 *              no other accesses require specifying the key_type and 
 *                  value_type. All other functionality are in vtable function 
 *                  calls to "stack"
 *      WARNINGS:
 *          element types must be single identifiers (structs, enums, 
 *              unions, pointers and base C types must be typedef'd)
 *          to any handlers or setting values, data is copied by value so 
 *              anything larger than a pointer should probably just be a 
 *              pointer
 *          if ELEMENT_COMP is not specified, default will compare 
 *              directly by value meaning pointers will be compared by address 
 *              and not by value. For example different pointers to the same 
 *              string will not match
 * 
 * To include in a header and get access to macros and declarations and NO 
 *      implementations, simply do not define ELEMENT_TYPE
 * 
 * To use an implementation: 
 *  set #defines
 *  include this header
 * 
 * the following #defines configure the stack:
 *      required:
 *          ELEMENT_TYPE
 *      optional:
 *          ELEMENT_COMP - The function used to compare two different elements
 *              in the stack
 *              required function prototype:
 *                  int ELEMENT_COMP(ELEMENT_TYPE val1, ELEMENT_TYPE val2)
 *                  returns 0 if val1 == val2 and non-zero if val1 != val2
 * 
 */

#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <peggy/utils.h>

#define STACK_INIT(k) CAT(k, Stack_init)

#define STACK(k) CAT(k, Stack)
#define STACK_CLASS(k) CAT(k, Stack_class) 

#endif // STACK_H

#if defined(ELEMENT_TYPE)

#ifndef STACK_DEFAULT_SIZE
#define STACK_DEFAULT_SIZE MAXIMUM(128 / sizeof(ELEMENT_TYPE), 16)
#endif // STACK_DEFAULT_SIZE

#ifndef STACK_DEFAULT_SCALE
#define STACK_DEFAULT_SCALE 2
#endif // STACK_DEFAULT_SCALE

#define STACK_TYPE STACK(ELEMENT_TYPE)

/**
 * @brief default comparison function. copies bytes into automatic buffers
 * and compares the bytes 
 */
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

/**
 * @brief vtable struct for stack
 */
typedef struct CAT(STACK_TYPE, _Type) {
    bool (*in)(STACK_TYPE * stack, ELEMENT_TYPE el);
    size_t (*len)(STACK_TYPE * stack);
    err_type (*get)(STACK_TYPE const * stack, size_t index, ELEMENT_TYPE * value);
    err_type (*set)(STACK_TYPE * stack, size_t index, ELEMENT_TYPE value);
    void (*dest)(STACK_TYPE * stack);
    err_type (*pop)(STACK_TYPE * stack, ELEMENT_TYPE * value);
    err_type (*push)(STACK_TYPE * stack, ELEMENT_TYPE value);
    err_type (*peek)(STACK_TYPE * stack, ELEMENT_TYPE * value);
    void (*for_each)(STACK_TYPE * stack, int (*handle_item)(void * data, ELEMENT_TYPE value), void * data);
    err_type (*resize)(STACK_TYPE * stack, size_t new_capacity);
} CAT(STACK_TYPE, _Type);

/**
 * @brief base struct for the stack
 */
struct STACK_TYPE {
    CAT(STACK_TYPE, _Type) const * _class;
    ELEMENT_TYPE * bins;
    size_t capacity;
    size_t fill;
};

// Forward declarations
static err_type CAT(STACK_TYPE, _pop)(STACK_TYPE * stack, ELEMENT_TYPE * value);
static err_type CAT(STACK_TYPE, _peek)(STACK_TYPE * stack, ELEMENT_TYPE * value);
static err_type CAT(STACK_TYPE, _push)(STACK_TYPE * stack, ELEMENT_TYPE value);
static err_type CAT(STACK_TYPE, _get)(STACK_TYPE const * stack, size_t key, ELEMENT_TYPE * value);
static bool CAT(STACK_TYPE, _in)(STACK_TYPE * stack, ELEMENT_TYPE el);
static size_t CAT(STACK_TYPE, _len)(STACK_TYPE * stack);
static err_type CAT(STACK_TYPE, _set)(STACK_TYPE * stack, size_t key, ELEMENT_TYPE value);
static void CAT(STACK_TYPE, _dest)(STACK_TYPE * stack);
static void CAT(STACK_TYPE, _for_each)(STACK_TYPE * stack, int (*handle_item)(void * data, ELEMENT_TYPE value), void * data);
static err_type CAT(STACK_TYPE, _resize)(STACK_TYPE * stack, size_t new_capacity);
static err_type CAT(STACK_TYPE, _init)(STACK_TYPE * stack, size_t init_capacity);

/**
 * @brief definition of the vtable
 */
static const CAT(STACK_TYPE, _Type) CAT(STACK_TYPE, _class) = {
    .in = &(CAT(STACK_TYPE, _in)),      //!< test for presence of key in stack
    .len = &(CAT(STACK_TYPE, _len)),    //!< get height of stack, # of elements
    .dest = &(CAT(STACK_TYPE, _dest)),  //!< destroy the stack and reclaim 
                                        //!<    internal memory
    .get = &(CAT(STACK_TYPE, _get)),    //!< retrieve a value at an index
    .set = &(CAT(STACK_TYPE, _set)),    //!< set a value at an index
    .pop = &(CAT(STACK_TYPE, _pop)),    //!< retrieve and remove a value at a 
                                        //!<   specified index
    .push = &(CAT(STACK_TYPE, _push)),  //!< add a value to the top of the stack
    .peek = &(CAT(STACK_TYPE, _peek)),  //!< retrieve the value at the top of
                                        //!<    the stack, do not remove
    .for_each = &(CAT(STACK_TYPE, _for_each)),  //!< apply a function to each
                                                //!< element of the stack
    .resize = &(CAT(STACK_TYPE, _resize))       //!< resize the underlying array
};

/**
 * @brief retrieve value and remove from stack
 * @param[in] stack the stack from which to remove the top value
 * @param[out] value a pointer to the value at the top of the stack, if found, 
 *      else no change
 * @returns a non-zero value if stack is empty, else 0
 */
static err_type CAT(STACK_TYPE, _pop)(STACK_TYPE * stack, ELEMENT_TYPE * value) {
    err_type status = CAT(STACK_TYPE, _peek)(stack, value);
    if (status) {
        return status;
    }
    stack->fill--;
    return PEGGY_SUCCESS;
}

/**
 * @brief retrieve value at the top of the stack, do not remove
 * @param[in] stack the stack from which to view the top value
 * @param[out] value a pointer to the value at the top of the stack, if found, 
 *      else no change
 * @returns a non-zero value if stack is empty, else 0
 */
static err_type CAT(STACK_TYPE, _peek)(STACK_TYPE * stack, ELEMENT_TYPE * value) {
    if (!stack->fill) {
        return PEGGY_EMPTY_STACK;
    }
    if (value) {
        memcpy((void *)value, (void *)(stack->bins + stack->fill - 1), sizeof(ELEMENT_TYPE));
    }
    return PEGGY_SUCCESS;
}

/**
 * @brief add a value to the top of the stack
 * @param[in] stack the stack to which a value will be added
 * @param[in] value an instance of ELEMENT_TYPE to be placed at the top of the 
 *      stack
 * @returns a non-zero value if element cannot be added else 0
 */
static err_type CAT(STACK_TYPE, _push)(STACK_TYPE * stack, ELEMENT_TYPE value) {
    err_type status = PEGGY_SUCCESS;
    if (stack->fill == stack->capacity) {
        if ((status = CAT(STACK_TYPE, _resize)(stack, stack->capacity * STACK_DEFAULT_SCALE))) {
            return status;
        }
    }
    stack->fill++; // before set to ensure it succeeds
    return CAT(STACK_TYPE, _set)(stack, stack->fill - 1, value);
}

/**
 * @brief retrieve a value at an index
 * @param[in] stack the stack from which to get a value
 * @param[in] key the index from which to query value
 * @param[out] value a pointer to where the value retrieved will be stored
 * @returns a non-zero value if key is out of bounds, else 0
 */
static err_type CAT(STACK_TYPE, _get)(STACK_TYPE const * stack, size_t key, ELEMENT_TYPE * value) {
    if (key < stack->fill) {
        memcpy((void *)value, (void *)(stack->bins + key), sizeof(ELEMENT_TYPE));
        return PEGGY_SUCCESS;
    }
    return PEGGY_INDEX_OUT_OF_BOUNDS;
}

/**
 * @brief test for presence of key in stack. For this to have sensible results
 *      ELEMENT_COMP should have been defined to an appropriate function
 * @param[in] stack the stack from which to get a value
 * @param[in] el an instance of ELEMENT_TYPE to find in stack
 * @returns true if element is found else false
 */
static bool CAT(STACK_TYPE, _in)(STACK_TYPE * stack, ELEMENT_TYPE el) {
    for (size_t i = 0; i < stack->fill; i++) {
        if (!ELEMENT_COMP(stack->bins[i], el)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief wrapper to get length/height of stack
 */
static size_t CAT(STACK_TYPE, _len)(STACK_TYPE * stack) {
    return stack->fill;
}

/**
 * @brief set a value at a specific index in the stack
 * @param[in] stack the stack to which a value will be set
 * @param[in] key the index at which to store an instance of ELEMENT_TYPE
 *      NOTE: key must be within the bounds of the underlying stack array
 * @param[in] value an instance of ELEMENT_TYPE to store at key in the stack
 * @returns a non-zero value if key is out of bounds, else 0
 */
static err_type CAT(STACK_TYPE, _set)(STACK_TYPE * stack, size_t key, ELEMENT_TYPE value) {
    if (key < stack->fill) {
        memcpy((void *)(stack->bins + key), (void *)&value, sizeof(ELEMENT_TYPE));
        return PEGGY_SUCCESS;
    }
    return PEGGY_INDEX_OUT_OF_BOUNDS;
}

/**
 * @brief destroy and reclaim memory from the stack
 */
static void CAT(STACK_TYPE, _dest)(STACK_TYPE * stack) {
    //printf("destroying the stack\n");
    if (stack->bins) {
        free(stack->bins);
    }
    stack->bins = NULL;
    stack->capacity = 0;
    stack->fill = 0;
}

/**
 * @brief apply a function for each element in the stack iteratively from
 *      bottom to top
 * @param[in] stack the stack over which to iterate
 * @param[in] handle_item 
 *      @brief the function to apply to each element
 *      @param[inout] data a context pointer for each element
 *      @param[in] value the instance of ELEMENT_TYPE at current iteration 
 *      @returns 0 to continue iteration, non-zero to abort
 * @param[inout] data the context pointer to pass to the handle_item function
 */
static void CAT(STACK_TYPE, _for_each)(STACK_TYPE * stack, int (*handle_item)(void * data, ELEMENT_TYPE value), void * data) {
    for (size_t i = 0; i < stack->fill; i++) {
        if (handle_item(data, stack->bins[i])) {
            break;
        }
    }
}

/**
 * @brief resize the underlying array
 * @param[in] stack the stack over which to iterate
 * @param[in] new_capacity the new capacity
 * @returns non-zero if reallocation fails else 0
 */
static err_type CAT(STACK_TYPE, _resize)(STACK_TYPE * stack, size_t new_capacity) {
    ELEMENT_TYPE * new_bins = calloc(new_capacity, sizeof(*stack->bins));
    if (!new_bins) {
        return PEGGY_MALLOC_FAILURE;
    }
    if (stack->bins) {
        memcpy(new_bins, stack->bins, stack->fill * sizeof(ELEMENT_TYPE));
        free(stack->bins);
    }
    stack->bins = new_bins;
    stack->capacity = new_capacity;
    return PEGGY_SUCCESS;
}

/**
 * @brief initialize the stack
 * @param[in] stack the stack
 * @param[in] init_capacity the initial capacity to assign to the stack. 
 *      STACK_DEFAULT_SIZE is provided if set to 0
 * @returns an error if initialization fails else 0
 */
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

// undef all the input macros so that the header can be re-included
#undef STACK_TYPE
#undef ELEMENT_COMP
#undef ELEMENT_TYPE
#undef STACK_DEFAULT_SIZE
#undef STACK_DEFAULT_SCALE

#endif // defined(ELEMENT_TYPE)

