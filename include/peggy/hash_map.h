
/**
 * This is an implementation of a type-safe unordered hash map using open 
 * addressing with linear probing and a simple sentinel for indicating 
 * removed items
 *      FEATURES:
 *          perfectly fine to store const values (uses memcpy)
 *          minimal requirements for simple templating:
 *              declaration: HASH_MAP(key_type, value_type) map;
 *              initialization: HASH_MAP_INIT(key_type, value_type)(&map, 0)
 *              no other accesses require specifying the key_type and 
 *                  value_type. All other functionality are in member function 
 *                  calls to "map"
 *      WARNINGS:
 *          insertion order is NOT maintained
 *          key and value types must be single identifiers (structs, enums, 
 *              unions and base C types must be typedef'd)
 *          to any handlers or setting values, data is copied by value so 
 *              anything larger than a pointer should probably just be a 
 *              pointer
 *          if HASH_FUNC or KEY_COMP are not specified, default will compare 
 *              directly by value meaning pointers will be compared by address 
 *              and not by value. This is similar to C++ unordered_map 
 *              especially where passing a raw c-string (char *), the contents 
 *              are not used, i.e. different pointers to the same string will 
 *              not match
 *          if either HASH_FUNC or KEY_COMP are implemented, the other should 
 *              be as well so that:
 *              KEY_COMP(key1, key2) == 0 implies 
 *              HASH_FUNC(key1, any_value) == HASH_FUNC(key2, any_value)
 * 
 * To include in a header and get access to macros and declarations and NO 
 *      implementations, simply do not define either KEY_TYPE or VALUE_TYPE
 * 
 * To use an implementation: 
 *  set #defines
 *  include this header
 * 
 * the following #defines configure the hash map:
 *      required:
 *          KEY_TYPE
 *          VALUE_TYPE
 *      optional:
 *          KEY_COMP - The function used to compare two different KEY_TYPE 
 *              values. If not specified a default is built
 *              required function prototype:
 *                  int KEY_COMP(KEY_TYPE key1, KEY_TYPE key2)
 *                  returns 0 if key1 == key2 and non-zero if key1 != key2
 *          HASH_FUNC - The function used to compute hashes of the KEY_TYPE 
 *              values. If not specified a default is built
 *              required function prototype:
 *                  size_t HASH_FUNC(KEY_TYPE key, size_t hash_size)
 *                  returns a value in range [0, hash_size)
 *          HASH_FILL_RESIZE_RATIO - ratio above which the hash_map will 
 *              re-size and re-insert all entries into hash map. Defaults to 2 / 5
 * 
 */

#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // for debug print statements
#include <peggy/utils.h>
#include "mempool.h"

typedef enum hash_map_err {
    HM_SUCCESS = 0,
    HM_KEY_NOT_FOUND,
    HM_MALLOC_MAP_FAILURE,
    HM_MALLOC_PAIR_FAILURE,
    HM_INSERTION_FAILURE
} hash_map_err;

#define HASH_DEFAULT_SIZE 127

/* sentinel */
#define HASH_PAIR_REMOVED ((void *)1)

#define HASH_MAP_INIT(k, v) CAT(CAT3(k, _, v), _init)

#define HASH_FUNCTION(type) CAT(type, _hash)
#define KEY_COMPARE(type) CAT(type, _comp)
#define HASH_MAP(k, v) CAT(CAT3(k, _, v), _map)
#define HASH_MAP_CLASS(k, v) CAT(CAT3(k, _, v), _class)

#include <peggy/hash_utils.h>

#endif // HASH_MAP_H

/* generate the implementation */
#if defined(KEY_TYPE) && defined(VALUE_TYPE)

#ifndef HASH_FILL_RESIZE_RATIO
#define HASH_FILL_RESIZE_RATIO .5
#endif

#ifndef HASH_PAIR_POOL_COUNT
#define HASH_PAIR_POOL_COUNT 256
#endif

#define HASH_COMBO CAT3(KEY_TYPE, _, VALUE_TYPE)
#define HASH_PAIR CAT(HASH_COMBO, _pair)
#define HASH_MAP_TYPE CAT(HASH_COMBO, _map)

