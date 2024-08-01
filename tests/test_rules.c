#include <string.h>

#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/parser_gen.h>

#include "test_utils.h"
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
    ab_letter._class->dest(&ab_letter);
    ab_letter_token._class->dest(&ab_letter_token);
    a2_seq._class->dest(&a2_seq);
    b2_seq._class->dest(&b2_seq);
    ab_seq._class->dest(&ab_seq);
    seq_parser_choice._class->dest(&seq_parser_choice);
    seq_parser._class->dest(&seq_parser);
    a_rep_any._class->dest(&a_rep_any);
    b_rep_any._class->dest(&b_rep_any);
    a_rep_gt1._class->dest(&a_rep_gt1);
    b_rep_gt1._class->dest(&b_rep_gt1);
    a_rep_opt._class->dest(&a_rep_opt);
    b_rep_opt._class->dest(&b_rep_opt);
    
    ab_list._class->dest(&ab_list);
    ba_list._class->dest(&ba_list);
    
    nla_a._class->dest(&nla_a);
    nla_b._class->dest(&nla_b);
    
    nla_a._class->dest(&nla_a);
    nla_b._class->dest(&nla_b);
}

int test_sequence(void) {
    int nerrors = 0;
    char const * string = "abbbabaaababbb";
    char * result_tokens[] = {"aa","bb","ab","aa","ab","ab","bb"};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&ab_letter_token, (Rule *)&seq_parser, SEQ_PARSER+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    parser._class->parse(&parser, string, strlen(string));
    FILE * ast_out = fopen("test_sequence_ast.txt", "w");
    nerrors += CHECK(parser.ast != &ASTNode_fail, "failed to parse string in %s\n", __func__);

    // TODO: replace printing with checks
    Parser_print_tokens(&parser, ast_out);
    Parser_print_ast(&parser, ast_out);
    fclose(ast_out);

    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s!\n", __func__, nerrors ? "failed" : "passed");
    }

    return nerrors;
}

int test_repeat(void) {
    int nerrors = 0;
    char const * string = "aaabbbbbbbaaaaaaaa";
    char * result_tokens[] = {"a","a","a","b","b","b","b","b","b","b","a","a","a","a","a","a","a","a"};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&ab_letter_token, (Rule *)&rep_parser, REP_PARSER+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    size_t Nstring = strlen(string);
    parser._class->parse(&parser, string, Nstring);
    FILE * ast_out = fopen("test_repeat_ast.txt", "w");
    nerrors += CHECK(parser.ast != &ASTNode_fail, "failed to parse string in %s\n", __func__);

    // TODO: replace printing with checks
    Parser_print_tokens(&parser, ast_out);
    Parser_print_ast(&parser, ast_out);
    fclose(ast_out);

    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s!\n", __func__, nerrors ? "failed" : "passed");
    }

    return nerrors;
}

int test_list(void) {
    int nerrors = 0;
    char const * string = "abababaabababababbabababababb";
    char * result_tokens[] = {"a","b","a","b","a","b","a","a","b","a","b","a","b","a","b","a","b","b","a","b","a","b","a","b","a","b","a","b","b"};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&ab_letter_token, (Rule *)&list_parser, LIST_PARSER+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    size_t Nstring = strlen(string);
    parser._class->parse(&parser, string, Nstring);
    FILE * ast_out = fopen("test_list_ast.txt", "w");
    nerrors += CHECK(parser.ast != &ASTNode_fail, "failed to parse string in %s\n", __func__);

    // TODO: replace printing with checks
    Parser_print_tokens(&parser, ast_out);
    Parser_print_ast(&parser, ast_out);
    fclose(ast_out);

    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s!\n", __func__, nerrors ? "failed" : "passed");
    }

    return nerrors;
}

int test_lookahead(void) {
    int nerrors = 0;
    char const * string = "aabbaa";
    char * result_tokens[] = {"a","a","b","b","a","a"};
    size_t N = sizeof(result_tokens)/sizeof(result_tokens[0]);

    Parser parser;
    Parser_init(&parser, __func__, strlen(__func__), (Rule *)&ab_letter_token, (Rule *)&la_parser, LA_PARSER+1, 0, test_log_file, LOG_LEVEL_ERROR);
    
    size_t Nstring = strlen(string);
    parser._class->parse(&parser, string, Nstring);
    FILE * ast_out = fopen("test_lookahead_ast.txt", "w");
    nerrors += CHECK(parser.ast != &ASTNode_fail, "failed to parse string in %s\n", __func__);

    // TODO: replace printing with checks
    Parser_print_tokens(&parser, ast_out);
    Parser_print_ast(&parser, ast_out);
    fclose(ast_out);

    Parser_dest(&parser);

    if (verbose) {
        printf("%s...%s!\n", __func__, nerrors ? "failed" : "passed");
    }

    return nerrors;
}

