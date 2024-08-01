#include <string.h>

// tokenizer
#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/parser_gen.h>

#include "test_utils.h"
#include "test_parser.h"

enum rules {
    LETTER,
    LETTER_TOKEN,
    DIGIT,
    ALPHANUMERIC,
    ALPHANUMERIC_TOKEN,
    HEXADECIMAL_DIGIT,
    HEXADECIMAL_TOKEN,
    WHITESPACE_RE,
    WHITESPACE,
    HEX_ALPHANUM_OR_WS,
    TEST_PARSER_TOKEN,
    HEX_WORD,
    ALPHA_WORD,
    WORD,
    WORDS,
    DOC
};

LITERALRULE(letter,LETTER,
	"[a-zA-Z]");

PRODUCTION(letter_token, LETTER_TOKEN,
	(Rule *)&letter,
	token_action);

LITERALRULE(digit, DIGIT,
	"[0-9]");

LITERALRULE(hexadecimal_digit, HEXADECIMAL_DIGIT,
    "[0-9a-fA-F]{2}");

PRODUCTION(hexadecimal_token, HEXADECIMAL_TOKEN,
	(Rule *)&hexadecimal_digit,
	token_action);

CHOICERULE(alphanumeric, ALPHANUMERIC, 
    (Rule *)&letter,
    (Rule *)&digit);

PRODUCTION(alphanumeric_token, ALPHANUMERIC_TOKEN,
	(Rule *)&alphanumeric,
	token_action);

LITERALRULE(whitespace_re, WHITESPACE_RE,
	"([ \t\r\n\v\f]+|(//[^\n]*\n)|(/\\*((\\*[^/])|[^*])*\\*/))+");

PRODUCTION(whitespace, WHITESPACE,
	(Rule *)&whitespace_re,
	skip_token);

CHOICERULE(hex_alphanum_or_ws, HEX_ALPHANUM_OR_WS,
    (Rule *)&hexadecimal_digit,
    (Rule *)&alphanumeric,
    (Rule *)&whitespace);

PRODUCTION(test_parser_token, TEST_PARSER_TOKEN,
	(Rule *)&hex_alphanum_or_ws,
	token_action);

SEQUENCERULE(hex_word, HEX_WORD,
    (Rule *)&hexadecimal_digit,
    (Rule *)&hexadecimal_digit);

SEQUENCERULE(alpha_word, ALPHA_WORD,
    (Rule *)&alphanumeric,
    (Rule *)&alphanumeric);

CHOICERULE(word, WORD,
    (Rule *)&hex_word,
    (Rule *)&alpha_word,
    (Rule *)&hexadecimal_digit,
    (Rule *)&alphanumeric);

REPEATRULE(words, WORDS,
    (Rule *)&word,
    1,
    0);

PRODUCTION(doc, DOC,
    (Rule *)&words);

void test_parser_cleanup(void) {
    test_parser_token._class->dest(&test_parser_token);
    hex_alphanum_or_ws._class->dest(&hex_alphanum_or_ws);
    letter._class->dest(&letter);
    letter_token._class->dest(&letter_token);
    digit._class->dest(&digit);
    hexadecimal_digit._class->dest(&hexadecimal_digit);
    hexadecimal_token._class->dest(&hexadecimal_token);
    alphanumeric._class->dest(&alphanumeric);
    alphanumeric_token._class->dest(&alphanumeric_token);
    whitespace._class->dest(&whitespace);
    whitespace_re._class->dest(&whitespace_re);
    hex_word._class->dest(&hex_word);
    alpha_word._class->dest(&alpha_word);
}

int test_tokenizer_single_char(void) {
    int nerrors = 0;
    char const * string = "pneumonoultramicroscopicsilicovolcanoconiosis";
    size_t N = strlen(string);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&letter_token, NULL, LETTER_TOKEN+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    Token * cur = parser._class->tokenize(&parser, string, N);
    size_t ntokens = 0;
    while (cur && cur->length) {
        nerrors += CHECK(cur->length == 1, "failed to capture only one character. matched %zu\n", cur->length);
        nerrors += CHECK(string[ntokens] == cur->string[0], "token-character mismatch: expected %c, found %c\n", string[ntokens], cur->string[0]);
        ntokens++;
        cur = cur->next;
    }
    nerrors += CHECK(ntokens == N, "failed to find all the tokens in the target string: expected %zu, found %zu\n", N, ntokens);
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s!\n", __func__, nerrors ? "failed" : "passed");
    }

    return nerrors;
}

