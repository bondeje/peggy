#include <string.h>
#include "test_parser.h"
#include "test_utils.h"

int main(int narg, char ** args) {
    for (int i = 1; i < narg; i++) {
        if (!strcmp(args[i], "--verbose")) {
            verbose = 1;
        }
    }
    int nerrors = 0;
    nerrors += test_tokenizer_single_char();
    nerrors += test_tokenizer_alphanumeric();
    nerrors += test_tokenizer_hexadecimal();
    //nerrors += test_tokenizer_hex_alphanum_or_ws();
    nerrors += test_tokenizer_test_parser_token();
    test_parser_cleanup();
    return nerrors;
}