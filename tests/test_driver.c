#include <stdio.h>
#include <string.h>
#include "peg4c/astnode.h"
#include "test_parser.h"
#include "test_rules.h"
#include "test_utils.h"

int main(int narg, char ** args) {
	printf("sizeof(ASTNode) = %zu\n", sizeof(ASTNode));
    for (int i = 1; i < narg; i++) {
        if (!strcmp(args[i], "--verbose")) {
            verbose = 1;
        }
    }
    int nerrors = 0;

    nerrors += test_tokenizer_single_char();
    nerrors += test_tokenizer_alphanumeric();
    nerrors += test_tokenizer_hexadecimal();
    nerrors += test_tokenizer_test_parser_token();

    nerrors += test_parser_doc();
    test_parser_cleanup();


    nerrors += test_sequence();
    nerrors += test_repeat();
    nerrors += test_list();
    nerrors += test_lookahead();
    test_rule_cleanup();

    return nerrors;
}

