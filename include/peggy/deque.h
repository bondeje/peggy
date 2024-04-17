/* for inclusion without implementation */
#ifndef DEQUE_H
#define DEQUE_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <peggy/utils.h>

#define DEQUE_INIT(k) CAT(k, Deque_init)

#define DEQUE(k) CAT(k, Deque)

#endif // DEQUE_H

#if defined(ELEMENT_TYPE)

#define DEQUE_TYPE DEQUE(ELEMENT_TYPE)

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

typedef struct DEQUE_TYPE DEQUE_TYPE;
#define DEQUE_NODE_TYPE CAT3(DEQUE_TYPE, , _node)
typedef struct DEQUE_NODE_TYPE DEQUE_NODE_TYPE;

typedef struct CAT(DEQUE_TYPE, _Type) {
    bool (*in)(DEQUE_TYPE * DEQUE, ELEMENT_TYPE el);
    size_t (*len)(DEQUE_TYPE * DEQUE);
    err_type (*get)(DEQUE_TYPE * DEQUE, size_t index, ELEMENT_TYPE * value);
    err_type (*set)(DEQUE_TYPE * DEQUE, size_t index, ELEMENT_TYPE value);
    void (*dest)(DEQUE_TYPE * DEQUE);
    err_type (*pop)(DEQUE_TYPE * DEQUE, ELEMENT_TYPE * value);
    err_type (*push)(DEQUE_TYPE * DEQUE, ELEMENT_TYPE value);
    err_type (*peek)(DEQUE_TYPE * DEQUE, ELEMENT_TYPE * value);
    err_type (*pop_front)(DEQUE_TYPE * DEQUE, ELEMENT_TYPE * value);
    err_type (*push_front)(DEQUE_TYPE * DEQUE, ELEMENT_TYPE value);
    err_type (*peek_front)(DEQUE_TYPE * DEQUE, ELEMENT_TYPE * value);
    void (*for_each)(DEQUE_TYPE * DEQUE, int (*handle_item)(void * data, ELEMENT_TYPE), void * data);
} CAT(DEQUE_TYPE, _Type);

struct DEQUE_NODE_TYPE {
    DEQUE_NODE_TYPE * prev;
    DEQUE_NODE_TYPE * next;
    ELEMENT_TYPE value;
};

struct DEQUE_TYPE {
    CAT(DEQUE_TYPE, _Type) const * _class;
    DEQUE_NODE_TYPE * head;
    DEQUE_NODE_TYPE * tail;
    size_t fill;
};

static err_type CAT(DEQUE_TYPE, _pop)(DEQUE_TYPE * deque, ELEMENT_TYPE * value);
static err_type CAT(DEQUE_TYPE, _peek)(DEQUE_TYPE * deque, ELEMENT_TYPE * value);
static err_type CAT(DEQUE_TYPE, _push)(DEQUE_TYPE * deque, ELEMENT_TYPE value);
static err_type CAT(DEQUE_TYPE, _pop_front)(DEQUE_TYPE * deque, ELEMENT_TYPE * value);
static err_type CAT(DEQUE_TYPE, _peek_front)(DEQUE_TYPE * deque, ELEMENT_TYPE * value);
static err_type CAT(DEQUE_TYPE, _push_front)(DEQUE_TYPE * deque, ELEMENT_TYPE value);
static err_type CAT(DEQUE_TYPE, _get)(DEQUE_TYPE * deque, size_t key, ELEMENT_TYPE * value);
static bool CAT(DEQUE_TYPE, _in)(DEQUE_TYPE * deque, ELEMENT_TYPE el);
static size_t CAT(DEQUE_TYPE, _len)(DEQUE_TYPE * deque);
static err_type CAT(DEQUE_TYPE, _set)(DEQUE_TYPE * deque, size_t key, ELEMENT_TYPE value);
static void CAT(DEQUE_TYPE, _dest)(DEQUE_TYPE * deque);
static void CAT(DEQUE_TYPE, _for_each)(DEQUE_TYPE * deque, int (*handle_item)(void * data, ELEMENT_TYPE value), void * data);
static err_type CAT(DEQUE_TYPE, _init)(DEQUE_TYPE * deque);

