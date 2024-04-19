#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <peggy/utils.h>
#include <peggy/token.h>
#include <peggy/rule.h>

#define N_STRING1 10
#define N_STRING2 6
#define N_STRING3 3
#define N_STRING4 3
#define N_TOKENS (N_STRING1 + N_STRING2 + N_STRING3 + N_STRING4)

char const string1[] = "I am the very model of a modern major general";
char const string2[] = "break it down for us camacho";
char const string3[] = "vini vidi vici";
char const string4[] = "sad, you are";

Token toks[N_TOKENS] = {
    {.string = string1, ._class = &Token_class, .start = 0, .end = 1},
    {.string = string1, ._class = &Token_class, .start = 2, .end = 4},
    {.string = string1, ._class = &Token_class, .start = 5, .end = 8},
    {.string = string1, ._class = &Token_class, .start = 9, .end = 13},
    {.string = string1, ._class = &Token_class, .start = 14, .end = 19},
    {.string = string1, ._class = &Token_class, .start = 20, .end = 22},
    {.string = string1, ._class = &Token_class, .start = 23, .end = 24},
    {.string = string1, ._class = &Token_class, .start = 25, .end = 31},
    {.string = string1, ._class = &Token_class, .start = 32, .end = 37},
    {.string = string1, ._class = &Token_class, .start = 38, .end = 45},
    {.string = string2, ._class = &Token_class, .start = 0, .end = 5},
    {.string = string2, ._class = &Token_class, .start = 6, .end = 8},
    {.string = string2, ._class = &Token_class, .start = 9, .end = 13},
    {.string = string2, ._class = &Token_class, .start = 14, .end = 17},
    {.string = string2, ._class = &Token_class, .start = 18, .end = 20},
    {.string = string2, ._class = &Token_class, .start = 21, .end = 28},
    {.string = string3, ._class = &Token_class, .start = 0, .end = 4},
    {.string = string3, ._class = &Token_class, .start = 5, .end = 9},
    {.string = string3, ._class = &Token_class, .start = 10, .end = 14},
    {.string = string4, ._class = &Token_class, .start = 0, .end = 4},
    {.string = string4, ._class = &Token_class, .start = 5, .end = 8},
    {.string = string4, ._class = &Token_class, .start = 9, .end = 12}
};

void test_init_dest(void) {
    printf("testing initialization...");
    PackratCache cache = DEFAULT_PACKRATCACHE;

    assert(cache.capacity == 0);
    assert(cache.fill == 0);
    assert(cache.bins == NULL);
    assert(cache._class != NULL);

    PackratCache_init(&cache);

    assert(cache.capacity == 3);
    assert(cache.fill == 0);
    assert(cache.bins != NULL);
    assert(cache._class != NULL);

    PackratCache_dest(&cache);

    printf("PASS\n");
}

