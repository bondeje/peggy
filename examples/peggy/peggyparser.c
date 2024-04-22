#include <peggy/parser_gen.h>
#include "peggy.h"
#include "peggyparser.h"

char const * peggyrule_names[NRULES] = {
    ""
};

/**
 * punctuator:
 *      '!','|', ',', '?', '.', '&', ':', '+', '*', '(', ')', '{', '}'
*/
LITERAL_RULE(exclaim, EXCLAIM, "^!");
//LITERAL_RULE(dquote, DQUOTE, "^\"");
//LITERAL_RULE(pound, POUND, "^#");
//LITERAL_RULE(dollar, DOLLAR, "^\\$");
//LITERAL_RULE(modulus, MODULUS, "^%");
LITERAL_RULE(ampersand, AMPERSAND, "^&");
//LITERAL_RULE(squote, SQUOTE, "^'");
LITERAL_RULE(lparen, LPAREN, "^\\(");
LITERAL_RULE(rparen, RPAREN, "^\\)");
LITERAL_RULE(asterisk, ASTERISK, "^\\*");
LITERAL_RULE(plus, PLUS, "^\\+");
LITERAL_RULE(comma, COMMA, "^,");
//LITERAL_RULE(minus, MINUS, "^-");
LITERAL_RULE(period, PERIOD, "^\\.");
//LITERAL_RULE(slash, SLASH, "^/");
LITERAL_RULE(colon, COLON, "^:");
//LITERAL_RULE(semicolon, SEMICOLON, "^;");
//LITERAL_RULE(langle, LANGLE, "^<");
//LITERAL_RULE(equals, EQUALS, "^=");
//LITERAL_RULE(rangle, RANGLE "^>");
LITERAL_RULE(question, QUESTION, "^\\?");
//LITERAL_RULE(at_sign, AT_SIGN, "^@");
//LITERAL_RULE(lbracket, LBRACKET, "^\\[");
//LITERAL_RULE(backslash, BACKSLASH, "^\\\\");
//LITERAL_RULE(rbracket, RBRACKET, "^]");
//LITERAL_RULE(caret, CARET, "^\\^");
//LITERAL_RULE(underscore, UNDERSCORE, "^_");
//LITERAL_RULE(backtick, BACKTICK, "^`");
LITERAL_RULE(lbrace, LBRACE, "^\\{");
LITERAL_RULE(vbar, VBAR, "^\\|");
LITERAL_RULE(rbrace, RBRACE, "^}");
//LITERAL_RULE(tilde, TILDE, "^~");
LITERAL_RULE(punctuator, PUNCTUATOR, "^(!|&|\\(|\\)|\\*|\\+|,|\\.|:|\\?|\\{|\\||})");

/**
 * keyword:
 *      'import', 'export', 'punctuator', 'keyword', 'token', 'root'
 */
LITERAL_RULE(import_kw, IMPORT_KW, "^import");
LITERAL_RULE(export_kw, EXPORT_KW, "^export");
LITERAL_RULE(punctuator_kw, PUNCTUATOR_KW, "^punctuator");
LITERAL_RULE(keyword_kw, KEYWORD_KW, "^keyword");
LITERAL_RULE(token_kw, TOKEN_KW, "^token");
LITERAL_RULE(root_kw, ROOT_KW, "^root");
LITERAL_RULE(keyword, KEYWORD, "^(import|export|punctuator|keyword|token|root)");

/* parsing rule instances */

/**
 *  whitespace(skip_token):
 *      "([[:space:]]+|//[^\n]\\*\n|/\\*.*\\* /)+" // includes C-style comments as whitespace
*/
LITERAL_RULE(peggy_whitespace_re, WHITESPACE_RE, "^([[:space:]]+|(//[^\n]*\n)|(/\\*((\\*[^/])|[^*])*\\*/))+");
PRODUCTION(peggy_whitespace, WHITESPACE, &peggy_whitespace_re.Rule, skip_token);