static const CAT(DEQUE_TYPE, _Type) CAT(DEQUE_TYPE, _class) = {
    .in = &(CAT(DEQUE_TYPE, _in)),
    .len = &(CAT(DEQUE_TYPE, _len)),
    .dest = &(CAT(DEQUE_TYPE, _dest)),
    .get = &(CAT(DEQUE_TYPE, _get)),
    .set = &(CAT(DEQUE_TYPE, _set)),
    .pop = &(CAT(DEQUE_TYPE, _pop)),
    .push = &(CAT(DEQUE_TYPE, _push)),
    .peek = &(CAT(DEQUE_TYPE, _peek)),
    .pop_front = &(CAT(DEQUE_TYPE, _pop_front)),
    .push_front = &(CAT(DEQUE_TYPE, _push_front)),
    .peek_front = &(CAT(DEQUE_TYPE, _peek_front)),
    .for_each = &(CAT(DEQUE_TYPE, _for_each)),
};

static err_type CAT(DEQUE_TYPE, _pop)(DEQUE_TYPE * deque, ELEMENT_TYPE * value) {
    DEQUE_NODE_TYPE * node = deque->tail;
    if (!node) {
        return PEGGY_EMPTY_DEQUE;
    }
    if (node->prev) { // there is at least one more element
        deque->tail = node->prev;
        deque->tail->next = NULL;
    } else { // node is last element
        deque->tail = NULL;
        deque->head = NULL;
    }
    node->prev = NULL;
    node->next = NULL;
    deque->fill--;
    memcpy((void *)value, (void *)(&(node->value)), sizeof(ELEMENT_TYPE));
    free(node);
    return PEGGY_SUCCESS;
}
static err_type CAT(DEQUE_TYPE, _peek)(DEQUE_TYPE * deque, ELEMENT_TYPE * value) {
    if (!deque->tail) {
        return PEGGY_EMPTY_DEQUE;
    }
    memcpy((void *)value, (void *)(&(deque->tail->value)), sizeof(ELEMENT_TYPE));
    return PEGGY_SUCCESS;
}
static err_type CAT(DEQUE_TYPE, _push)(DEQUE_TYPE * deque, ELEMENT_TYPE value) {
    DEQUE_NODE_TYPE * node = malloc(sizeof(*node));
    if (!node) {
        return PEGGY_MALLOC_FAILURE;
    }
    memcpy((void *)(&(node->value)), (void *)(&value), sizeof(ELEMENT_TYPE));
    node->prev = deque->tail;
    node->next = NULL;
    if (deque->tail) { // there is at least one more element
        deque->tail->next = node;
    } else { // node is the first element
        deque->head = node;
    }
    deque->tail = node;
    deque->fill++;
    return PEGGY_SUCCESS;
}
static err_type CAT(DEQUE_TYPE, _pop_front)(DEQUE_TYPE * deque, ELEMENT_TYPE * value) {
    DEQUE_NODE_TYPE * node = deque->head;
    if (!node) {
        return PEGGY_EMPTY_DEQUE;
    }
    if (node->next) { // there is at least one more element
        deque->head = node->next;
        deque->head->prev = NULL;
    } else { // node is last element
        deque->head = NULL;
        deque->tail = NULL;
    }
    node->prev = NULL;
    node->next = NULL;
    deque->fill--;
    memcpy((void *)value, (void *)(&(node->value)), sizeof(ELEMENT_TYPE));
    free(node);
    return PEGGY_SUCCESS;
}
static err_type CAT(DEQUE_TYPE, _peek_front)(DEQUE_TYPE * deque, ELEMENT_TYPE * value) {
    if (!deque->head) {
        return PEGGY_EMPTY_DEQUE;
    }
    memcpy((void *)value, (void *)(&(deque->head->value)), sizeof(ELEMENT_TYPE));
    return PEGGY_SUCCESS;
}
static err_type CAT(DEQUE_TYPE, _push_front)(DEQUE_TYPE * deque, ELEMENT_TYPE value) {
    DEQUE_NODE_TYPE * node = malloc(sizeof(*node));
    if (!node) {
        return PEGGY_MALLOC_FAILURE;
    }
    memcpy((void *)(&(node->value)), (void *)(&value), sizeof(ELEMENT_TYPE));
    node->next = deque->head;
    node->prev = NULL;
    if (deque->head) { // there is at least one more element
        deque->head->prev = node;
    } else { // node is the first element
        deque->tail = node;
    }
    deque->head = node;
    deque->fill++;
    return PEGGY_SUCCESS;
}
static err_type CAT(DEQUE_TYPE, _get)(DEQUE_TYPE * deque, size_t key, ELEMENT_TYPE * value) {
    if (key >= deque->fill) {
        return PEGGY_INDEX_OUT_OF_BOUNDS;
    }
    DEQUE_NODE_TYPE * node = NULL;;
    if (key < deque->fill / 2) {
        size_t i = 0;
        node = deque->head;
        while (i++ < key) {
            node = node->next;
        }
    } else {
        size_t i = deque->fill;
        node = deque->tail;
        while (--i < key) {
            node = node->prev;
        }
    }
    memcpy((void *)value, (void *)(&(node->value)), sizeof(ELEMENT_TYPE));
    return PEGGY_SUCCESS;
}
static bool CAT(DEQUE_TYPE, _in)(DEQUE_TYPE * deque, ELEMENT_TYPE el) {
    DEQUE_NODE_TYPE * node = deque->head;
    while (node) {
        if (!ELEMENT_COMP(node->value, el)) {
            return true;
        }
        node = node->next;
    }
    return false;
}
static size_t CAT(DEQUE_TYPE, _len)(DEQUE_TYPE * deque) {
    return deque->fill;
}
static err_type CAT(DEQUE_TYPE, _set)(DEQUE_TYPE * deque, size_t key, ELEMENT_TYPE value) {
    if (key >= deque->fill) {
        return PEGGY_INDEX_OUT_OF_BOUNDS;
    }
    DEQUE_NODE_TYPE * node = NULL;;
    if (key < deque->fill / 2) {
        size_t i = 0;
        node = deque->head;
        while (i++ < key) {
            node = node->next;
        }
    } else {
        size_t i = deque->fill;
        node = deque->tail;
        while (--i < key) {
            node = node->prev;
        }
    }
    memcpy((void *)(&(node->value)), (void *)(&value), sizeof(ELEMENT_TYPE));
    return PEGGY_SUCCESS;
}
static void CAT(DEQUE_TYPE, _dest)(DEQUE_TYPE * deque) {
    while (deque->tail->prev) {
        DEQUE_NODE_TYPE * node = deque->tail;
        deque->tail = node->prev;
        node->prev = NULL;
        node->next = NULL;
        free(node);
    }
    free(deque->tail);
    deque->tail = NULL;
    deque->fill = 0;
    deque->head = NULL;
}
static void CAT(DEQUE_TYPE, _for_each)(DEQUE_TYPE * deque, int (*handle_item)(void * data, ELEMENT_TYPE value), void * data) {
    DEQUE_NODE_TYPE * node = deque->head;
    while (node) {
        if (handle_item(data, node->value)) {
            break;
        }
        node = node->next;
    }
}
static err_type CAT(DEQUE_TYPE, _init)(DEQUE_TYPE * deque) {
    deque->head = NULL;
    deque->tail = NULL;
    deque->fill = 0;
    deque->_class = &(CAT(DEQUE_TYPE, _class));
}

#undef DEQUE_TYPE
#undef DEQUE_NODE_TYPE
#undef ELEMENT_COMP
#undef ELEMENT_TYPE

#endif // ELEMENT_TYPE