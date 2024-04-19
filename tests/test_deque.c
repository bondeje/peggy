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
    DEQUE(cstring) deque;
    DEQUE_INIT(cstring)(&deque);

    for (size_t i = 0; i < N_STRINGS; i++) {
        if (i & 1) {
            printf("adding '%s' to front of deque\n", strings[i]);
            deque._class->push_front(&deque, strings[i]);
        } else {
            printf("adding '%s' to back of deque\n", strings[i]);
            deque._class->push(&deque, strings[i]);
        }
    }
    print_cstring_deque(&deque);
    size_t j = 0;
    while (deque._class->len(&deque)) {
        cstring out;
        if (j & 1) {
            deque._class->pop_front(&deque, &out);
            printf("popped '%s' from front deque\n", out);
        } else {
            deque._class->pop(&deque, &out);
            printf("popped '%s' from back of deque\n", out);
        }
        
        j++;
        if (j == N_STRINGS / 2) {
            printf("\ncurrent contents:\n");
            print_cstring_deque(&deque);
        }
    }
    printf("\nfinal contents:\n");
    print_cstring_deque(&deque);
    
}

void test_size_t_deque(void) {
    DEQUE(size_t) deque;
    DEQUE_INIT(size_t)(&deque);

    for (size_t i = 0; i < N_SIZE_T; i++) {
        if (i & 1) {
            printf("adding '%zu' to front of deque\n", values[i]);
            deque._class->push_front(&deque, values[i]);
        } else {
            printf("adding '%zu' to back of deque\n", values[i]);
            deque._class->push(&deque, values[i]);
        }
    }
    print_size_t_deque(&deque);
    size_t j = 0;
    while (deque._class->len(&deque)) {
        size_t out;
        if (j & 1) {
            deque._class->pop_front(&deque, &out);
            printf("popped '%zu' from front deque\n", out);
        } else {
            deque._class->pop(&deque, &out);
            printf("popped '%zu' from back of deque\n", out);
        }
        
        j++;
        if (j == N_SIZE_T / 2) {
            printf("\ncurrent contents:\n");
            print_size_t_deque(&deque);
        }
    }
    printf("\nfinal contents:\n");
    print_size_t_deque(&deque);
}

int main(void) {
    printf("built!\n");

    test_cstring_deque();
    test_size_t_deque();

    return 0;
}