#ifndef HASH_FUNC
static size_t CAT(KEY_TYPE, _hash)(KEY_TYPE key, size_t bin_size) {
    char bytes[sizeof(KEY_TYPE)+1];
    bytes[sizeof(KEY_TYPE)] = '\0';
    memcpy(bytes, &key, sizeof(KEY_TYPE));
    return cstr_hash(bytes, bin_size);
}
#define HASH_FUNC HASH_FUNCTION(KEY_TYPE)
#endif

#ifndef KEY_COMP
static int CAT(KEY_TYPE, _comp)(KEY_TYPE key1, KEY_TYPE key2) {
    char bytes1[sizeof(KEY_TYPE)+1];
    char bytes2[sizeof(KEY_TYPE)+1];
    bytes1[sizeof(KEY_TYPE)] = '\0', bytes2[sizeof(KEY_TYPE)] = '\0';
    memcpy(bytes1, &key1, sizeof(KEY_TYPE));
    memcpy(bytes2, &key2, sizeof(KEY_TYPE));
    return strcmp(bytes1, bytes2);                              
}
#define KEY_COMP KEY_COMPARE(KEY_TYPE)
#endif

typedef struct HASH_PAIR {
    KEY_TYPE key;
    VALUE_TYPE value;
} HASH_PAIR;

#if __STDC_VERSION__ < 201112L && !defined(_Alignof)
struct CAT(HASH_PAIR,_OFFSET) {
	char a;
	HASH_PAIR b;
};
#endif

typedef struct HASH_MAP_TYPE HASH_MAP_TYPE;

typedef struct CAT(HASH_COMBO, _Type) {
    bool (*in)(HASH_MAP_TYPE * map, KEY_TYPE key);
    hash_map_err (*get)(HASH_MAP_TYPE * map, KEY_TYPE key, VALUE_TYPE * value);
    hash_map_err (*set)(HASH_MAP_TYPE * map, KEY_TYPE key, VALUE_TYPE value);
    void (*dest)(HASH_MAP_TYPE * map);
    hash_map_err (*pop)(HASH_MAP_TYPE * map, KEY_TYPE key, VALUE_TYPE * value);
    hash_map_err (*remove)(HASH_MAP_TYPE * map, KEY_TYPE key);
    void (*for_each)(HASH_MAP_TYPE * map, int (*handle_item)(void * data, KEY_TYPE key, VALUE_TYPE value), void * data);
} CAT(HASH_COMBO, _Type);

struct HASH_MAP_TYPE {
    CAT(HASH_COMBO, _Type) const * _class;
    HASH_PAIR ** bins;
    size_t capacity;
    size_t fill;
    MemPoolManager * pair_mgr;
};

static hash_map_err CAT(HASH_COMBO, _insert)(HASH_MAP_TYPE * map, HASH_PAIR * pair);
static HASH_PAIR * CAT(HASH_COMBO, _get_pair)(HASH_MAP_TYPE * map, KEY_TYPE key, size_t * bin_num);
static hash_map_err CAT(HASH_COMBO, _pop)(HASH_MAP_TYPE * map, KEY_TYPE key, VALUE_TYPE * value);
static hash_map_err CAT(HASH_COMBO, _remove)(HASH_MAP_TYPE * map, KEY_TYPE key);
static hash_map_err CAT(HASH_COMBO, _get)(HASH_MAP_TYPE * map, KEY_TYPE key, VALUE_TYPE * value);
static bool CAT(HASH_COMBO, _in)(HASH_MAP_TYPE * map, KEY_TYPE key);
static hash_map_err CAT(HASH_COMBO, _set)(HASH_MAP_TYPE * map, KEY_TYPE key, VALUE_TYPE value);
static void CAT(HASH_COMBO, _dest)(HASH_MAP_TYPE * map);
static void CAT(HASH_COMBO, _for_each)(HASH_MAP_TYPE * map, int (*handle_item)(void * data, KEY_TYPE key, VALUE_TYPE value), void * data);
static hash_map_err CAT(HASH_COMBO, _resize)(HASH_MAP_TYPE * map, size_t new_capacity);
static hash_map_err CAT(HASH_COMBO, _init)(HASH_MAP_TYPE * map, size_t init_capacity);

