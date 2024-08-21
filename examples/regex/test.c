#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "reNFA.h"
#include "reparser.h"
#include "peggy/mempool.h"
#include "test_utils.h"
#include "test_re_utils.h"

char const * TEST_REGEX[] = {
    "a",
    "ab",
    "a|b",
    "ab+",
    "ab\\+",
    "ab{,}",
    "ab{,0}",
    "ab{0,0}",
    "ab{,1}",
    "ab{0,1}",
    "ab{1,2}",
    "ab{2}",
    "ab{2,5}",
    "ab\\{",
    "a(bc){,}",
    "a(bc){,0}",
    "a(bc){0,0}",
    "a(bc){,1}",
    "a(bc){0,1}",
    "a(bc){1,2}",
    "a(bc){2}",
    "a(bc){2,5}",
    "a(bc)\\{",
    "a(bc)\\+",
    "a(bc)+",
    "[abc]",
    "[^abc]",
    "[a-c]",
    "(a|b)*abb",
    NULL
};

char const * TEST_REGEX_PP[] = {
    "a",
    "ab",
    "a|b",
    "abb*",
    "ab+",
    "ab*",
    "ab*",
    "ab*",
    "ab?",
    "ab?",
    "ab(b)?",
    "abb",
    "abb(b(b(b)?)?)?",
    "ab{",
    "a(bc)*",
    "a(bc)*",
    "a(bc)*",
    "a(bc)?",
    "a(bc)?",
    "a(bc)((bc))?",    
    "a(bc)(bc)",
    "a(bc)(bc)((bc)((bc)((bc))?)?)?",
    "a(bc){",
    "a(bc)+",
    "a(bc)(bc)*",
    "[abc]",
    "[^abc]",
    "[a-c]",
    "(a|b)*abb",
    NULL
};

struct TestState ** TEST_REGEX_NFA[] = {
    //"a"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"ab"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"a|b"
    &(struct TestState *[]){&TEST_STATE(0, 2, &TEST_TRANSITION("", 0, 1), &TEST_TRANSITION("", 0, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("a", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"ab+"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 3), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 4)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 3), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"ab\\+"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("+", 1, 3)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"ab{,}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"ab{,0}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"ab{0,0}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"ab{,1}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"ab{0,1}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"ab{1,2}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 3), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"ab{2}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"ab{2,5}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 4), &TEST_TRANSITION("", 0, 12)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 5)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 6), &TEST_TRANSITION("", 0, 11)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 7)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 8), &TEST_TRANSITION("", 0, 10)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 9)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 10)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("", 0, 11)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("", 0, 12)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"ab\\{"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("{", 1, 3)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"a(bc){,}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"a(bc){,0}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"a(bc){0,0}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"a(bc){,1}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"a(bc){0,1}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"a(bc){1,2}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 4), &TEST_TRANSITION("", 0, 7)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 6)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 7)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"a(bc){2}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 5)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"a(bc){2,5}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 4)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 5)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 6), &TEST_TRANSITION("", 0, 17)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 7)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 8)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 9), &TEST_TRANSITION("", 0, 16)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 10)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 11)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 12), &TEST_TRANSITION("", 0, 15)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 13)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 14)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 15)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("", 0, 16)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("", 0, 17)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"a(bc)\\{"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("{", 1, 4)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"a(bc)\\+"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("+", 1, 4)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"a(bc)+"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 4), &TEST_TRANSITION("", 0, 7)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 6)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 4), &TEST_TRANSITION("", 0, 7)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    //"[abc]"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("abc", 3, 1)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"[^abc]"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("^abc", 4, 1)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"[a-c]"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a-c", 3, 1)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    //"(a|b)*abb"
    
    &(struct TestState *[]){&TEST_STATE(0, 2, &TEST_TRANSITION("", 0, 1), &TEST_TRANSITION("", 0, 7)),
                            &TEST_STATE(2, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("a", 1, 3)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 6)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 5)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 6)),
                            &TEST_STATE(2, 2, &TEST_TRANSITION("", 0, 1), &TEST_TRANSITION("", 0, 7)),
                            &TEST_STATE(2, 1, &TEST_TRANSITION("a", 1, 8)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 9)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 10)),
                            &TEST_STATE(1, 0, NULL)}[0],
    NULL
};

int test_preprocess(void) {
    int nerrors = 0;

    reNFA nfa;
    reNFA_init(&nfa);
    RegexBuilder reb;
    RegexBuilder_init(&reb, &nfa);

    size_t i = 0;
    while (TEST_REGEX[i]) {
        size_t len = strlen(TEST_REGEX[i]);
        RegexBuilder_preprocess(&reb, TEST_REGEX[i], len);
        size_t res_len = strlen(TEST_REGEX_PP[i]);
        int nerrors_i = CHECK(res_len == nfa.regex_len, "preprocessed regex does not match length %.*s. expected: %zu, found: %zu\n", (int)res_len, TEST_REGEX_PP[i], res_len, nfa.regex_len);
        //if (!nerrors_i) {
            nerrors += nerrors_i + CHECK(!strncmp(nfa.regex_s, TEST_REGEX_PP[i], len), "preprocessed regex does not match. expected: (%zu)%.*s, found: (%zu)%.*s\n", res_len, (int)res_len, TEST_REGEX_PP[i], (int)nfa.regex_len, (int)nfa.regex_len, nfa.regex_s);
        //}
        i++;
    }

    reNFA_dest(&nfa);
    RegexBuilder_dest(&reb);
    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }
    return nerrors;
}

int test_NFA(void) {
    int nerrors = 0;

    reNFA nfa;
    RegexBuilder reb;
    size_t j = 0;
    size_t i = 0;
    while (TEST_REGEX_NFA[i]) {
        RegexBuilder_init(&reb, &nfa);
        reNFA_init(&nfa);
        if (!strncmp(TEST_REGEX[i], "(a|b)*abb", strlen(TEST_REGEX[i]))) {
            j = 1;
        }
        size_t regex_len = strlen(TEST_REGEX[i]);
        RegexBuilder_build(&reb, TEST_REGEX[i], regex_len);
        nerrors += check_NFA(&nfa, TEST_REGEX_NFA[i]);
        i++;
        reNFA_dest(&nfa);
        RegexBuilder_dest(&reb);
    }

    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }
    return nerrors;
}

int main(int narg, char ** args) {

    if (narg > 1) {
        if (!strncmp("--verbose", args[1], strlen("--verbose"))) {
            verbose = true;
        }
    }

    test_preprocess();
    test_NFA();

    return 0;
}