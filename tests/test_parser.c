#include <string.h>

// tokenizer
#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/parser_gen.h>

#include "test_utils.h"
#include "test_parser.h"
#include "test_peggy_utils.h"

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
    // should only have to destroy the LiteralRules to clean up regex allocations
    // if tests require additional LiteralRules or other Rules require cleanup...add them
    letter._class->dest(&letter);
    digit._class->dest(&digit);
    hexadecimal_digit._class->dest(&hexadecimal_digit);
    whitespace_re._class->dest(&whitespace_re);
}

int test_tokenizer_single_char(void) {
    int nerrors = 0;
    char const * string = "pneumonoultramicroscopicsilicovolcanoconiosis";
    size_t N = strlen(string);
    char const * result_tokens[] = {"p","n","e","u","m","o","n","o","u",
        "l","t","r","a","m","i","c","r","o",
        "s","c","o","p","i","c","s","i","l",
        "i","c","o","v","o","l","c","a","n",
        "o","c","o","n","i","o","s","i","s", NULL};

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&letter_token, NULL, LETTER_TOKEN+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    Token * cur, * end;
    size_t ntokens = parser._class->tokenize(&parser, string, N, &cur, &end);
    check_tokens(cur, ntokens, result_tokens, __FILE__, __func__, __LINE__);
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }

    return nerrors;
}

int test_tokenizer_alphanumeric(void) {
    int nerrors = 0;
    char const * string = "0abcde1fghij2klmno3pqrst4uvwxy5zABCD6EFGHI7JKLMN8OPQRS9TUVWXYZ";
    size_t N = strlen(string);
    char const * result_tokens[] = {"0","a","b","c","d","e","1","f","g","h",
        "i","j","2","k","l","m","n","o","3","p",
        "q","r","s","t","4","u","v","w","x","y",
        "5","z","A","B","C","D","6","E","F","G",
        "H","I","7","J","K","L","M","N","8","O",
        "P","Q","R","S","9","T","U","V","W","X",
        "Y","Z", NULL};

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&alphanumeric_token, NULL, ALPHANUMERIC_TOKEN+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    Token * cur, * end;
    size_t ntokens = parser._class->tokenize(&parser, string, N, &cur, &end);
    check_tokens(cur, ntokens, result_tokens, __FILE__, __func__, __LINE__);
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }

    return nerrors;
}

int test_tokenizer_hexadecimal(void) {
    int nerrors = 0;
    char const * string = "000102030405060708090a0B0c0D0e0F101112131415161718191A1b1C1d1E1f2030405060708090a0B0c0D0e0F0A1b1C1d1E1f1";
    size_t N = strlen(string);
    char const * result_tokens[] = {"00","01","02","03","04","05","06","07",
        "08","09","0a","0B","0c","0D","0e","0F",
        "10","11","12","13","14","15","16","17",
        "18","19","1A","1b","1C","1d","1E","1f",
        "20","30","40","50","60","70","80","90",
        "a0","B0","c0","D0","e0","F0","A1","b1",
        "C1","d1","E1","f1", NULL};

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&hexadecimal_token, NULL, HEXADECIMAL_TOKEN+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    Token * cur, * end;
    size_t ntokens = parser._class->tokenize(&parser, string, N, &cur, &end);
    check_tokens(cur, ntokens, result_tokens, __FILE__, __func__, __LINE__);
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }

    return nerrors;
}

int test_tokenizer_test_parser_token(void) {
    int nerrors = 0;
    char const * string = "0a bcde1fgh i j 2 k lm no 3p qr s t4 uv wx y 5 z AB CD 6E FG HI7 J KL M N8 O P Q RS 9 T UV W XY Z";
    size_t nstring = strlen(string);
    char const * result_tokens[] = {"0a","bc","de","1f","g","h","i","j",
                        "2","k","l","m","n","o","3","p",
                        "q","r","s","t","4","u","v","w",
                        "x","y","5","z","AB","CD","6E","F",
                        "G","H","I","7","J","K","L","M",
                        "N","8","O","P","Q","R","S","9",
                        "T","U","V","W","X","Y","Z", NULL};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]) - 1;

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&test_parser_token, NULL, TEST_PARSER_TOKEN+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    Token * cur, * end;
    size_t ntokens = parser._class->tokenize(&parser, string, nstring, &cur, &end);
    check_tokens(cur, ntokens, result_tokens, __FILE__, __func__, __LINE__);
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }

    return nerrors;
}