/**
 * nonws_printable:
 *      "[^[:space:]]+"
 * 
 * NOTE: this really should change for the target config
 */
LITERAL_RULE(peggy_nonws_printable_re, NONWS_PRINTABLE_RE, "^[^[:space:]]+");
PRODUCTION(peggy_nonws_printable, NONWS_PRINTABLE, &peggy_nonws_printable_re.Rule);

/**
 * identifier:
 *      "[:word:]"
 */
LITERAL_RULE(peggy_identifier_re, IDENTIFIER_RE, "^[A-Za-z0-9_]+");
PRODUCTION(peggy_identifier, IDENTIFIER, 
    &peggy_identifier_re.Rule);

/**
 * nonterminal:
 *      identifier
 */
PRODUCTION(peggy_nonterminal, NONTERMINAL, 
    &peggy_identifier.AnonymousProduction.DerivedRule.Rule);

/** 
 * regex_literal:
 *      "\"((\\\\\")|[^\"])*\""
 */
LITERAL_RULE(peggy_regex_literal_re, REGEX_LITERAL_RE, "^\"((\\\\\")|[^\"])*\"");
PRODUCTION(peggy_regex_literal, REGEX_LITERAL, 
    &peggy_regex_literal_re.Rule);

/**
 *  string_literal:
 *      "'((\\\\)\'|[^\'])*'"
 */
LITERAL_RULE(peggy_string_literal_re, STRING_LITERAL_RE, "^'((\\\\)\'|[^\'])*'");
PRODUCTION(peggy_string_literal, STRING_LITERAL, 
    &peggy_string_literal_re.Rule);

/**
 * terminal:
 *      string_literal | regex_literal
*/
CHOICE_RULE(peggy_choice_literals, CHOICE_LITERALS, 
    &peggy_string_literal.AnonymousProduction.DerivedRule.Rule, 
    &peggy_regex_literal.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_terminal, TERMINAL, 
    &peggy_choice_literals.ChainRule.Rule);

/**
 * base_rule:
 *      terminal | nonterminal | '(', choice, ')'
*/
Production peggy_choice; // forward declaration
SEQUENCE_RULE(peggy_choice_expression, CHOICE_EXPRESSION, 
    &lparen.Rule, 
    &peggy_choice.AnonymousProduction.DerivedRule.Rule, 
    &rparen.Rule);
CHOICE_RULE(peggy_choice_term_nonterm_choice_expr, CHOICE_TERM_NONTERM_CHOICE_EXPR, 
    &peggy_terminal.AnonymousProduction.DerivedRule.Rule, 
    &rparen.Rule, 
    &peggy_nonterminal.AnonymousProduction.DerivedRule.Rule, 
    &rparen.Rule, 
    &peggy_choice_expression.ChainRule.Rule, 
    &rparen.Rule);
PRODUCTION(peggy_base_rule, BASE_RULE, 
    &peggy_choice_term_nonterm_choice_expr.ChainRule.Rule);

/**
 * lookahead_rule(simplify_rule):
 *      ('&' | '!')?, base_rule
 */
CHOICE_RULE(peggy_choice_posi_nega, CHOICE_POSI_NEGA, 
    &ampersand.Rule, 
    &exclaim.Rule);
OPTIONAL_RULE(peggy_opt_choice_posi_nega, OPT_CHOICE_POSI_NEGA, 
    &peggy_choice_posi_nega.ChainRule.Rule);
SEQUENCE_RULE(peggy_opt_choice_posi_nega_base, OPT_CHOICE_POSI_NEGA_BASE, 
    &peggy_opt_choice_posi_nega.DerivedRule.Rule, 
    &peggy_base_rule.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_lookahead_rule, LOOKAHEAD_RULE, 
    &peggy_opt_choice_posi_nega_base.ChainRule.Rule, 
    simplify_rule);

