#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nfa.h"
#include "dfa.h"
#include "avram.h"
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
    "ab?",
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
    "abb(a|b)*",
    "[a-zA-Z_][a-zA-Z_0-9]*",
    "[ \t\r\n\v\f]+",
    "//[^\n]*\n",
    "[^/]",
    "\\*",
    "\\*[^/]",
    "[^*]",
    "(\\*[^/])|[^*]",
    "a(?=b)",
    "a(?=\\))",
    "a(?!b)",
    "/\\*((\\*(?!/))|[^*])*\\*/)",
    // check that it can compile it's own more complicated regex
    "[\\\\\\^$.[|()*?]", // character tokens. these are characters that are not escaped in a symbol outside a character class or lookaround
    "[^\\\\\\^$.[|()*?]", // nonclass_escape. these are characters that must be escaped in a symbol outside a character class or lookaround
    "[\\\\\\-\\]\\^]", // unescaped character class symbols
    "[^\\\\\\-\\]\\^]", // escaped character class symbols
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
    "abb(a|b)*",
    "[a-zA-Z_][a-zA-Z_0-9]*", // the all-important identifier
    "[ \t\r\n\v\f][ \t\r\n\v\f]*",
    "//[^\n]*\n",
    "[^/]",
    "\\*",
    "\\*[^/]",
    "[^*]",
    "(\\*[^/])|[^*]",
    "a(?=b)",
    "a(?=\\))",
    "a(?!b)",
    "/\\*((\\*(?!/))|[^*])*\\*/)",
    // check that it can compile it's own more complicated regex
    "[\\\\\\^$.[|()*?]",
    "[^\\\\\\^$.[|()*?]",
    "[\\\\\\-\\]\\^]",
    "[^\\\\\\-\\]\\^]",
    NULL
};

struct TestState ** TEST_REGEX_NFA[] = {
    // "a"
    &(struct TestState *[]){
        &TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
        &TEST_STATE(1, 0, NULL)
    }[0],
    // "ab"
    &(struct TestState *[]){
        &TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
        &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
        &TEST_STATE(1, 0, NULL)
    }[0],
    // "a|b"
    &(struct TestState *[]){&TEST_STATE(0, 2, &TEST_TRANSITION("", 0, 1), &TEST_TRANSITION("", 0, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("a", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "ab+"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 3), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 4)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 3), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "ab\\+"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("+", 1, 3)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    // "ab{,}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "ab{,0}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "ab{0,0}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "ab{,1}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "ab{0,1}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "ab?"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 4)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "ab{1,2}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 3), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "ab{2}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    // "ab{2,5}"
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
    // "ab\\{"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("{", 1, 3)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    // "a(bc){,}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "a(bc){,0}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "a(bc){0,0}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "a(bc){,1}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "a(bc){0,1}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 2), &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 5)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "a(bc){1,2}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 4), &TEST_TRANSITION("", 0, 7)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 5)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 6)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 7)), 
                            &TEST_STATE(2, 0, NULL)}[0],
    // "a(bc){2}"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 4)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 5)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    // "a(bc){2,5}"
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
    // "a(bc)\\{"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("{", 1, 4)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    // "a(bc)\\+"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)), 
                            &TEST_STATE(1, 1, &TEST_TRANSITION("+", 1, 4)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    // "a(bc)+"
    &(struct TestState *[]){
        &TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
        &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)), 
        &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 3)), 
        &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 4), &TEST_TRANSITION("", 0, 7)), 
        &TEST_STATE(2, 1, &TEST_TRANSITION("b", 1, 5)), 
        &TEST_STATE(1, 1, &TEST_TRANSITION("c", 1, 6)), 
        &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 4), &TEST_TRANSITION("", 0, 7)), 
        &TEST_STATE(2, 0, NULL)
    }[0],
    // "[abc]"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("[abc]", 5, 1)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    // "[^abc]"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("[^abc]", 6, 1)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    // "[a-c]"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("[a-c]", 5, 1)), 
                            &TEST_STATE(1, 0, NULL)}[0],
    // "(a|b)*abb"
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
    // "abb(a|b)*"
    &(struct TestState *[]){&TEST_STATE(0, 1, &TEST_TRANSITION("a", 1, 1)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 2)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 3)),
                            &TEST_STATE(1, 2, &TEST_TRANSITION("", 0, 4), &TEST_TRANSITION("", 0, 10)),
                            &TEST_STATE(2, 2, &TEST_TRANSITION("", 0, 5), &TEST_TRANSITION("", 0, 7)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("a", 1, 6)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 9)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("b", 1, 8)),
                            &TEST_STATE(1, 1, &TEST_TRANSITION("", 0, 9)),
                            &TEST_STATE(2, 2, &TEST_TRANSITION("", 0, 10), &TEST_TRANSITION("", 0, 4)),
                            &TEST_STATE(2, 0, NULL)}[0],
    NULL
};

