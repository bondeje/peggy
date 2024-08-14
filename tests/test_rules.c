#include <string.h>

#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/parser_gen.h>

#include "test_utils.h"
#include "test_peggy_utils.h"
#include "test_rules.h"

enum rules {
    A,
    B,
    AB_LETTER,
    AB_LETTER_TOKEN,
    A2_SEQ,
    B2_SEQ,
    AB_SEQ,
    SEQ_PARSER_CHOICE,
    SEQ_PARSER_REP,
    SEQ_PARSER,
    A_REP_ANY,
    B_REP_ANY,
    A_REP_GT1,
    B_REP_GT1,
    A_REP_OPT,
    B_REP_OPT,
    REP_SEQ_BA,
    REP_SEQ_AA,
    REP_CHOICE,
    REP_REP,
    REP_PARSER,
    AB_LIST,
    BA_LIST,
    LIST_CHOICE,
    LIST_REP,
    LIST_PARSER,
    NLA_A,
    NLA_B,
    PLA_A,
    PLA_B,
    NLA_SEQ,
    PLA_SEQ,
    LA_CHOICE,
    LA_REP,
    LA_PARSER,
    LETTER_PARSER,
};

LITERALRULE(a, A,
	"a");

LITERALRULE(b, B,
	"b");

CHOICERULE(ab_letter, AB_LETTER,
    (Rule *)&a,
    (Rule *)&b);

PRODUCTION(ab_letter_token, AB_LETTER_TOKEN,
    (Rule *)&ab_letter,
    token_action);

SEQUENCERULE(a2_seq, A2_SEQ,
    (Rule *)&a,
    (Rule *)&a);

SEQUENCERULE(b2_seq, B2_SEQ,
    (Rule *)&b,
    (Rule *)&b);

SEQUENCERULE(ab_seq, AB_SEQ,
    (Rule *)&a,
    (Rule *)&b);

CHOICERULE(seq_parser_choice, SEQ_PARSER_CHOICE,
    (Rule *)&a2_seq,
    (Rule *)&b2_seq,
    (Rule *)&ab_seq);

REPEATRULE(seq_parser_rep, SEQ_PARSER_REP,
    (Rule *)&seq_parser_choice,
    1);

PRODUCTION(seq_parser, SEQ_PARSER,
    (Rule *)&seq_parser_rep);

REPEATRULE(a_rep_any, A_REP_ANY,
    (Rule *)&a);

REPEATRULE(b_rep_any, B_REP_ANY,
    (Rule *)&b);

REPEATRULE(a_rep_gt1, A_REP_GT1,
    (Rule *)&a,
    1);

REPEATRULE(b_rep_gt1, B_REP_GT1,
    (Rule *)&b,
    1);

REPEATRULE(a_rep_opt, A_REP_OPT,
    (Rule *)&a,
    0,
    1);

REPEATRULE(b_rep_opt, B_REP_OPT,
    (Rule *)&b,
    0,
    1);

SEQUENCERULE(rep_seq_ba, REP_SEQ_BA,
    (Rule *)&b_rep_gt1,
    (Rule *)&a_rep_any);

SEQUENCERULE(rep_seq_aa, REP_SEQ_AA,
    (Rule *)&a,
    (Rule *)&a_rep_opt);

CHOICERULE(rep_choice, REP_CHOICE,
    (Rule *)&rep_seq_ba,
    (Rule *)&b,
    (Rule *)&rep_seq_aa);

REPEATRULE(rep_rep, REP_REP,
    (Rule *)&rep_choice,
    1);

PRODUCTION(rep_parser, REP_PARSER,
    (Rule *)&rep_rep);

LISTRULE(ab_list, AB_LIST,
    (Rule *)&a,
    (Rule *)&b);

LISTRULE(ba_list, BA_LIST,
    (Rule *)&b,
    (Rule *)&a);

CHOICERULE(list_choice, LIST_CHOICE,
    (Rule *)&ab_list,
    (Rule *)&ba_list);

REPEATRULE(list_rep, LIST_REP,
    (Rule *)&list_choice,
    1);

PRODUCTION(list_parser, LIST_PARSER,
    (Rule *)&list_rep);

NEGATIVELOOKAHEAD(nla_a, NLA_A,
    (Rule *)&a);

NEGATIVELOOKAHEAD(nla_b, NLA_B,
    (Rule *)&b);