/**
 * list_rule(simplify_rule):
 *      '.'.lookahead_rule
*/
LIST_RULE(peggy_list_period_lookahead, LIST_PERIOD_LOOKAHEAD, &period.Rule, 
    &peggy_lookahead_rule.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_list_rule, LIST_RULE, 
    &peggy_list_period_lookahead.DerivedRule.Rule, 
    simplify_rule);

/**
 * repeated_rule(simplify_rule):
 *      ('+' | '*' | '?' | '{', \d*, ':', \d* '}')?, list_rule
*/
LITERAL_RULE(peggy_digit_seq, DIGIT_SEQ, "^[0-9]*");
SEQUENCE_RULE(peggy_repeat_params, REPEAT_PARAMS, 
    &lbrace.Rule, 
    &peggy_digit_seq.Rule, 
    &colon.Rule, 
    &peggy_digit_seq.Rule, 
    &rbrace.Rule);
CHOICE_RULE(peggy_repeat_ops, REPEAT_OPS, 
    &plus.Rule, 
    &asterisk.Rule, 
    &question.Rule, 
    &peggy_repeat_params.ChainRule.Rule, 
    &rbrace.Rule);
OPTIONAL_RULE(peggy_opt_repeat_ops, OPT_REPEAT_OPS, 
    &peggy_repeat_ops.ChainRule.Rule);
SEQUENCE_RULE(peggy_opt_repeat_ops_list, OPT_REPEAT_OPS_LIST, 
    &peggy_opt_repeat_ops.DerivedRule.Rule, 
    &peggy_list_rule.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_repeated_rule, REPEATED_RULE, 
    &peggy_opt_repeat_ops_list.ChainRule.Rule, simplify_rule);

/**
 * sequence(simplify_rule):
 *      ','.repeated_rule
*/
LIST_RULE(peggy_list_repeated, LIST_REPEATED, 
    &comma.Rule, 
    &peggy_repeated_rule.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_sequence, SEQUENCE, 
    &peggy_list_repeated.DerivedRule.Rule, simplify_rule);

/**
 * choice(simplify_rule):
 *      '|'.sequence
*/
LIST_RULE(peggy_list_sequence, LIST_SEQUENCE, 
    &vbar.Rule, 
    &peggy_sequence.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_choice, CHOICE, 
    &peggy_list_sequence.DerivedRule.Rule, simplify_rule);

/**
 * token:
 *      whitespace | string_literal | regex_literal | punctuator | keyword | identifier
*/
CHOICE_RULE(peggy_choice_tokens, CHOICE_TOKENS, 
    &peggy_whitespace.AnonymousProduction.DerivedRule.Rule,
    &peggy_string_literal.AnonymousProduction.DerivedRule.Rule,
    &peggy_regex_literal.AnonymousProduction.DerivedRule.Rule,
    &punctuator.Rule,
    &keyword.Rule,
    &peggy_identifier.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_token, TOKEN, 
    &peggy_choice_tokens.ChainRule.Rule, token_action);

/**
 * special_production:
 *        ('token' | 'root'), ':', base_rule 
 *      | ('punctuator' | 'keyword'), ':', ','.string_literal
 */
CHOICE_RULE(peggy_root_or_token, ROOT_OR_TOKEN, 
    &token_kw.Rule, 
    &root_kw.Rule);
SEQUENCE_RULE(peggy_seq_root_or_token, SEQ_ROOT_OR_TOKEN, 
    &peggy_root_or_token.ChainRule.Rule, 
    &colon.Rule,
    &peggy_base_rule.AnonymousProduction.DerivedRule.Rule);
CHOICE_RULE(peggy_punc_or_kw, PUNC_OR_KW, 
    &punctuator_kw.Rule, 
    &keyword_kw.Rule);
LIST_RULE(peggy_list_string, LIST_STRING, 
    &comma.Rule, 
    &peggy_string_literal.AnonymousProduction.DerivedRule.Rule);