static const CAT(HASH_COMBO, _Type) CAT(HASH_COMBO, _class) = {
    .in = &(CAT(HASH_COMBO, _in)),
    .dest = &(CAT(HASH_COMBO, _dest)),
    .get = &(CAT(HASH_COMBO, _get)),
    .set = &(CAT(HASH_COMBO, _set)),
    .pop = &(CAT(HASH_COMBO, _pop)),
    .remove = &(CAT(HASH_COMBO, _remove)),
    .for_each = &(CAT(HASH_COMBO, _for_each)),
};

static HASH_PAIR ** CAT(HASH_COMBO, _linear_probe)(HASH_PAIR ** bins, size_t start_index, size_t capacity) {
    if (*(bins + start_index)) {
        HASH_PAIR ** bin_begin = bins + start_index;
        HASH_PAIR ** bin = bin_begin + 1;
        HASH_PAIR ** bin_cap = bins + capacity;
        if (bin == bin_cap) {
            bin = bins;
        }
        
        while (*bin != NULL && bin != bin_begin) {
            if (++bin == bin_cap) {
                bin = bins;
            }
        }
        if (bin == bin_begin) {
            return NULL;
        }
        bins = bin;
    } else {
        bins = bins + start_index;
    }
    return bins;
}

/* inserts a key-value pair into the hash map */
static hash_map_err CAT(HASH_COMBO, _insert)(HASH_MAP_TYPE * map, HASH_PAIR * pair) {
    //printf("inserting pair into map of size %zu (%zu)\n", map->fill, map->capacity);
    hash_map_err status = HM_SUCCESS;
    if (map->fill >= map->capacity * HASH_FILL_RESIZE_RATIO) { /* this will even work if the HASH_MAP has been cleared since map->fill == map->capacity == 0 */
        size_t new_cap = next_prime(2*map->capacity);
        //printf("resizing hash %zu to %zu\n",map->capacity, new_cap);
        if ((status = CAT(HASH_COMBO, _resize)(map, new_cap))) {
            return status;
        }
    }
    size_t bin_start = HASH_FUNC(pair->key, map->capacity);
    
    HASH_PAIR ** bins = map->bins;

    bins = CAT(HASH_COMBO, _linear_probe)(bins, bin_start, map->capacity);
    if (bins) {
        *bins = pair;
    } else {
        return HM_INSERTION_FAILURE;
    }
    
    /*
    HASH_PAIR * p;
    while ((p = bins[bin_start], p) && (p != HASH_PAIR_REMOVED)) {
        if (++bin_start == map->capacity) {
            bin_start = 0;
        }
    }
    
    //printf("inserting pair at %p\n", (void *)(map->bins + bin));
    bins[bin_start] = pair;
    */
    map->fill++;
    return status;
}