POSITIVELOOKAHEAD(pla_a, PLA_A,
    (Rule *)&a);

POSITIVELOOKAHEAD(pla_b, PLA_B,
    (Rule *)&b);

SEQUENCERULE(nla_seq, NLA_SEQ,
    (Rule *)&a2_seq,
    (Rule *)&nla_a);

SEQUENCERULE(pla_seq, PLA_SEQ,
    (Rule *)&b2_seq,
    (Rule *)&pla_a);

CHOICERULE(la_choice, LA_CHOICE,
    (Rule *)&nla_seq,
    (Rule *)&pla_seq);

REPEATRULE(la_rep, LA_REP,
    (Rule *)&la_choice,
    1);

PRODUCTION(la_parser, LA_PARSER,
    (Rule *)&la_rep);

void test_rule_cleanup(void) {
    a._class->dest(&a);
    b._class->dest(&b);
    // should only have to destroy the LiteralRules to clean up regex allocations
    // if tests require additional LiteralRules or other Rules require cleanup...add them
}

int test_sequence(void) {
    int nerrors = 0;
    char const * string = "abbbabaaababbb";
    char const * result_tokens[] = {"a","b","b","b","a","b","a","a","a","b","a","b","b","b",NULL};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);
    TestASTNode * result_nodes = &TESTASTNODE(SEQ_PARSER, "a", "b", 7, 
        &TESTASTNODE(6, "a", "b", 2, 
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE) 
        ),
        &TESTASTNODE(5, "b", "b", 2, 
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE) 
        ),
        &TESTASTNODE(6, "a", "b", 2, 
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE) 
        ),
        &TESTASTNODE(4, "a", "a", 2, 
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE) 
        ),
        &TESTASTNODE(6, "a", "b", 2, 
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE) 
        ),
        &TESTASTNODE(6, "a", "b", 2, 
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE) 
        ),
        &TESTASTNODE(5, "b", "b", 2, 
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE) 
        )
    );

    Parser parser;
    Parser_init(&parser, (Rule *)&ab_letter_token, (Rule *)&seq_parser, SEQ_PARSER+1, 0);
    Parser_set_log_file(&parser, test_log_file, LOG_LEVEL_ERROR);
    
    parser._class->parse(&parser, string, strlen(string));
    nerrors += check_tokens(parser.token_head->next, Parser_get_ntokens(&parser), result_tokens, __FILE__, __func__, __LINE__);
    nerrors += check_ASTNodes(parser.ast, result_nodes, __FILE__, __func__, __LINE__);
    /* // for print debugging
    FILE * ast_out = fopen("test_sequence_ast.txt", "w");
    nerrors += CHECK(parser.ast != parser.fail_node, "failed to parse string in %s\n", __func__);
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

int test_repeat(void) {
    int nerrors = 0;
    char const * string = "aaabbbbbbbaaaaaaaa";
    char const * result_tokens[] = {"a","a","a","b","b","b","b","b","b","b","a","a","a","a","a","a","a","a",NULL};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);
    TestASTNode * result_nodes = &TESTASTNODE(20, "a", "a", 3, 
        &TESTASTNODE(17, "a", "a", 2, 
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(14, "a", "a", 1, 
                &TESTASTNODE(0, "a", "a", 0, NULLNODE)
            ), 
        ),
        &TESTASTNODE(17, "a", "a", 2, 
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(14, "", "", 0, NULLNODE)
        ),
        &TESTASTNODE(16, "b", "a", 2, 
            &TESTASTNODE(13, "b", "b", 7, 
                &TESTASTNODE(1, "b", "b", 0, NULLNODE),
                &TESTASTNODE(1, "b", "b", 0, NULLNODE),
                &TESTASTNODE(1, "b", "b", 0, NULLNODE),
                &TESTASTNODE(1, "b", "b", 0, NULLNODE),
                &TESTASTNODE(1, "b", "b", 0, NULLNODE),
                &TESTASTNODE(1, "b", "b", 0, NULLNODE),
                &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            ),
            &TESTASTNODE(10, "a", "a", 8, 
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            ),
        ),
    );

    Parser parser;
    Parser_init(&parser, (Rule *)&ab_letter_token, (Rule *)&rep_parser, REP_PARSER+1, 0);
    Parser_set_log_file(&parser, test_log_file, LOG_LEVEL_ERROR);
    
    size_t Nstring = strlen(string);
    parser._class->parse(&parser, string, Nstring);
    nerrors += check_tokens(parser.token_head->next, Parser_get_ntokens(&parser), result_tokens, __FILE__, __func__, __LINE__);
    nerrors += check_ASTNodes(parser.ast, result_nodes, __FILE__, __func__, __LINE__);
    /*// for print debugging
    FILE * ast_out = fopen("test_repeat_ast.txt", "w");
    nerrors += CHECK(parser.ast != &ASTNode_fail, "failed to parse string in %s\n", __func__);
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