int test_parser_doc(void) {
    int nerrors = 0;
    char const * string = "0a bcde1fgh i j 2 k lm no 3p qr s t4 uv wx y 5 z AB CD 6E FG HI7 J KL M N8 O P Q RS 9 T UV W XY Z a";
    char const * result_tokens[] = {"0a","bc","de","1f","g","h","i","j",
                        "2","k","l","m","n","o","3","p",
                        "q","r","s","t","4","u","v","w",
                        "x","y","5","z","AB","CD","6E","F",
                        "G","H","I","7","J","K","L","M",
                        "N","8","O","P","Q","R","S","9",
                        "T","U","V","W","X","Y","Z", "a", NULL};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);
    TestASTNode * result_nodes = &TESTASTNODE(15, "0a", "a", 29, 
        &TESTASTNODE(11, "0a", "bc", 2, 
            &TESTASTNODE(5, "0a", "0a", 0, NULLNODE,
            &TESTASTNODE(5, "bc", "bc", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(11, "de", "1f", 2, 
            &TESTASTNODE(5, "de", "de", 0, NULLNODE,
            &TESTASTNODE(5, "1f", "1f", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "g", "h", 2, 
            &TESTASTNODE(0, "g", "g", 0, NULLNODE,
            &TESTASTNODE(0, "h", "h", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "i", "j", 2, 
            &TESTASTNODE(0, "i", "i", 0, NULLNODE,
            &TESTASTNODE(0, "j", "j", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "2", "k", 2, 
            &TESTASTNODE(2, "2", "2", 0, NULLNODE,
            &TESTASTNODE(0, "k", "k", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "l", "m", 2, 
            &TESTASTNODE(0, "l", "l", 0, NULLNODE,
            &TESTASTNODE(0, "m", "m", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "n", "o", 2, 
            &TESTASTNODE(0, "n", "n", 0, NULLNODE,
            &TESTASTNODE(0, "o", "o", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "3", "p", 2, 
            &TESTASTNODE(2, "3", "3", 0, NULLNODE,
            &TESTASTNODE(0, "p", "p", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "q", "r", 2, 
            &TESTASTNODE(0, "q", "q", 0, NULLNODE,
            &TESTASTNODE(0, "r", "r", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "s", "t", 2, 
            &TESTASTNODE(0, "s", "s", 0, NULLNODE,
            &TESTASTNODE(0, "t", "t", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "4", "u", 2, 
            &TESTASTNODE(2, "4", "4", 0, NULLNODE,
            &TESTASTNODE(0, "u", "u", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "v", "w", 2, 
            &TESTASTNODE(0, "v", "v", 0, NULLNODE,
            &TESTASTNODE(0, "w", "w", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "x", "y", 2, 
            &TESTASTNODE(0, "x", "x", 0, NULLNODE,
            &TESTASTNODE(0, "y", "y", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "5", "z", 2, 
            &TESTASTNODE(2, "5", "5", 0, NULLNODE,
            &TESTASTNODE(0, "z", "z", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(11, "AB", "CD", 2, 
            &TESTASTNODE(5, "AB", "AB", 0, NULLNODE,
            &TESTASTNODE(5, "CD", "CD", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(5, "6E", "6E", 0, // fix this
            NULLNODE,
        &TESTASTNODE(12, "F", "G", 2, 
            &TESTASTNODE(0, "F", "F", 0, NULLNODE,
            &TESTASTNODE(0, "G", "G", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "H", "I", 2, 
            &TESTASTNODE(0, "H", "H", 0, NULLNODE,
            &TESTASTNODE(0, "I", "I", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "7", "J", 2, 
            &TESTASTNODE(2, "7", "7", 0, NULLNODE,
            &TESTASTNODE(0, "J", "J", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "K", "L", 2, 
            &TESTASTNODE(0, "K", "K", 0, NULLNODE,
            &TESTASTNODE(0, "L", "L", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "M", "N", 2, 
            &TESTASTNODE(0, "M", "M", 0, NULLNODE,
            &TESTASTNODE(0, "N", "N", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "8", "O", 2, 
            &TESTASTNODE(2, "8", "8", 0, NULLNODE,
            &TESTASTNODE(0, "O", "O", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "P", "Q", 2, 
            &TESTASTNODE(0, "P", "P", 0, NULLNODE,
            &TESTASTNODE(0, "Q", "Q", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "R", "S", 2, 
            &TESTASTNODE(0, "R", "R", 0, NULLNODE,
            &TESTASTNODE(0, "S", "S", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "9", "T", 2, 
            &TESTASTNODE(2, "9", "9", 0, NULLNODE,
            &TESTASTNODE(0, "T", "T", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "U", "V", 2, 
            &TESTASTNODE(0, "U", "U", 0, NULLNODE,
            &TESTASTNODE(0, "V", "V", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "W", "X", 2, 
            &TESTASTNODE(0, "W", "W", 0, NULLNODE,
            &TESTASTNODE(0, "X", "X", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(12, "Y", "Z", 2, 
            &TESTASTNODE(0, "Y", "Y", 0, NULLNODE,
            &TESTASTNODE(0, "Z", "Z", 0, NULLNODE, 
            NULLNODE)),
        &TESTASTNODE(0, "a", "a", 0, 
            NULLNODE, 
        NULLNODE))))))))))))))))))))))))))))),
        NULLNODE);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&test_parser_token, (Rule *)&doc, DOC+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    size_t string_length = strlen(string);
    parser._class->parse(&parser, string, string_length);
    nerrors += check_tokens(parser.token_head->next, Parser_get_ntokens(&parser), result_tokens, __FILE__, __func__, __LINE__);
    nerrors += check_ASTNodes(parser.ast, result_nodes, __FILE__, __func__, __LINE__);
    /* // for print debugging
    FILE * ast_out = fopen("test_parser_doc_ast.txt", "w");
    Parser_print_tokens(&parser, ast_out);
    Parser_print_ast(&parser, ast_out);
    fclose(ast_out);
    */
    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s with %d errors!\n", __func__, nerrors ? "failed" : "passed", nerrors);
    }

    return nerrors;
}