Symbol sym_a = {.sym = "a", .sym_len = 1, .match = reChar_match};
Symbol sym_b = {.sym = "b", .sym_len = 1, .match = reChar_match};
Symbol sym_c = {.sym = "c", .sym_len = 1, .match = reChar_match};
Symbol sym_plus = {.sym = "+", .sym_len = 1, .match = reChar_match};
Symbol sym_lbrace = {.sym = "{", .sym_len = 1, .match = reChar_match};
Symbol sym_abc = {.sym = "[abc]", .sym_len = 5, .match = reCharClass_match};
Symbol sym_not_abc = {.sym = "[^abc]", .sym_len = 6, .match = reCharClass_inv_match};
Symbol sym_range_ac = {.sym = "[a-c]", .sym_len = 5, .match = reCharClass_match};

// don't actually need to fill out the symbols
DFA * TEST_REGEX_DFA[] = {
    // "a"
    &(DFA){.nstates = 2, //.nsymbols = 1,
        .regex_s = "a", .regex_len = 1,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "ab"
    &(DFA){.nstates = 3, //.nsymbols = 2, 
        .regex_s = "ab", .regex_len = 2,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "a|b"
    &(DFA){.nstates = 3, //.nsymbols = 2, 
        .regex_s = "a|b", .regex_len = 3,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = &(DFATransition) {
                        .final_state = 1, 
                        .next = NULL, 
                        .sym = &sym_a
                    }, 
                    .sym = &sym_b
                }
            },
            {
                .trans = NULL,
                .accepting = true
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "ab+" // NOTE: this DFA is reducible. The [3] DFAState is essentially identical to [2]. When I am able to reduce states, this should be fixed
    &(DFA){.nstates = 4, //.nsymbols = 2, 
        .regex_s = "ab+", .regex_len = 3,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
        }[0]
    },
    // "ab\\+"
    &(DFA){.nstates = 4, //.nsymbols = 3, 
        .regex_s = "ab\\+", .regex_len = 4,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_plus
                }
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "ab{,}" // NOTE: this DFA is reducible. The [2] DFAState is essentially identical to [1]. When I am able to reduce states, this should be fixed
    &(DFA){.nstates = 3, //.nsymbols = 2, 
        .regex_s = "ab{,}", .regex_len = 5,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
        }[0]
    },
    // "ab{,0}" // NOTE: this DFA is reducible. The [2] DFAState is essentially identical to [1]. When I am able to reduce states, this should be fixed
    &(DFA){.nstates = 3, //.nsymbols = 2, 
        .regex_s = "ab{,0}", .regex_len = 6,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
        }[0]
    },
    // "ab{0,0}" // NOTE: this DFA is reducible. The [2] DFAState is essentially identical to [1]. When I am able to reduce states, this should be fixed
    &(DFA){.nstates = 3, //.nsymbols = 2, 
        .regex_s = "ab{0,0}", .regex_len = 7,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
        }[0]
    },
    // "ab{,1}"
    &(DFA){.nstates = 3, //.nsymbols = 2, 
        .regex_s = "ab{,1}", .regex_len = 6,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "ab{0,1}"
    &(DFA){.nstates = 3, //.nsymbols = 2, 
        .regex_s = "ab{0,1}", .regex_len = 7,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "ab?"
    &(DFA){.nstates = 3, //.nsymbols = 2, 
        .regex_s = "ab{0,1}", .regex_len = 7,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "ab{1,2}"
    &(DFA){.nstates = 4, //.nsymbols = 2, 
        .regex_s = "ab{1,2}", .regex_len = 7,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "ab{2}"
    &(DFA){.nstates = 4, //.nsymbols = 2, 
        .regex_s = "ab{2}", .regex_len = 5,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "ab{2,5}"
    &(DFA){.nstates = 7, //.nsymbols = 2, 
        .regex_s = "ab{2,5}", .regex_len = 7,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 4, 
                    .next = NULL, 
                    .sym = &sym_b
                },
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 5, 
                    .next = NULL, 
                    .sym = &sym_b
                },
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 6, 
                    .next = NULL, 
                    .sym = &sym_b
                },
                .accepting = true
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "ab\\{"
    &(DFA){.nstates = 4, //.nsymbols = 3, 
        .regex_s = "ab\\+", .regex_len = 4,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_lbrace
                }
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "a(bc){,}" // NOTE: this DFA is reducible. The [2] DFAState is essentially identical to [1]. When I am able to reduce states, this should be fixed
    &(DFA){.nstates = 4, //.nsymbols = 3, 
        .regex_s = "a(bc){,}", .regex_len = 8,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
        }[0]
    },
    // "a(bc){,0}" // NOTE: this DFA is reducible. The [2] DFAState is essentially identical to [1]. When I am able to reduce states, this should be fixed
    &(DFA){.nstates = 4, //.nsymbols = 3, 
        .regex_s = "a(bc){,0}", .regex_len = 9,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
        }[0]
    },
    // "a(bc){0,0}" // NOTE: this DFA is reducible. The [2] DFAState is essentially identical to [1]. When I am able to reduce states, this should be fixed
    &(DFA){.nstates = 4, //.nsymbols = 3, 
        .regex_s = "a(bc){0,0}", .regex_len = 10,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
        }[0]
    },
    // "a(bc){,1}"
    &(DFA){.nstates = 4, //.nsymbols = 3, 
        .regex_s = "a(bc){,1}", .regex_len = 9,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "a(bc){0,1}"
    &(DFA){.nstates = 4, //.nsymbols = 3, 
        .regex_s = "a(bc){0,1}", .regex_len = 10,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "a(bc){1,2}"
    &(DFA){.nstates = 6, //.nsymbols = 3, 
        .regex_s = "a(bc){1,2}", .regex_len = 10,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 4, 
                    .next = NULL, 
                    .sym = &sym_b
                },
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 5, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "a(bc){2}"
    &(DFA){.nstates = 6, //.nsymbols = 3, 
        .regex_s = "a(bc){2}", .regex_len = 8,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 4, 
                    .next = NULL, 
                    .sym = &sym_b
                },
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 5, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "a(bc){2,5}"
    &(DFA){.nstates = 12, //.nsymbols = 3, 
        .regex_s = "a(bc){2,5}", .regex_len = 10,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 4, 
                    .next = NULL, 
                    .sym = &sym_b
                },
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 5, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 6, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 7, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 8, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 9, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 10, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 11, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = NULL,
                .accepting = true
            },
        }[0]
    },
    // "a(bc)\\{"
    &(DFA){.nstates = 5, //.nsymbols = 4, 
        .regex_s = "ab\\{", .regex_len = 4,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 4, 
                    .next = NULL, 
                    .sym = &sym_lbrace
                }
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "a(bc)\\+"
    &(DFA){.nstates = 5, //.nsymbols = 4, 
        .regex_s = "ab\\+", .regex_len = 4,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 4, 
                    .next = NULL, 
                    .sym = &sym_plus
                }
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "a(bc)+"
    &(DFA){.nstates = 6, //.nsymbols = 3, 
        .regex_s = "ab+", .regex_len = 3,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_a
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .next = NULL, 
                    .sym = &sym_b
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 4, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 5, 
                    .next = NULL, 
                    .sym = &sym_c
                }, 
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 4, 
                    .next = NULL, 
                    .sym = &sym_b
                }, 
                .accepting = true
            },
        }[0]
    },
    // "[abc]"
    &(DFA){.nstates = 2, //.nsymbols = 1, 
        .regex_s = "[abc]", .regex_len = 5,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_abc
                }
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "[^abc]"
    &(DFA){.nstates = 2, //.nsymbols = 1, 
        .regex_s = "[^abc]", .regex_len = 6,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_not_abc
                }
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "[a-c]"
    &(DFA){.nstates = 2, //.nsymbols = 1, 
        .regex_s = "[a-c]", .regex_len = 5,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .next = NULL, 
                    .sym = &sym_range_ac
                }
            },
            {
                .trans = NULL,
                .accepting = true
            }
        }[0]
    },
    // "(a|b)*abb"
    &(DFA){.nstates = 5, //.nsymbols = 2, 
        .regex_s = "(a|b)*abb", .regex_len = 9,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .sym = &sym_b,
                    .next = &(DFATransition) {
                            .final_state = 1, 
                            .sym = &sym_a,
                            .next = NULL, 
                        }, 
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .sym = &sym_b,
                    .next = &(DFATransition) {
                            .final_state = 1, 
                            .sym = &sym_a,
                            .next = NULL, 
                        }, 
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .sym = &sym_b,
                    .next = &(DFATransition) {
                            .final_state = 1, 
                            .sym = &sym_a,
                            .next = NULL, 
                        }, 
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 4, 
                    .sym = &sym_b,
                    .next = &(DFATransition) {
                            .final_state = 1, 
                            .sym = &sym_a,
                            .next = NULL, 
                        }, 
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .sym = &sym_b,
                    .next = &(DFATransition) {
                            .final_state = 1, 
                            .sym = &sym_a,
                            .next = NULL, 
                        }, 
                },
                .accepting = true
            }
        }[0]
    },
    // "abb(a|b)*"
    &(DFA){.nstates = 6, //.nsymbols = 2, 
        .regex_s = "abb(a|b)*", .regex_len = 9,
        .states = &(DFAState[]){
            {
                .trans = &(DFATransition) {
                    .final_state = 1, 
                    .sym = &sym_a,
                    .next = NULL
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 2, 
                    .sym = &sym_b,
                    .next = NULL
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 3, 
                    .sym = &sym_b,
                    .next = NULL
                }
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 5, 
                    .sym = &sym_b,
                    .next = &(DFATransition) {
                            .final_state = 4, 
                            .sym = &sym_a,
                            .next = NULL, 
                        }, 
                },
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 5, 
                    .sym = &sym_b,
                    .next = &(DFATransition) {
                            .final_state = 4, 
                            .sym = &sym_a,
                            .next = NULL, 
                        }, 
                },
                .accepting = true
            },
            {
                .trans = &(DFATransition) {
                    .final_state = 5, 
                    .sym = &sym_b,
                    .next = &(DFATransition) {
                            .final_state = 4, 
                            .sym = &sym_a,
                            .next = NULL, 
                        }, 
                },
                .accepting = true
            }
        }[0]
    },
    NULL
};

