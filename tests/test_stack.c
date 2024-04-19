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
#include <peggy/stack.h>

#define ELEMENT_TYPE cstring
#define ELEMENT_COMP cstr_comp
#include <peggy/stack.h>

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

void print_size_t_stack(STACK(size_t) * stack) {
    stack->_class->for_each(stack, &print_size_t, NULL);
}

void print_cstring_stack(STACK(cstring) * stack) {
    stack->_class->for_each(stack, &print_cstring, NULL);
}

void test_cstring_stack(void) {
    STACK(cstring) stack;
    STACK_INIT(cstring)(&stack, 0);

    for (size_t i = 0; i < N_STRINGS; i++) {
        printf("adding '%s' to back of stack\n", strings[i]);
        stack._class->push(&stack, strings[i]);
    }
    assert(stack.fill == N_STRINGS);
    print_cstring_stack(&stack);
    size_t j = 0;
    while (stack._class->len(&stack)) {
        cstring out;
        stack._class->pop(&stack, &out);
        printf("popped '%s' from back of stack\n", out);
        
        j++;
        if (j == N_STRINGS / 2) {
            printf("\ncurrent contents:\n");
            print_cstring_stack(&stack);
        }
    }
    printf("\nfinal contents:\n");
    print_cstring_stack(&stack);
}

void test_size_t_stack(void) {
    STACK(size_t) stack;
    STACK_INIT(size_t)(&stack, 0);

    for (size_t i = 0; i < N_SIZE_T; i++) {
        printf("adding '%zu' to back of stack\n", values[i]);
        stack._class->push(&stack, values[i]);
    }
    assert(stack.fill == N_SIZE_T);
    print_size_t_stack(&stack);
    size_t j = 0;
    while (stack._class->len(&stack)) {
        size_t out;
        stack._class->pop(&stack, &out);
        printf("popped '%zu' from back of stack\n", out);
        
        j++;
        if (j == N_SIZE_T / 2) {
            printf("\ncurrent contents:\n");
            print_size_t_stack(&stack);
        }
    }
    printf("\nfinal contents:\n");
    print_size_t_stack(&stack);
}

int main(void) {
    printf("built!\n");

    test_cstring_stack();
    test_size_t_stack();

    return 0;
}