int test_tokenizer_alphanumeric(void) {
    int nerrors = 0;
    char const * string = "0abcde1fghij2klmno3pqrst4uvwxy5zABCD6EFGHI7JKLMN8OPQRS9TUVWXYZ";
    size_t N = strlen(string);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&alphanumeric_token, NULL, ALPHANUMERIC_TOKEN+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    Token * cur = parser._class->tokenize(&parser, string, N);
    size_t ntokens = 0;
    while (cur && cur->length) {
        nerrors += CHECK(cur->length == 1, "failed to capture only one character or digit. matched %zu\n", cur->length);
        nerrors += CHECK(string[ntokens] == cur->string[0], "token-character mismatch: expected %c, found %c\n", string[ntokens], cur->string[0]);
        ntokens++;
        cur = cur->next;
    }
    nerrors += CHECK(ntokens == N, "failed to find all the tokens in the target string: expected %zu, found %zu\n", N, ntokens);
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s!\n", __func__, nerrors ? "failed" : "passed");
    }

    return nerrors;
}

int test_tokenizer_hexadecimal(void) {
    int nerrors = 0;
    char const * string = "000102030405060708090a0B0c0D0e0F101112131415161718191A1b1C1d1E1f2030405060708090a0B0c0D0e0F0A1b1C1d1E1f1";
    size_t N = strlen(string);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&hexadecimal_token, NULL, HEXADECIMAL_TOKEN+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    Token * cur = parser._class->tokenize(&parser, string, N);
    size_t ntokens = 0;
    while (cur && cur->length) {
        nerrors += CHECK(cur->length == 2, "failed to capture hexadecimal digit pair. matched %zu\n", cur->length);
        nerrors += CHECK(string[ntokens] == cur->string[0], "token-character mismatch: expected %c, found %c\n", string[ntokens], cur->string[0]);
        ntokens++;
        nerrors += CHECK(string[ntokens] == cur->string[1], "token-character mismatch: expected %c, found %c\n", string[ntokens], cur->string[1]);
        ntokens++;
        cur = cur->next;
    }
    nerrors += CHECK(ntokens == N, "failed to find all the tokens in the target string: expected %zu, found %zu\n", N, ntokens);
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s!\n", __func__, nerrors ? "failed" : "passed");
    }

    return nerrors;
}

int test_tokenizer_test_parser_token(void) {
    int nerrors = 0;
    char const * string = "0a bcde1fgh i j 2 k lm no 3p qr s t4 uv wx y 5 z AB CD 6E FG HI7 J KL M N8 O P Q RS 9 T UV W XY Z";
    char * result_tokens[] = {"0a","bc","de","1f","g","h","i","j",
                        "2","k","l","m","n","o","3","p",
                        "q","r","s","t","4","u","v","w",
                        "x","y","5","z","AB","CD","6E","F",
                        "G","H","I","7","J","K","L","M",
                        "N","8","O","P","Q","R","S","9",
                        "T","U","V","W","X","Y","Z"};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&test_parser_token, NULL, TEST_PARSER_TOKEN+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    Token * cur = parser._class->tokenize(&parser, string, strlen(string));
    size_t ntokens = 0;
    while (cur && cur->length) {
        switch (strlen(result_tokens[ntokens])) {
            case 1: {
                nerrors += CHECK(cur->length == 1, "failed to capture alphanumeric digit in token %zu. matched length %zu\n", ntokens, cur->length);
                nerrors += CHECK(!strncmp(result_tokens[ntokens], cur->string, 1), "token-character mismatch: expected %s, found %.*s\n", string[ntokens], 1, cur->string);
                break;
            }
            case 2: {
                nerrors += CHECK(cur->length == 2, "failed to capture hexadecimal digit pair in token %zu. matched %zu\n", ntokens, cur->length);
                nerrors += CHECK(!strncmp(result_tokens[ntokens], cur->string, 2), "token-character mismatch: expected %s, found %.*s\n", string[ntokens], 2, cur->string);
                break;
            }
        }
        ntokens++;
        cur = cur->next;
    }
    nerrors += CHECK(ntokens == N, "failed to find all the tokens in the target string: expected %zu, found %zu\n", N, ntokens);
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s!\n", __func__, nerrors ? "failed" : "passed");
    }

    return nerrors;
}

int test_parser_doc(void) {
    int nerrors = 0;
    char const * string = "0a bcde1fgh i j 2 k lm no 3p qr s t4 uv wx y 5 z AB CD 6E FG HI7 J KL M N8 O P Q RS 9 T UV W XY Z a";
    char * result_tokens[] = {"0a","bc","de","1f","g","h","i","j",
                        "2","k","l","m","n","o","3","p",
                        "q","r","s","t","4","u","v","w",
                        "x","y","5","z","AB","CD","6E","F",
                        "G","H","I","7","J","K","L","M",
                        "N","8","O","P","Q","R","S","9",
                        "T","U","V","W","X","Y","Z", "a"};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&test_parser_token, (Rule *)&doc, DOC+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    parser._class->parse(&parser, string, strlen(string));
    FILE * ast_out = fopen("test_parser_doc_ast.txt", "w");
    Parser_print_tokens(&parser, ast_out);
    Parser_print_ast(&parser, ast_out);
    fclose(ast_out);
    
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s!\n", __func__, nerrors ? "failed" : "passed");
    }

    return nerrors;
}