SEQUENCE_RULE(peggy_seq_punc_or_kw, SEQ_PUNC_OR_KW, 
    &peggy_punc_or_kw.ChainRule.Rule, 
    &colon.Rule, 
    &peggy_list_string.DerivedRule.Rule);
CHOICE_RULE(peggy_choice_spec_prod, CHOICE_SPEC_PROD,
    &peggy_seq_root_or_token.ChainRule.Rule,
    &peggy_seq_punc_or_kw.ChainRule.Rule);
PRODUCTION(peggy_special_production, SPECIAL_PRODUCTION,
    &peggy_choice_spec_prod.ChainRule.Rule);

/** 
 * transform_functions: 
 *      ','.nonws_printable
 */
LIST_RULE(peggy_list_nonws_print, LIST_NONWS_PRINT, 
    &comma.Rule, 
    &peggy_nonws_printable.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_transform_functions, TRANSFORM_FUNCTIONS, 
    &peggy_list_nonws_print.DerivedRule.Rule);

/** 
 * production: 
 *      identifier, ('(', transform_functions, ')')?, ':', choice 
 */
SEQUENCE_RULE(peggy_transform_func_expr, TRANSFORM_FUNC_EXPR, 
    &lparen.Rule, 
    &peggy_transform_functions.AnonymousProduction.DerivedRule.Rule, 
    &rparen.Rule);
OPTIONAL_RULE(peggy_opt_transform_func_expr, OPT_TRANSFORM_FUNC_EXPR, 
    &peggy_transform_func_expr.ChainRule.Rule);
SEQUENCE_RULE(peggy_seq_prod_rules, SEQ_PROD_RULES, 
    &peggy_identifier.AnonymousProduction.DerivedRule.Rule, 
    &peggy_opt_transform_func_expr.DerivedRule.Rule, 
    &colon.Rule,
    &peggy_choice.AnonymousProduction.DerivedRule.Rule);
// using production ENUM_IDENTIFIER is OK since I don't make a macro function call out of it
PRODUCTION(peggy_production, PRODUCTION, 
    &peggy_seq_prod_rules.ChainRule.Rule);

/** 
 * config: 
 *      ('import' | 'export'), ':', nonws_printable 
 */
CHOICE_RULE(peggy_import_export, IMPORT_EXPORT,
    &import_kw.Rule,
    &export_kw.Rule);
SEQUENCE_RULE(peggy_seq_config, SEQ_CONFIG,
    &peggy_import_export.ChainRule.Rule,
    &colon.Rule,
    &peggy_nonws_printable.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_config, CONFIG, 
    &peggy_seq_config.ChainRule.Rule);

/** 
 * peggy: 
 *      (config | special_production | production)+ 
 */
CHOICE_RULE(peggy_entry, ENTRY,
    &peggy_config.AnonymousProduction.DerivedRule.Rule,
    &peggy_special_production.AnonymousProduction.DerivedRule.Rule,
    &peggy_production.AnonymousProduction.DerivedRule.Rule);
REPEAT_RULE(peggy_rep_entry, REP_ENTRY, 
    &peggy_entry.ChainRule.Rule,
    1);
PRODUCTION(peggy_peggy, PEGGY,
    &peggy_rep_entry.DerivedRule.Rule);

