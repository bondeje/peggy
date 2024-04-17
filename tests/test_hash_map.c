#include <stddef.h>
#include <assert.h>
#include <stdio.h>

int size_t_comp(size_t a, size_t b) {
    if (a > b) {
        return 1;
    } else if (a < b) {
        return -1;
    }
    return 0;
}

typedef char const * cstring;

#define KEY_TYPE size_t
#define VALUE_TYPE cstring
#define KEY_COMP size_t_comp
#include <peggy/hash_map.h>

#define KEY_TYPE cstring
#define VALUE_TYPE size_t
#define KEY_COMP cstr_comp
#define HASH_FUNC cstr_hash
#include <peggy/hash_map.h>

#define N_STRINGS 8
cstring strings[] = {
    "Hello darkness my old friend",
    "All around me are familiar faces",
    "I am the very model of a modern major general",
    "Push me away, make me fall",
    "Before I forget",
    "I know you better than that",
    "Lass mich deine Traene reiten ubers Kinn nach Afrika",
    "what"
};

size_t values[20] = {3, 1, 4, 5, 9, 2, 6, 8, 7, 0, 3, 1, 4, 5, 9, 2, 6, 8, 7, 0};

int print_cstring_size_t_pairs(void * data, cstring key, size_t value) {
    printf("\t%s = %zu\n", key, value);
    return 0;
}

void test_cstring_size_t_map(void) {
    printf("\nIn test_cstring_size_t_map\n");
    hash_map_err status = HM_SUCCESS;
    HASH_MAP(cstring, size_t) map;

    assert(!(status = HASH_MAP_INIT(cstring, size_t)(&map, 0)));
    if (status) {
        printf("init failed with status %d\n", status);
    }
    printf("fill: %zu\ncapacity: %zu\n", map.fill, map.capacity);
    printf("setting initial values for strings\n");
    for (size_t i = 0; i < N_STRINGS; i++) {
        cstring s = strings[i];
        assert(!(status = map.set(&map, s, values[i])));
        if (status) {
            printf("set failed with status %d\n", status);
        }    
    }
    printf("fill: %zu\ncapacity: %zu\n", map.fill, map.capacity);
    printf("printing initial values for strings\n");
    for (size_t i = 0; i < N_STRINGS; i++) {
        size_t v;
        cstring s = strings[i];
        assert(!(status = map.get(&map, s, &v)));
        if (status) {
            printf("get failed with status %d\n", status);
        }
        printf("\t%s = %zu\n", s, v);
    }
    printf("printing initial values for strings using a buffer\n");
    char buffer[50] = {'\0'};
    for (size_t i = 0; i < N_STRINGS; i++) {
        size_t v;
        size_t N = strlen(strings[i]);
        memcpy(buffer, strings[i], N);
        buffer[N] = '\0';
        assert(!(status = map.get(&map, buffer, &v)));
        if (status) {
            printf("get failed with status %d\n", status);
        }
        printf("\t%s = %zu\n", buffer, v);
    }
    printf("setting secondary values for strings\n");
    for (size_t i = 0; i < N_STRINGS; i++) {
        cstring s = strings[i];
        assert(!(status = map.set(&map, s, values[i + N_STRINGS])));
        if (status) {
            printf("set failed with status %d\n", status);
        }    
    }
    printf("fill: %zu\ncapacity: %zu\n", map.fill, map.capacity);
    printf("printing secondary values for strings\n");
    for (size_t i = 0; i < N_STRINGS; i++) {
        size_t v;
        cstring s = strings[i];
        assert(!(status = map.get(&map, s, &v)));
        if (status) {
            printf("get failed with status %d\n", status);
        }
        printf("\t%s = %zu\n", s, v);
    }
    printf("fill: %zu\ncapacity: %zu\n", map.fill, map.capacity);
    {
        size_t v;
        cstring removed = strings[3];
        assert(!(status = map.pop(&map, strings[3], &v)));
        if (status) {
            printf("pop failed with status %d\n", status);
        }
        printf("removed key-value pair: %s = %zu\n", removed, v);
    }
    printf("printing secondary values for strings\n");
    for (size_t i = 0; i < N_STRINGS; i++) {
        size_t v;
        cstring s = strings[i];
        status = map.get(&map, s, &v);
        if (status) {
            printf("get failed for key %s with status %d\n", s, status);
        }
        printf("\t%s = %zu\n", s, v);
    }
    printf("fill: %zu\ncapacity: %zu\n", map.fill, map.capacity);
    printf("printing with for_each\n");
    map.for_each(&map, &print_cstring_size_t_pairs, NULL);
}

void test_size_t_cstring_map(void) {
    printf("\nIn test_size_t_cstring_map\n");
    hash_map_err status = HM_SUCCESS;
    HASH_MAP(size_t, cstring) map;

    assert(!(status = HASH_MAP_INIT(size_t, cstring)(&map, 0)));
    if (status) {
        printf("init failed with status %d\n", status);
    }
    assert(!(status = map.set(&map, 5, "I am the very model of a modern major general\n")));
    if (status) {
        printf("set failed with status %d\n", status);
    }
    cstring retrieved;
    assert(!(status = map.get(&map, 5, &retrieved)));
    if (status) {
        printf("get failed with status %d\n", status);
    }
    printf("retrieved string: %s\n", retrieved);
}

int main(void) {
    test_size_t_cstring_map();

    test_cstring_size_t_map();

    return 0;
}