void test_set_get(void) {

    printf("testing set...");
    PackratCache cache = DEFAULT_PACKRATCACHE;

    assert(cache.fill == 0);
    assert(cache.capacity == 0);

    HASH_MAP_INIT(pToken, pSAMap)(&cache, 1);
    assert(cache.fill == 0);
    assert(cache.capacity == 1);

    SAMap * samap;
    ASTNode * node = NULL;

    hash_map_err status = PackratCache_get(&cache, &toks[0], &node);
    DEBUG_ASSERT(status == HM_KEY_NOT_FOUND, "found node at %p when none were set. expected HM_KEY_NOT_FOUND error\n", (void *)node);

    for (int i = 0; i < N_STRING1; i++) {
        PackratCache_set(&cache, &toks[i], (ASTNode *)(uintptr_t)(i + 1));
        assert(cache.fill == 1);
        DEBUG_ASSERT(cache.capacity == 3, "unexpected cache capacity after %d sets: %zu, expected %zu\n", i+1, cache.capacity, (size_t) 3);
        PackratCache_get(&cache, &toks[i], &node);
        DEBUG_ASSERT(node == (ASTNode *)(uintptr_t)(i + 1), "failed to retrieve stored node. %p, expected %p\n", (void*)node, (void*)(uintptr_t)(i + 1));
        cache._class->get(&cache, &toks[i], &samap);
        DEBUG_ASSERT(samap != NULL, "failed to retrieve SAMap from PackractCache\n");
        DEBUG_ASSERT(samap->fill == (size_t)i + 1, "unexpected fill size after %d sets: %zu, expected %d\n", i + 1, samap->fill, i + 1);
        if (i < HASH_DEFAULT_SIZE * 2 / 5) { // 2/5 is HASH_FILL_RESIZE_RATIO, which is not exposed
            DEBUG_ASSERT(samap->capacity == HASH_DEFAULT_SIZE, "unexpected cache capacity after %d sets: %zu, expected %d\n", i+1, samap->capacity, HASH_DEFAULT_SIZE);
        } else {
            DEBUG_ASSERT(samap->capacity == next_prime(2 * HASH_DEFAULT_SIZE), "unexpected cache capacity after %d sets: %zu, expected %zu\n", i+1, samap->capacity, next_prime(2 * HASH_DEFAULT_SIZE));
        }
        
    }

    for (int i = 0; i < N_STRING2; i++) {
        PackratCache_set(&cache, &toks[N_STRING1 + i], (ASTNode *)(uintptr_t)(i + 1));
        assert(cache.fill == 2);
        DEBUG_ASSERT(cache.capacity == 7, "unexpected cache capacity after %d sets: %zu, expected %zu\n", i+1, cache.capacity, (size_t) 7);
        PackratCache_get(&cache, &toks[N_STRING1 + i], &node);
        DEBUG_ASSERT(node == (ASTNode *)(uintptr_t)(i + 1), "failed to retrieve stored node. %p, expected %p\n", (void*)node, (void*)(uintptr_t)(i + 1));
        cache._class->get(&cache, &toks[N_STRING1 + i], &samap);
        DEBUG_ASSERT(samap != NULL, "failed to retrieve SAMap from PackractCache\n");
        DEBUG_ASSERT(samap->fill == (size_t)i + 1, "unexpected fill size after %d sets: %zu, expected %d\n", i + 1, samap->fill, i + 1);
        if (i < HASH_DEFAULT_SIZE * 2 / 5) { // 2/5 is HASH_FILL_RESIZE_RATIO, which is not exposed
            DEBUG_ASSERT(samap->capacity == HASH_DEFAULT_SIZE, "unexpected cache capacity after %d sets: %zu, expected %d\n", i+1, samap->capacity, HASH_DEFAULT_SIZE);
        } else {
            DEBUG_ASSERT(samap->capacity == next_prime(2 * HASH_DEFAULT_SIZE), "unexpected cache capacity after %d sets: %zu, expected %zu\n", i+1, samap->capacity, next_prime(2 * HASH_DEFAULT_SIZE));
        }
    }

    for (int i = 0; i < N_STRING3; i++) {
        PackratCache_set(&cache, &toks[N_STRING1 + N_STRING2 + i], (ASTNode *)(uintptr_t)(i + 1));
        assert(cache.fill == 3);
        DEBUG_ASSERT(cache.capacity == 17, "unexpected cache capacity after %d sets: %zu, expected %zu\n", i+1, cache.capacity, (size_t) 17);
        PackratCache_get(&cache, &toks[N_STRING1 + N_STRING2 + i], &node);
        DEBUG_ASSERT(node == (ASTNode *)(uintptr_t)(i + 1), "failed to retrieve stored node. %p, expected %p\n", (void*)node, (void*)(uintptr_t)(i + 1));
        cache._class->get(&cache, &toks[N_STRING1 + N_STRING2 + i], &samap);
        DEBUG_ASSERT(samap != NULL, "failed to retrieve SAMap from PackractCache\n");
        DEBUG_ASSERT(samap->fill == (size_t)i + 1, "unexpected fill size after %d sets: %zu, expected %d\n", i + 1, samap->fill, i + 1);
        if (i < HASH_DEFAULT_SIZE * 2 / 5) { // 2/5 is HASH_FILL_RESIZE_RATIO, which is not exposed
            DEBUG_ASSERT(samap->capacity == HASH_DEFAULT_SIZE, "unexpected cache capacity after %d sets: %zu, expected %d\n", i+1, samap->capacity, HASH_DEFAULT_SIZE);
        } else {
            DEBUG_ASSERT(samap->capacity == next_prime(2 * HASH_DEFAULT_SIZE), "unexpected cache capacity after %d sets: %zu, expected %zu\n", i+1, samap->capacity, next_prime(2 * HASH_DEFAULT_SIZE));
        }
    }

    for (int i = 0; i < N_STRING4; i++) {
        PackratCache_set(&cache, &toks[N_STRING1 + N_STRING2 + N_STRING3 + i], (ASTNode *)(uintptr_t)(i + 1));
        assert(cache.fill == 4);
        DEBUG_ASSERT(cache.capacity == 17, "unexpected cache capacity after %d sets: %zu, expected %zu\n", i+1, cache.capacity, (size_t) 17);
        PackratCache_get(&cache, &toks[N_STRING1 + N_STRING2 + N_STRING3 + i], &node);
        DEBUG_ASSERT(node == (ASTNode *)(uintptr_t)(i + 1), "failed to retrieve stored node. %p, expected %p\n", (void*)node, (void*)(uintptr_t)(i + 1));
        cache._class->get(&cache, &toks[N_STRING1 + N_STRING2 + N_STRING3 + i], &samap);
        DEBUG_ASSERT(samap != NULL, "failed to retrieve SAMap from PackractCache\n");
        DEBUG_ASSERT(samap->fill == (size_t)i + 1, "unexpected fill size after %d sets: %zu, expected %d\n", i + 1, samap->fill, i + 1);
        if (i < HASH_DEFAULT_SIZE * 2 / 5) { // 2/5 is HASH_FILL_RESIZE_RATIO, which is not exposed
            DEBUG_ASSERT(samap->capacity == HASH_DEFAULT_SIZE, "unexpected cache capacity after %d sets: %zu, expected %d\n", i+1, samap->capacity, HASH_DEFAULT_SIZE);
        } else {
            DEBUG_ASSERT(samap->capacity == next_prime(2 * HASH_DEFAULT_SIZE), "unexpected cache capacity after %d sets: %zu, expected %zu\n", i+1, samap->capacity, next_prime(2 * HASH_DEFAULT_SIZE));
        }
    }

    PackratCache_dest(&cache);

    printf("PASS\n");

}

void print_tokens(void) {
    for (int i = 0; i < N_TOKENS; i++) {
        Token_print(&toks[i]);
        printf("\n");
    }
}

int main(void) {
    printf("Tokens:\n");
    print_tokens();
    test_init_dest();
    test_set_get();
    return 0;
}