TestString * TEST_REGEX_STRINGS[] = {
    // "a"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = NULL},
    }[0],
    // "b"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 2, .str = "ab"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "aba", .match = {.len = 2, .str = "ab"}},
        {.cstr = "abab", .match = {.len = 2, .str = "ab"}},
        {.cstr = NULL},
    }[0],
    // "a|b"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 1, .str = "b"}},
        {.cstr = "c", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = "ba", .match = {.len = 1, .str = "b"}},
        {.cstr = NULL},
    }[0],
    // "ab+"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 2, .str = "ab"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "aba", .match = {.len = 2, .str = "ab"}},
        {.cstr = "abb", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abba", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbb", .match = {.len = 4, .str = "abbb"}},
        {.cstr = NULL},
    }[0],
    // ab\\+
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "aba", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab+", .match = {.len = 3, .str = "ab+"}},
        {.cstr = "ab+a", .match = {.len = 3, .str = "ab+"}},
        {.cstr = NULL},
    }[0],
    // "ab{,}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 2, .str = "ab"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "aba", .match = {.len = 2, .str = "ab"}},
        {.cstr = "abb", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abba", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbb", .match = {.len = 4, .str = "abbb"}},
        {.cstr = NULL},
    }[0],
    // "ab{,0}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 2, .str = "ab"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "aba", .match = {.len = 2, .str = "ab"}},
        {.cstr = "abb", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abba", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbb", .match = {.len = 4, .str = "abbb"}},
        {.cstr = NULL},
    }[0],
    // "ab{0,0}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 2, .str = "ab"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "aba", .match = {.len = 2, .str = "ab"}},
        {.cstr = "abb", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abba", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbb", .match = {.len = 4, .str = "abbb"}},
        {.cstr = NULL},
    }[0],
    // "ab{,1}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "aa", .match = {.len = 1, .str = "a"}},
        {.cstr = "ab", .match = {.len = 2, .str = "ab"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 2, .str = "ab"}},
        {.cstr = NULL},
    }[0],
    // "ab{0,1}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "aa", .match = {.len = 1, .str = "a"}},
        {.cstr = "ab", .match = {.len = 2, .str = "ab"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 2, .str = "ab"}},
        {.cstr = NULL},
    }[0],
    // "ab?"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "aa", .match = {.len = 1, .str = "a"}},
        {.cstr = "ab", .match = {.len = 2, .str = "ab"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 2, .str = "ab"}},
        {.cstr = NULL},
    }[0],
    // "ab{1,2}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "aa", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 2, .str = "ab"}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abba", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbb", .match = {.len = 3, .str = "abb"}},
        {.cstr = NULL},
    }[0],
    // "ab{2}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "aa", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abba", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbb", .match = {.len = 3, .str = "abb"}},
        {.cstr = NULL},
    }[0],
    // "ab{2,5}"
    &(TestString[]){
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abba", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbb", .match = {.len = 4, .str = "abbb"}},
        {.cstr = "abbba", .match = {.len = 4, .str = "abbb"}},
        {.cstr = "abbbb", .match = {.len = 5, .str = "abbbb"}},
        {.cstr = "abbbba", .match = {.len = 5, .str = "abbbb"}},
        {.cstr = "abbbbb", .match = {.len = 6, .str = "abbbbb"}},
        {.cstr = "abbbbba", .match = {.len = 6, .str = "abbbbb"}},
        {.cstr = "abbbbbb", .match = {.len = 6, .str = "abbbbb"}},
        {.cstr = NULL},
    }[0],
    // ab\\{
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "aba", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab{", .match = {.len = 3, .str = "ab{"}},
        {.cstr = "ab{a", .match = {.len = 3, .str = "ab{"}},
        {.cstr = NULL},
    }[0],
    // a(bc){,}
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = "abc", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcb", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcbc", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = NULL},
    }[0],
    // a(bc){,0}
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = "abc", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcb", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcbc", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = NULL},
    }[0],
    // a(bc){0,0}
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = "abc", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcb", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcbc", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = NULL},
    }[0],
    // "a(bc){,1}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = "abc", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcb", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcbc", .match = {.len = 3, .str = "abc"}},
        {.cstr = NULL},
    }[0],
    // "a(bc){0,1}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = "abc", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcb", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcbc", .match = {.len = 3, .str = "abc"}},
        {.cstr = NULL},
    }[0],
    // "a(bc){1,2}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "abc", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcb", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcbc", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = "abcbcb", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = NULL},
    }[0],
    // "a(bc){2}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "abc", .match = {.len = 0, .str = NULL}},
        {.cstr = "abcb", .match = {.len = 0, .str = NULL}},
        {.cstr = "abcbc", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = "abcbcb", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = NULL},
    }[0],
    // "a(bc){2,5}"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "abc", .match = {.len = 0, .str = NULL}},
        {.cstr = "abcb", .match = {.len = 0, .str = NULL}},
        {.cstr = "abcbc", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = "abcbcb", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = "abcbcbc", .match = {.len = 7, .str = "abcbcbc"}},
        {.cstr = "abcbcbcb", .match = {.len = 7, .str = "abcbcbc"}},
        {.cstr = "abcbcbcbc", .match = {.len = 9, .str = "abcbcbcbc"}},
        {.cstr = "abcbcbcbcb", .match = {.len = 9, .str = "abcbcbcbc"}},
        {.cstr = "abcbcbcbcbc", .match = {.len = 11, .str = "abcbcbcbcbc"}},
        {.cstr = "abcbcbcbcbcb", .match = {.len = 11, .str = "abcbcbcbcbc"}},
        {.cstr = NULL},
    }[0],
    // "a(bc)\\{"
    // ab\\{
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "aba", .match = {.len = 0, .str = NULL}},
        {.cstr = "abc", .match = {.len = 0, .str = NULL}},
        {.cstr = "abca", .match = {.len = 0, .str = NULL}},
        {.cstr = "abc{", .match = {.len = 4, .str = "abc{"}},
        {.cstr = "abc{a", .match = {.len = 4, .str = "abc{"}},
        {.cstr = NULL},
    }[0],
    // "a(bc)\\+"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "ba", .match = {.len = 0, .str = NULL}},
        {.cstr = "aba", .match = {.len = 0, .str = NULL}},
        {.cstr = "abc", .match = {.len = 0, .str = NULL}},
        {.cstr = "abca", .match = {.len = 0, .str = NULL}},
        {.cstr = "abc+", .match = {.len = 4, .str = "abc+"}},
        {.cstr = "abc+a", .match = {.len = 4, .str = "abc+"}},
        {.cstr = NULL},
    }[0],
    // "a(bc)+"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "abc", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcb", .match = {.len = 3, .str = "abc"}},
        {.cstr = "abcbc", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = "abcbcb", .match = {.len = 5, .str = "abcbc"}},
        {.cstr = NULL},
    }[0],
    // "[abc]"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 1, .str = "b"}},
        {.cstr = "c", .match = {.len = 1, .str = "c"}},
        {.cstr = "d", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = NULL},
    }[0],
    // "[^abc]"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "b", .match = {.len = 0, .str = NULL}},
        {.cstr = "c", .match = {.len = 0, .str = NULL}},
        {.cstr = "d", .match = {.len = 1, .str = "d"}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = NULL},
    }[0],
    // "[a-c]"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "b", .match = {.len = 1, .str = "b"}},
        {.cstr = "c", .match = {.len = 1, .str = "c"}},
        {.cstr = "d", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = NULL},
    }[0],
    // "(a|b)*abb"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abba", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbab", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbabb", .match = {.len = 6, .str = "abbabb"}},
        {.cstr = "abaabb", .match = {.len = 6, .str = "abaabb"}},
        {.cstr = "ababb", .match = {.len = 5, .str = "ababb"}},
        {.cstr = "aaabb", .match = {.len = 5, .str = "aaabb"}},
        {.cstr = "aaaabb", .match = {.len = 6, .str = "aaaabb"}},
        {.cstr = "aaaabba", .match = {.len = 6, .str = "aaaabb"}},
        {.cstr = NULL},
    }[0],
    // "abb(a|b)*"
    &(TestString[]){
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abbc", .match = {.len = 3, .str = "abb"}},
        {.cstr = "abba", .match = {.len = 4, .str = "abba"}},
        {.cstr = "abbab", .match = {.len = 5, .str = "abbab"}},
        {.cstr = "abbabb", .match = {.len = 6, .str = "abbabb"}},
        {.cstr = "abaabb", .match = {.len = 0, .str = NULL}},
        {.cstr = NULL},
    }[0],
    // "[a-zA-Z_][a-zA-Z_0-9]*",
    &(TestString[]){
        {.cstr = "_", .match = {.len = 1, .str = "_"}},
        {.cstr = "__", .match = {.len = 2, .str = "__"}},
        {.cstr = "0", .match = {.len = 0, .str = NULL}},
        {.cstr = "_0", .match = {.len = 2, .str = "_0"}},
        {.cstr = "IAmTheVeryModelOfAModernMajorGeneral", .match = {.len = 36, .str = "IAmTheVeryModelOfAModernMajorGeneral"}},
        {.cstr = "_!", .match = {.len = 1, .str = "_"}},
        {.cstr = "__.", .match = {.len = 2, .str = "__"}},
        {.cstr = "_0#", .match = {.len = 2, .str = "_0"}},
        {.cstr = NULL},
    }[0],
    // "[ \t\r\n\v\f]+"
    &(TestString[]){
        {.cstr = " ", .match = {.len = 1, .str = " "}},
        {.cstr = "\t", .match = {.len = 1, .str = "\t"}},
        {.cstr = "\r", .match = {.len = 1, .str = "\r"}},
        {.cstr = "\n", .match = {.len = 1, .str = "\n"}},
        {.cstr = "\v", .match = {.len = 1, .str = "\v"}},
        {.cstr = "\f", .match = {.len = 1, .str = "\f"}},
        {.cstr = " a", .match = {.len = 1, .str = " "}},
        {.cstr = "a ", .match = {.len = 0, .str = NULL}},
        {.cstr = "  ", .match = {.len = 2, .str = "  "}},
        {.cstr = NULL},
    }[0],
    // "//[^\n]*\n"
    &(TestString[]){
        {.cstr = "//\n", .match = {.len = 3, .str = "//\n"}},
        {.cstr = "// \n", .match = {.len = 4, .str = "// \n"}},
        {.cstr = "// This is a comment \n", .match = {.len = 22, .str = "// This is a comment \n"}},
        {.cstr = "/ /This is not a comment\n", .match = {.len = 0, .str = NULL}},
        {.cstr = " //This is also not a comment\n", .match = {.len = 0, .str = NULL}},
        {.cstr = NULL},
    }[0],
    // "[^/]",
    &(TestString[]){
        {.cstr = "/", .match = {.len = 0, .str = NULL}},
        {.cstr = "*", .match = {.len = 1, .str = "*"}},
        {.cstr = NULL},
    }[0],
    // "\\*",
    &(TestString[]){
        {.cstr = "/", .match = {.len = 0, .str = NULL}},
        {.cstr = "*", .match = {.len = 1, .str = "*"}},
        {.cstr = NULL},
    }[0],
    // "\\*[^/]",
    &(TestString[]){
        {.cstr = "*", .match = {.len = 0, .str = NULL}},
        {.cstr = "**", .match = {.len = 2, .str = "**"}},
        {.cstr = NULL},
    }[0],
    // "[^*]"
    &(TestString[]){
        {.cstr = "/", .match = {.len = 1, .str = "/"}},
        {.cstr = "*", .match = {.len = 0, .str = NULL}},
        {.cstr = NULL},
    }[0],
    // "(\\*[^/])|[^*]"
    &(TestString[]){
        {.cstr = "*", .match = {.len = 0, .str = NULL}},
        {.cstr = "**", .match = {.len = 2, .str = "**"}},
        {.cstr = "/", .match = {.len = 1, .str = "/"}},
        {.cstr = NULL},
    }[0],
    // "a(?=b)"
    &(TestString[]){
        {.cstr = "ab", .match = {.len = 1, .str = "a"}},
        {.cstr = "aa", .match = {.len = 0, .str = NULL}},
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "abb", .match = {.len = 1, .str = "a"}},
        {.cstr = NULL},
    }[0],
    // "a(?=\\))"
    &(TestString[]){
        {.cstr = "a)", .match = {.len = 1, .str = "a"}},
        {.cstr = "aa", .match = {.len = 0, .str = NULL}},
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = "a))", .match = {.len = 1, .str = "a"}},
        {.cstr = NULL},
    }[0],
    // "a(?!b)"
    &(TestString[]){
        {.cstr = "ab", .match = {.len = 0, .str = NULL}},
        {.cstr = "aa", .match = {.len = 1, .str = "a"}},
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = "abb", .match = {.len = 0, .str = NULL}},
        {.cstr = NULL},
    }[0],
    // "/\\*((\\*(?!/))|[^*])*\\*/)"
    &(TestString[]){
        {.cstr = "/**/", .match = {.len = 4, .str = "/**/"}},
        {.cstr = "/** /", .match = {.len = 0, .str = NULL}},
        {.cstr = "/ **/", .match = {.len = 0, .str = NULL}},
        {.cstr = " /**/", .match = {.len = 0, .str = NULL}},
        {.cstr = "/* */", .match = {.len = 5, .str = "/* */"}},
        {.cstr = "/***/", .match = {.len = 5, .str = "/***/"}},
        {.cstr = "/*/*/", .match = {.len = 5, .str = "/*/*/"}},
        // should only capture the first and not the second
        {.cstr = "/**/ not a comment /**/", .match = {.len = 4, .str = "/**/"}},
        {.cstr = NULL},
    }[0],
    // "[\\\\\\^$.[|()*?]"
    &(TestString[]){
        {.cstr = "\\", .match = {.len = 1, .str = "\\"}},
        {.cstr = "^", .match = {.len = 1, .str = "^"}},
        {.cstr = "$", .match = {.len = 1, .str = "$"}},
        {.cstr = ".", .match = {.len = 1, .str = "."}},
        {.cstr = "[", .match = {.len = 1, .str = "["}},
        {.cstr = "|", .match = {.len = 1, .str = "|"}},
        {.cstr = "(", .match = {.len = 1, .str = "("}},
        {.cstr = ")", .match = {.len = 1, .str = ")"}},
        {.cstr = "*", .match = {.len = 1, .str = "*"}},
        {.cstr = "?", .match = {.len = 1, .str = "?"}},
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = NULL},
    }[0],
    // "[^\\\\\\^$.[|()*?]"
    &(TestString[]){
        {.cstr = "\\", .match = {.len = 0, .str = NULL}},
        {.cstr = "^", .match = {.len = 0, .str = NULL}},
        {.cstr = "$", .match = {.len = 0, .str = NULL}},
        {.cstr = ".", .match = {.len = 0, .str = NULL}},
        {.cstr = "[", .match = {.len = 0, .str = NULL}},
        {.cstr = "|", .match = {.len = 0, .str = NULL}},
        {.cstr = "(", .match = {.len = 0, .str = NULL}},
        {.cstr = ")", .match = {.len = 0, .str = NULL}},
        {.cstr = "*", .match = {.len = 0, .str = NULL}},
        {.cstr = "?", .match = {.len = 0, .str = NULL}},
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = NULL},
    }[0],
    //"[\\\\\\-\\]\\^]",
    &(TestString[]){
        {.cstr = "\\", .match = {.len = 1, .str = "\\"}},
        {.cstr = "^", .match = {.len = 1, .str = "^"}},
        {.cstr = "-", .match = {.len = 1, .str = "-"}},
        {.cstr = "]", .match = {.len = 1, .str = "]"}},
        {.cstr = "$", .match = {.len = 0, .str = NULL}},
        {.cstr = ".", .match = {.len = 0, .str = NULL}},
        {.cstr = "[", .match = {.len = 0, .str = NULL}},
        {.cstr = "|", .match = {.len = 0, .str = NULL}},
        {.cstr = "(", .match = {.len = 0, .str = NULL}},
        {.cstr = ")", .match = {.len = 0, .str = NULL}},
        {.cstr = "*", .match = {.len = 0, .str = NULL}},
        {.cstr = "?", .match = {.len = 0, .str = NULL}},
        {.cstr = "a", .match = {.len = 0, .str = NULL}},
        {.cstr = NULL},
    }[0],
    //"[^\\\\\\-\\]\\^]",
    &(TestString[]){
        {.cstr = "\\", .match = {.len = 0, .str = NULL}},
        {.cstr = "^", .match = {.len = 0, .str = NULL}},
        {.cstr = "-", .match = {.len = 0, .str = NULL}},
        {.cstr = "]", .match = {.len = 0, .str = NULL}},
        {.cstr = "$", .match = {.len = 1, .str = "$"}},
        {.cstr = ".", .match = {.len = 1, .str = "."}},
        {.cstr = "[", .match = {.len = 1, .str = "["}},
        {.cstr = "|", .match = {.len = 1, .str = "|"}},
        {.cstr = "(", .match = {.len = 1, .str = "("}},
        {.cstr = ")", .match = {.len = 1, .str = ")"}},
        {.cstr = "*", .match = {.len = 1, .str = "*"}},
        {.cstr = "?", .match = {.len = 1, .str = "?"}},
        {.cstr = "a", .match = {.len = 1, .str = "a"}},
        {.cstr = NULL},
    }[0],
    (TestString *)NULL
};