/* never returns a new pointer. returns NULL if not found. Should never return the sentinel */
static HASH_PAIR * CAT(HASH_COMBO, _get_pair)(HASH_MAP_TYPE * map, KEY_TYPE key, size_t * bin_num) {
    
    size_t bin_start = HASH_FUNC(key, map->capacity);
    HASH_PAIR ** bins = map->bins;
    HASH_PAIR ** bin_begin = bins + bin_start;

    if (*bin_begin && *bin_begin != HASH_PAIR_REMOVED && !KEY_COMP((*bin_begin)->key, key)) {
        if (bin_num) {
            *bin_num = bin_start;
        }
        return *bin_begin;
    }
    if (!*bin_begin) {
        if (bin_num) {
            *bin_num = bin_start;
        }
        return NULL;
    }
    
    HASH_PAIR ** bin = bin_begin + 1;
    HASH_PAIR ** bin_cap = bins + map->capacity;
    if (bin >= bin_cap) {
        bin = bins;
    }
    
    while (*bin && bin != bin_begin) {
        if (*bin != HASH_PAIR_REMOVED && !KEY_COMP((*bin)->key, key)) {
            if (bin_num) {
                *bin_num = (size_t) (bin - bins);
            }
            return *bin;
        }
        if (++bin >= bin_cap) {
            bin = bins;
        }
    }
    return NULL;
    
    /*
    //printf("getting pair\n");
    size_t bin_start = HASH_FUNC(key, map->capacity);
    //printf("starting search at %zu\n", bin_start);
    HASH_PAIR * pair = map->bins[bin_start];
    //printf("address at starting pointer %p\n", (void *) (map->bins + bin_start));
    if (!(pair && ((pair == HASH_PAIR_REMOVED) || KEY_COMP(pair->key, key)))) {
        if (bin_num) {
            *bin_num = bin_start;
        }
        return pair;
    }
    size_t bin = bin_start + 1;
    if (bin == map->capacity) {
        bin = 0;
    }
    while ((bin != bin_start) && (pair = map->bins[bin], pair && ((pair == HASH_PAIR_REMOVED) || KEY_COMP(pair->key, key)))) {
        if (++bin == map->capacity) {
            bin = 0;
        }
    }
    if (bin == bin_start) { // prevents returning the HASH_PAIR_REMOVED sentinel value
        return NULL;
    }
    if (bin_num) {
        *bin_num = bin;
    }
    
    return pair;
    */
}

static hash_map_err CAT(HASH_COMBO, _pop)(HASH_MAP_TYPE * map, KEY_TYPE key, VALUE_TYPE * value) {
    //printf("popping key from hash_map\n");
    size_t bin_num;
    HASH_PAIR * pair = CAT(HASH_COMBO, _get_pair)(map, key, &bin_num); /* cannot return the HASH_PAIR_REMOVED sentinel */
    if (!pair) {
        return HM_KEY_NOT_FOUND;
    }
    if (value) {
        memcpy(value, &(pair->value), sizeof(VALUE_TYPE));
    }
    free(map->bins[bin_num]);
    //printf("setting bin to HASH_PAIR_REMOVED\n");
    map->bins[bin_num] = HASH_PAIR_REMOVED;
    map->fill--;
    return HM_SUCCESS;
}

static hash_map_err CAT(HASH_COMBO, _remove)(HASH_MAP_TYPE * map, KEY_TYPE key) {
    return CAT(HASH_COMBO, _pop)(map, key, NULL);
}

/* a good reason for doing it by passing a pointer to VALUE_TYPE * value is that it acts as a default as well. If not found, the value is not changed */
static hash_map_err CAT(HASH_COMBO, _get)(HASH_MAP_TYPE * map, KEY_TYPE key, VALUE_TYPE * value) {
    //printf("getting value from key\n");
    HASH_PAIR * pair = CAT(HASH_COMBO, _get_pair)(map, key, NULL);
    if (!pair) {
        return HM_KEY_NOT_FOUND;
    }
    if (value) {
        memcpy(value, &(pair->value), sizeof(VALUE_TYPE));
    }
    return HM_SUCCESS;
}

static bool CAT(HASH_COMBO, _in)(HASH_MAP_TYPE * map, KEY_TYPE key) {
    return CAT(HASH_COMBO, _get_pair)(map, key, NULL) != NULL;
}

static hash_map_err CAT(HASH_COMBO, _set)(HASH_MAP_TYPE * map, KEY_TYPE key, VALUE_TYPE value) {
    //printf("setting key to value\n");
    HASH_PAIR * pair = CAT(HASH_COMBO, _get_pair)(map, key, NULL);
    if (pair) { /* key-value pair already exists, overwriting value */
        //printf("overwriting existing value\n");
        memmove(&(pair->value), &value, sizeof(VALUE_TYPE));
        return HM_SUCCESS;
    } else {
        //printf("found open key-value pair\n");
    }
    pair = MemPoolManager_next(map->pair_mgr);
    // above replaces:
    // pair = malloc(sizeof(HASH_PAIR));
    if (!pair) {
        return HM_MALLOC_PAIR_FAILURE;
    }
    memcpy(&(pair->key), &key, sizeof(KEY_TYPE));
    memcpy(&(pair->value), &value, sizeof(VALUE_TYPE));
    return CAT(HASH_COMBO, _insert)(map, pair);
}