int test_list(void) {
    int nerrors = 0;
    char const * string = "abababaabababababbabababababb";
    char const * result_tokens[] = {"a","b","a","b","a","b","a","a","b","a","b","a","b","a","b","a","b","b","a","b","a","b","a","b","a","b","a","b","b",NULL};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);
    TestASTNode * result_nodes = &TESTASTNODE(25, "a", "b", 5, 
        &TESTASTNODE(22, "a", "a", 7, 
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
        ),
        &TESTASTNODE(22, "a", "a", 9, 
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
        ),
        &TESTASTNODE(21, "b", "b", 1, 
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
        ),
        &TESTASTNODE(21, "b", "b", 11, 
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
            &TESTASTNODE(0, "a", "a", 0, NULLNODE),
            &TESTASTNODE(1, "b", "b", 0, NULLNODE),
        ),
        &TESTASTNODE(21, "b", "b", 1, 
            &TESTASTNODE(1, "b", "b", 0, NULLNODE)
        )
    );

    Parser parser;
    Parser_init(&parser, (Rule *)&ab_letter_token, (Rule *)&list_parser, LIST_PARSER+1, 0);
    Parser_set_log_file(&parser, test_log_file, LOG_LEVEL_ERROR);
    
    size_t Nstring = strlen(string);
    parser._class->parse(&parser, string, Nstring);
    nerrors += check_tokens(parser.token_head->next, Parser_get_ntokens(&parser), result_tokens, __FILE__, __func__, __LINE__);
    nerrors += check_ASTNodes(parser.ast, result_nodes, __FILE__, __func__, __LINE__);
    /*// for print debugging
    FILE * ast_out = fopen("test_repeat_ast.txt", "w");
    nerrors += CHECK(parser.ast != &ASTNode_fail, "failed to parse string in %s\n", __func__);
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

int test_lookahead(void) {
    int nerrors = 0;
    char const * string = "aabbaa";
    char const * result_tokens[] = {"a","a","b","b","a","a",NULL};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);
    TestASTNode * result_nodes = &TESTASTNODE(34, "a", "a", 3, 
        &TESTASTNODE(30, "a", "a", 2, 
            &TESTASTNODE(4, "a", "a", 2, 
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
                &TESTASTNODE(0, "a", "a", 0, NULLNODE)
            ),
            &TESTASTNODE(26, "", "", 0, NULLNODE),
        ),
        &TESTASTNODE(31, "b", "b", 2, 
            &TESTASTNODE(5, "b", "b", 2, 
                &TESTASTNODE(1, "b", "b", 0, NULLNODE),
                &TESTASTNODE(1, "b", "b", 0, NULLNODE)
            ),
            &TESTASTNODE(28, "", "", 0, NULLNODE)
        ),
        &TESTASTNODE(30, "a", "a", 2, 
            &TESTASTNODE(4, "a", "a", 2, 
                &TESTASTNODE(0, "a", "a", 0, NULLNODE),
                &TESTASTNODE(0, "a", "a", 0, NULLNODE)
            ),
            &TESTASTNODE(26, "", "", 0, NULLNODE)
        )
    );

    Parser parser;
    Parser_init(&parser, (Rule *)&ab_letter_token, (Rule *)&la_parser, LA_PARSER+1, 0);
    Parser_set_log_file(&parser, test_log_file, LOG_LEVEL_ERROR);
    
    size_t Nstring = strlen(string);
    parser._class->parse(&parser, string, Nstring);
    nerrors += check_tokens(parser.token_head->next, Parser_get_ntokens(&parser), result_tokens, __FILE__, __func__, __LINE__);
    nerrors += check_ASTNodes(parser.ast, result_nodes, __FILE__, __func__, __LINE__);
    /*// for print debugging
    FILE * ast_out = fopen("test_repeat_ast.txt", "w");
    nerrors += CHECK(parser.ast != &ASTNode_fail, "failed to parse string in %s\n", __func__);
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