int test_preprocess(void) {
    int nerrors = 0;

    NFA nfa;
    NFA_init(&nfa);
    RegexBuilder reb;
    RegexBuilder_init(&reb, &nfa);

    size_t i = 0;
    while (TEST_REGEX[i]) {
        size_t len = strlen(TEST_REGEX[i]);
        RegexBuilder_preprocess(&reb, TEST_REGEX[i], len);
        size_t res_len = strlen(TEST_REGEX_PP[i]);
        int nerrors_i = CHECK(res_len == nfa.regex_len_pp, "preprocessed regex does not match length %.*s. expected: %zu, found: %zu\n", (int)res_len, TEST_REGEX_PP[i], res_len, nfa.regex_len_pp);
        //if (!nerrors_i) {
            nerrors += nerrors_i + CHECK(!strncmp(nfa.regex_s_pp, TEST_REGEX_PP[i], len), "preprocessed regex does not match. expected: (%zu)%.*s, found: (%zu)%.*s\n", res_len, (int)res_len, TEST_REGEX_PP[i], (int)nfa.regex_len_pp, (int)nfa.regex_len_pp, nfa.regex_s_pp);
        //}
        i++;
    }

    NFA_dest(&nfa);
    RegexBuilder_dest(&reb);
    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }
    return nerrors;
}