/* probably encapsulate this is some iteration, but save that for cexpress */
static void CAT(HASH_COMBO, _dest)(HASH_MAP_TYPE * map) {
    //printf("clearing map\n");
    if (map->bins) {
        /*
        for (size_t i = 0; i < map->capacity; i++) {
            HASH_PAIR * p = map->bins[i];
            if (p && (p != HASH_PAIR_REMOVED)) {
                free(p);
            }
            map->bins[i] = NULL;
        }
        */
        free(map->bins);
        map->bins = NULL;
    }
    if (map->pair_mgr) {
        MemPoolManager_del(map->pair_mgr);
    }
    map->pair_mgr = NULL;
    map->fill = 0;
    map->capacity = 0;
}

static void CAT(HASH_COMBO, _for_each)(HASH_MAP_TYPE * map, int (*handle_item)(void * data, KEY_TYPE key, VALUE_TYPE value), void * data) {
    //printf("executing for_each for hash with capacity %zu...", map->capacity);
    int status = 0;
    size_t i = 0;
    for (i = 0; i < map->capacity; i++) {
        HASH_PAIR * p = map->bins[i];
        //printf("handling item with address %p\n", (void*) p);
        if (p && (p != HASH_PAIR_REMOVED)) {
            
            if ((status = handle_item(data, p->key, p->value))) {
                break;
            }
        }
    }
    //printf("%zu cleared\n", i);
}

static hash_map_err CAT(HASH_COMBO, _init)(HASH_MAP_TYPE * map, size_t init_capacity) {
    if (!init_capacity) {
        init_capacity = HASH_DEFAULT_SIZE;
    }
    //printf("initializing map to %zu\n", init_capacity);
    map->bins = NULL;
    map->capacity = 0;
    map->fill = 0;
#if __STDC_VERSION__ < 201112L && !defined(_Alignof)
	map->pair_mgr = MemPoolManager_new(HASH_PAIR_POOL_COUNT, sizeof(HASH_PAIR), offsetof(struct  CAT(HASH_PAIR,_OFFSET), b));
#else
    map->pair_mgr = MemPoolManager_new(HASH_PAIR_POOL_COUNT, sizeof(HASH_PAIR), _Alignof(HASH_PAIR));
#endif
    map->_class = &(CAT(HASH_COMBO, _class));
    return CAT(HASH_COMBO, _resize)(map, init_capacity);
}

static hash_map_err CAT(HASH_COMBO, _resize)(HASH_MAP_TYPE * map, size_t new_capacity) {
    //printf("resizing map from %zu to %zu\n", map->capacity, new_capacity);
    if (!new_capacity) {
        CAT(DELAY(HASH_COMBO), _dest)(map);
        return HM_SUCCESS;
    }
    HASH_PAIR ** old_bins = map->bins;
    size_t old_capacity = map->capacity;
    map->bins = calloc(new_capacity, sizeof(*(map->bins)));
    map->fill = 0;
    map->capacity = new_capacity;
    if (!map->bins) {
        return HM_MALLOC_MAP_FAILURE;
    }
    //printf("hash_map bins starting from %p to %p\n", (void *)map->bins, (void *)(map->bins + new_capacity));
    hash_map_err status = HM_SUCCESS;
    for (size_t i = 0; i < old_capacity; i++) {
        HASH_PAIR * bin = old_bins[i];
        if (bin && bin != HASH_PAIR_REMOVED) {
            if ((status = CAT(HASH_COMBO, _insert)(map, bin))) {
                return status;
            }
        }
    }
    free(old_bins); // need to free old bins. OK if old_bins is also NULL
    return HM_SUCCESS;
}

#undef HASH_COMBO
#undef HASH_PAIR
#undef HASH_MAP_TYPE
#undef HASH_FUNC
#undef HASH_FILL_RESIZE_RATIO
#undef KEY_COMP
#undef KEY_TYPE
#undef VALUE_TYPE

#endif // if defined(KEY_TYPE) && defined(VALUE_TYPE)