Rule * peggyrules[NRULES + 1] = {
    &exclaim.Rule, // = 0
    &ampersand.Rule,
    &lparen.Rule,
    &rparen.Rule,
    &asterisk.Rule,
    &plus.Rule,
    &comma.Rule,
    &period.Rule,
    &colon.Rule,
    &question.Rule,
    &lbrace.Rule, // = 10
    &vbar.Rule,
    &rbrace.Rule,
    &punctuator.Rule,
    &import_kw.Rule,
    &export_kw.Rule,
    &punctuator_kw.Rule,
    &keyword_kw.Rule,
    &token_kw.Rule,
    &root_kw.Rule,
    &keyword.Rule, // = 20
    &peggy_whitespace_re.Rule,
    &peggy_whitespace.AnonymousProduction.DerivedRule.Rule,
    &peggy_nonws_printable_re.Rule,
    &peggy_nonws_printable.AnonymousProduction.DerivedRule.Rule,
    &peggy_identifier_re.Rule,
    &peggy_identifier.AnonymousProduction.DerivedRule.Rule,
    &peggy_nonterminal.AnonymousProduction.DerivedRule.Rule,
    &peggy_regex_literal_re.Rule,
    &peggy_regex_literal.AnonymousProduction.DerivedRule.Rule,
    &peggy_string_literal_re.Rule, // = 30
    &peggy_string_literal.AnonymousProduction.DerivedRule.Rule,
    &peggy_choice_literals.ChainRule.Rule,
    &peggy_terminal.AnonymousProduction.DerivedRule.Rule,
    &peggy_choice_expression.ChainRule.Rule,
    &peggy_choice_term_nonterm_choice_expr.ChainRule.Rule,
    &peggy_base_rule.AnonymousProduction.DerivedRule.Rule,
    &peggy_choice_posi_nega.ChainRule.Rule,
    &peggy_opt_choice_posi_nega.DerivedRule.Rule,
    &peggy_opt_choice_posi_nega_base.ChainRule.Rule,
    &peggy_lookahead_rule.AnonymousProduction.DerivedRule.Rule, // = 40
    &peggy_list_period_lookahead.DerivedRule.Rule,
    &peggy_list_rule.AnonymousProduction.DerivedRule.Rule,
    &peggy_digit_seq.Rule,
    &peggy_repeat_params.ChainRule.Rule,
    &peggy_repeat_ops.ChainRule.Rule,
    &peggy_opt_repeat_ops.DerivedRule.Rule,
    &peggy_opt_repeat_ops_list.ChainRule.Rule,
    &peggy_repeated_rule.AnonymousProduction.DerivedRule.Rule,
    &peggy_list_repeated.DerivedRule.Rule,
    &peggy_sequence.AnonymousProduction.DerivedRule.Rule, // = 50
    &peggy_list_sequence.DerivedRule.Rule,
    &peggy_choice.AnonymousProduction.DerivedRule.Rule,
    &peggy_choice_tokens.ChainRule.Rule,
    &peggy_token.AnonymousProduction.DerivedRule.Rule,
    &peggy_root_or_token.ChainRule.Rule,
    &peggy_seq_root_or_token.ChainRule.Rule,
    &peggy_punc_or_kw.ChainRule.Rule,
    &peggy_list_string.DerivedRule.Rule,
    &peggy_seq_punc_or_kw.ChainRule.Rule,
    &peggy_choice_spec_prod.ChainRule.Rule, // = 60
    &peggy_special_production.AnonymousProduction.DerivedRule.Rule,
    &peggy_list_nonws_print.DerivedRule.Rule,
    &peggy_transform_functions.AnonymousProduction.DerivedRule.Rule,
    &peggy_transform_func_expr.ChainRule.Rule,
    &peggy_opt_transform_func_expr.DerivedRule.Rule,
    &peggy_seq_prod_rules.ChainRule.Rule,
    &peggy_production.AnonymousProduction.DerivedRule.Rule,
    &peggy_import_export.ChainRule.Rule,
    &peggy_seq_config.ChainRule.Rule,
    &peggy_config.AnonymousProduction.DerivedRule.Rule, // = 70
    &peggy_entry.ChainRule.Rule,
    &peggy_rep_entry.DerivedRule.Rule,
    &peggy_peggy.AnonymousProduction.DerivedRule.Rule,
    NULL
};

void peggy_dest(void) {
    printf("tearing down peggy\n");
    int i = 0;
    while (peggyrules[i]) {
        peggyrules[i]->_class->dest(peggyrules[i]);
        i++;
    }
}