int test_NFA(void) {
    int nerrors = 0;

    NFA nfa;
    RegexBuilder reb;
    size_t j = 0;
    size_t i = 0;
    while (TEST_REGEX[i] && TEST_REGEX_NFA[i]) {
        RegexBuilder_init(&reb, &nfa);
        NFA_init(&nfa);
        if (!strncmp(TEST_REGEX[i], "(a|b)*abb", strlen(TEST_REGEX[i]))) {
            j = 1;
        }
        size_t regex_len = strlen(TEST_REGEX[i]);
        RegexBuilder_build_NFA(&reb, TEST_REGEX[i], regex_len);
        nerrors += check_NFA(&nfa, TEST_REGEX_NFA[i]);
        i++;
        NFA_dest(&nfa);
        RegexBuilder_dest(&reb);
    }

    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }
    return nerrors;
}

int test_DFA(void) {
    int nerrors = 0;

    DFA dfa;
    RegexBuilder reb;
    size_t i = 0;
    while (TEST_REGEX[i] && TEST_REGEX_DFA[i]) {
        NFA nfa; // dummy, not used. Need to fix API for RegexBuilder_build. probably just replace with a DFA_init
        RegexBuilder_init(&reb, &nfa);
        RegexBuilder_build(&reb, TEST_REGEX[i], strlen(TEST_REGEX[i]), &dfa);
        if (check_DFA(&dfa, TEST_REGEX_DFA[i])) {
            printf("errors on DFA with regex: %s\n", TEST_REGEX[i]);
            nerrors++;
        }
        RegexBuilder_dest(&reb);
        DFA_dest(&dfa);
        i++;
    }
    
    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }
    return nerrors;
}

int test_regex(void) {
    int nerrors = 0;

    size_t i = 0;
    while (TEST_REGEX[i] && TEST_REGEX_STRINGS[i]) {
        size_t j = 0;
        struct avramT3 av;
        size_t len = strlen(TEST_REGEX[i]);
        nerrors += CHECK(!are_compile_pattern(TEST_REGEX[i], len, &av, 0), "failed to compile regex pattern: %s\n", TEST_REGEX[i]);
        
        //if (!strcmp(TEST_REGEX[i], "/\\*((\\*[^/])|[^*])*\\*/)")) {
        //    DFA_print((DFA *)&av);
        //}
        
        while (TEST_REGEX_STRINGS[i][j].cstr) {
            nerrors += check_regex(&av, TEST_REGEX_STRINGS[i] + j);
            // need to reset av for next regex
            are_reset(&av);
            j++;
        }
        are_free(&av); // clear buffer memory and DFA
        i++;
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
    test_DFA();
    test_regex();

    return 0;
}

