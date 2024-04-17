#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <peggy/hash_map.h> // for cstr_comp until I refactor

int size_t_comp(size_t a, size_t b) {
    if (a > b) {
        return 1;
    } else if (a < b) {
        return -1;
    }
    return 0;
}

typedef char const * cstring;

#define ELEMENT_TYPE size_t
#define ELEMENT_COMP size_t_comp
#include <peggy/deque.h>

#define ELEMENT_TYPE cstring
#define ELEMENT_COMP cstr_comp
#include <peggy/deque.h>

#define N_STRINGS 8
cstring strings[N_STRINGS] = {
    "Hello darkness my old friend",
    "All around me are familiar faces",
    "I am the very model of a modern major general",
    "Push me away, make me fall",
    "Before I forget",
    "I know you better than that",
    "Lass mich deine Traene reiten ubers Kinn nach Afrika",
    "what"
};

#define N_SIZE_T 20
size_t values[N_SIZE_T] = {3, 1, 4, 5, 9, 2, 6, 8, 7, 0, 3, 1, 4, 5, 9, 2, 6, 8, 7, 0};

int print_cstring(void * data, cstring str) {
    printf("%s\n", str);
    return 0;
}

int print_size_t(void * data, size_t s) {
    printf("%zu\n", s);
    return 0;
}

void print_size_t_deque(DEQUE(size_t) * deque) {
    deque->_class->for_each(deque, &print_size_t, NULL);
}

void print_cstring_deque(DEQUE(cstring) * deque) {
    deque->_class->for_each(deque, &print_cstring, NULL);
}

void test_cstring_deque(void) {
    
}

void test_size_t_deque(void) {
    
}

int main(void) {
    printf("built!\n");

    return 0;
}