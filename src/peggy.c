/* this file is auto-generated, do not modify */
#include <peggy/parser_gen.h>
#include "peggy.h"
#include "peggytransform.h"
Production peggy_whitespace; // PEGGY_WHITESPACE
LiteralRule peggy_peggy_whitespace_re; // PEGGY_PEGGY_WHITESPACE_RE
Production peggy_string_literal; // PEGGY_STRING_LITERAL
LiteralRule peggy_peggy_string_literal_re; // PEGGY_PEGGY_STRING_LITERAL_RE
Production peggy_regex_literal; // PEGGY_REGEX_LITERAL
LiteralRule peggy_peggy_regex_literal_re; // PEGGY_PEGGY_REGEX_LITERAL_RE
LiteralRule peggy_punctuator; // PEGGY_PUNCTUATOR
LiteralRule peggy_exclaim; // PEGGY_EXCLAIM
LiteralRule peggy_vbar; // PEGGY_VBAR
LiteralRule peggy_comma; // PEGGY_COMMA
LiteralRule peggy_question; // PEGGY_QUESTION
LiteralRule peggy_period; // PEGGY_PERIOD
LiteralRule peggy_ampersand; // PEGGY_AMPERSAND
LiteralRule peggy_colon; // PEGGY_COLON
LiteralRule peggy_plus; // PEGGY_PLUS
LiteralRule peggy_asterisk; // PEGGY_ASTERISK
LiteralRule peggy_lparen; // PEGGY_LPAREN
LiteralRule peggy_rparen; // PEGGY_RPAREN
LiteralRule peggy_lbrace; // PEGGY_LBRACE
LiteralRule peggy_rbrace; // PEGGY_RBRACE
LiteralRule peggy_equals; // PEGGY_EQUALS
Production peggy_digit_seq; // PEGGY_DIGIT_SEQ
LiteralRule peggy_peggy_digit_seq_re; // PEGGY_PEGGY_DIGIT_SEQ_RE
LiteralRule peggy_keyword; // PEGGY_KEYWORD
LiteralRule peggy_punctuator_kw; // PEGGY_PUNCTUATOR_KW
LiteralRule peggy_keyword_kw; // PEGGY_KEYWORD_KW
LiteralRule peggy_token_kw; // PEGGY_TOKEN_KW
Production peggy_identifier; // PEGGY_IDENTIFIER
LiteralRule peggy_peggy_identifier_re; // PEGGY_PEGGY_IDENTIFIER_RE
Production peggy_token; // PEGGY_TOKEN
ChoiceRule peggy_choice_7_30; // PEGGY_CHOICE_7_30
Production peggy_nonws_printable; // PEGGY_NONWS_PRINTABLE
LiteralRule peggy_peggy_nonws_printable_re; // PEGGY_PEGGY_NONWS_PRINTABLE_RE
Production peggy_nonterminal; // PEGGY_NONTERMINAL
Production peggy_terminal; // PEGGY_TERMINAL
ChoiceRule peggy_choice_2_35; // PEGGY_CHOICE_2_35
Production peggy_base_rule; // PEGGY_BASE_RULE
ChoiceRule peggy_choice_3_37; // PEGGY_CHOICE_3_37
SequenceRule peggy_seq_3_38; // PEGGY_SEQ_3_38
Production peggy_lookahead_rule; // PEGGY_LOOKAHEAD_RULE
SequenceRule peggy_seq_2_41; // PEGGY_SEQ_2_41
RepeatRule peggy_rep_0_1_42; // PEGGY_REP_0_1_42
ChoiceRule peggy_choice_2_43; // PEGGY_CHOICE_2_43
Production peggy_list_rule; // PEGGY_LIST_RULE
ListRule peggy_list_2_45; // PEGGY_LIST_2_45
Production peggy_repeated_rule; // PEGGY_REPEATED_RULE
SequenceRule peggy_seq_2_47; // PEGGY_SEQ_2_47
RepeatRule peggy_rep_0_1_48; // PEGGY_REP_0_1_48
ChoiceRule peggy_choice_4_49; // PEGGY_CHOICE_4_49
SequenceRule peggy_seq_5_50; // PEGGY_SEQ_5_50
RepeatRule peggy_rep_0_1_51; // PEGGY_REP_0_1_51
RepeatRule peggy_rep_0_1_52; // PEGGY_REP_0_1_52
Production peggy_sequence; // PEGGY_SEQUENCE
ListRule peggy_list_2_54; // PEGGY_LIST_2_54
Production peggy_choice; // PEGGY_CHOICE
ListRule peggy_list_2_55; // PEGGY_LIST_2_55
Production peggy_special_production; // PEGGY_SPECIAL_PRODUCTION
ChoiceRule peggy_choice_2_57; // PEGGY_CHOICE_2_57
SequenceRule peggy_seq_3_58; // PEGGY_SEQ_3_58
SequenceRule peggy_seq_3_59; // PEGGY_SEQ_3_59
ChoiceRule peggy_choice_2_60; // PEGGY_CHOICE_2_60
ListRule peggy_list_2_61; // PEGGY_LIST_2_61
Production peggy_transform_functions; // PEGGY_TRANSFORM_FUNCTIONS
ListRule peggy_list_2_63; // PEGGY_LIST_2_63
Production peggy_production; // PEGGY_PRODUCTION
SequenceRule peggy_seq_4_65; // PEGGY_SEQ_4_65
RepeatRule peggy_rep_0_1_66; // PEGGY_REP_0_1_66
SequenceRule peggy_seq_3_67; // PEGGY_SEQ_3_67
Production peggy_config; // PEGGY_CONFIG
SequenceRule peggy_seq_3_69; // PEGGY_SEQ_3_69
Production peggy_peggy; // PEGGY_PEGGY
RepeatRule peggy_rep_1_0_71; // PEGGY_REP_1_0_71
ChoiceRule peggy_choice_3_72; // PEGGY_CHOICE_3_72
LITERALRULE(peggy_vbar,PEGGY_VBAR,
	"\\|");
PRODUCTION(peggy_token,PEGGY_TOKEN,
	(Rule *)&peggy_choice_7_30,
	token_action);
CHOICERULE(peggy_choice_2_57,PEGGY_CHOICE_2_57,
	(Rule *)&peggy_seq_3_58,
	(Rule *)&peggy_seq_3_59);
PRODUCTION(peggy_sequence,PEGGY_SEQUENCE,
	(Rule *)&peggy_list_2_54,
	simplify_rule);
CHOICERULE(peggy_choice_2_43,PEGGY_CHOICE_2_43,
	(Rule *)&peggy_ampersand,
	(Rule *)&peggy_exclaim);
PRODUCTION(peggy_whitespace,PEGGY_WHITESPACE,
	(Rule *)&peggy_peggy_whitespace_re,
	skip_token);
LITERALRULE(peggy_rbrace,PEGGY_RBRACE,
	"}");
PRODUCTION(peggy_peggy,PEGGY_PEGGY,
	(Rule *)&peggy_rep_1_0_71,
	handle_peggy);
LITERALRULE(peggy_question,PEGGY_QUESTION,
	"\\?");
SEQUENCERULE(peggy_seq_3_59,PEGGY_SEQ_3_59,
	(Rule *)&peggy_choice_2_60,
	(Rule *)&peggy_colon,
	(Rule *)&peggy_list_2_61);
LITERALRULE(peggy_peggy_nonws_printable_re,PEGGY_PEGGY_NONWS_PRINTABLE_RE,
	"[^ \t\r\n\v\f]+");
CHOICERULE(peggy_choice_7_30,PEGGY_CHOICE_7_30,
	(Rule *)&peggy_whitespace,
	(Rule *)&peggy_string_literal,
	(Rule *)&peggy_regex_literal,
	(Rule *)&peggy_punctuator,
	(Rule *)&peggy_identifier,
	(Rule *)&peggy_keyword,
	(Rule *)&peggy_digit_seq);
CHOICERULE(peggy_choice_3_37,PEGGY_CHOICE_3_37,
	(Rule *)&peggy_terminal,
	(Rule *)&peggy_nonterminal,
	(Rule *)&peggy_seq_3_38);
LITERALRULE(peggy_lparen,PEGGY_LPAREN,
	"\\(");
LISTRULE(peggy_list_2_61,PEGGY_LIST_2_61,
	(Rule *)&peggy_vbar,
	(Rule *)&peggy_string_literal);
REPEATRULE(peggy_rep_0_1_42,PEGGY_REP_0_1_42,
	(Rule *)&peggy_choice_2_43,
	0,
	1);
REPEATRULE(peggy_rep_0_1_48,PEGGY_REP_0_1_48,
	(Rule *)&peggy_choice_4_49,
	0,
	1);
LITERALRULE(peggy_punctuator,PEGGY_PUNCTUATOR,
	"!|\\||,|\\?|\\.|&|:|\\+|\\*|\\(|\\)|\\{|}|=");
SEQUENCERULE(peggy_seq_3_58,PEGGY_SEQ_3_58,
	(Rule *)&peggy_token_kw,
	(Rule *)&peggy_colon,
	(Rule *)&peggy_choice);
LISTRULE(peggy_list_2_55,PEGGY_LIST_2_55,
	(Rule *)&peggy_vbar,
	(Rule *)&peggy_sequence);
PRODUCTION(peggy_config,PEGGY_CONFIG,
	(Rule *)&peggy_seq_3_69);
LISTRULE(peggy_list_2_54,PEGGY_LIST_2_54,
	(Rule *)&peggy_comma,
	(Rule *)&peggy_repeated_rule);
CHOICERULE(peggy_choice_2_35,PEGGY_CHOICE_2_35,
	(Rule *)&peggy_string_literal,
	(Rule *)&peggy_regex_literal);
LITERALRULE(peggy_exclaim,PEGGY_EXCLAIM,
	"!");
LITERALRULE(peggy_punctuator_kw,PEGGY_PUNCTUATOR_KW,
	"punctuator");
SEQUENCERULE(peggy_seq_2_47,PEGGY_SEQ_2_47,
	(Rule *)&peggy_list_rule,
	(Rule *)&peggy_rep_0_1_48);
LITERALRULE(peggy_rparen,PEGGY_RPAREN,
	"\\)");
PRODUCTION(peggy_repeated_rule,PEGGY_REPEATED_RULE,
	(Rule *)&peggy_seq_2_47,
	simplify_rule);
REPEATRULE(peggy_rep_0_1_51,PEGGY_REP_0_1_51,
	(Rule *)&peggy_digit_seq,
	0,
	1);
LITERALRULE(peggy_token_kw,PEGGY_TOKEN_KW,
	"token");
REPEATRULE(peggy_rep_1_0_71,PEGGY_REP_1_0_71,
	(Rule *)&peggy_choice_3_72,
	1);
SEQUENCERULE(peggy_seq_3_38,PEGGY_SEQ_3_38,
	(Rule *)&peggy_lparen,
	(Rule *)&peggy_choice,
	(Rule *)&peggy_rparen);
SEQUENCERULE(peggy_seq_2_41,PEGGY_SEQ_2_41,
	(Rule *)&peggy_rep_0_1_42,
	(Rule *)&peggy_base_rule);
PRODUCTION(peggy_digit_seq,PEGGY_DIGIT_SEQ,
	(Rule *)&peggy_peggy_digit_seq_re);
LITERALRULE(peggy_asterisk,PEGGY_ASTERISK,
	"\\*");
REPEATRULE(peggy_rep_0_1_52,PEGGY_REP_0_1_52,
	(Rule *)&peggy_comma,
	0,
	1);
SEQUENCERULE(peggy_seq_3_69,PEGGY_SEQ_3_69,
	(Rule *)&peggy_identifier,
	(Rule *)&peggy_equals,
	(Rule *)&peggy_nonws_printable);
LITERALRULE(peggy_colon,PEGGY_COLON,
	":");
PRODUCTION(peggy_nonterminal,PEGGY_NONTERMINAL,
	(Rule *)&peggy_identifier);
PRODUCTION(peggy_list_rule,PEGGY_LIST_RULE,
	(Rule *)&peggy_list_2_45,
	simplify_rule);
LITERALRULE(peggy_peggy_string_literal_re,PEGGY_PEGGY_STRING_LITERAL_RE,
	"'((\\\\')|[^'])*'");
PRODUCTION(peggy_terminal,PEGGY_TERMINAL,
	(Rule *)&peggy_choice_2_35);
PRODUCTION(peggy_transform_functions,PEGGY_TRANSFORM_FUNCTIONS,
	(Rule *)&peggy_list_2_63);
LITERALRULE(peggy_plus,PEGGY_PLUS,
	"\\+");
PRODUCTION(peggy_regex_literal,PEGGY_REGEX_LITERAL,
	(Rule *)&peggy_peggy_regex_literal_re);
REPEATRULE(peggy_rep_0_1_66,PEGGY_REP_0_1_66,
	(Rule *)&peggy_seq_3_67,
	0,
	1);
PRODUCTION(peggy_base_rule,PEGGY_BASE_RULE,
	(Rule *)&peggy_choice_3_37);
PRODUCTION(peggy_production,PEGGY_PRODUCTION,
	(Rule *)&peggy_seq_4_65);
CHOICERULE(peggy_choice_3_72,PEGGY_CHOICE_3_72,
	(Rule *)&peggy_config,
	(Rule *)&peggy_special_production,
	(Rule *)&peggy_production);
LITERALRULE(peggy_peggy_regex_literal_re,PEGGY_PEGGY_REGEX_LITERAL_RE,
	"\"([^\"\\\\]|(\\\\.))*\"");
LITERALRULE(peggy_keyword,PEGGY_KEYWORD,
	"punctuator|keyword|token");
SEQUENCERULE(peggy_seq_5_50,PEGGY_SEQ_5_50,
	(Rule *)&peggy_lbrace,
	(Rule *)&peggy_rep_0_1_51,
	(Rule *)&peggy_rep_0_1_52,
	(Rule *)&peggy_rep_0_1_51,
	(Rule *)&peggy_rbrace);
PRODUCTION(peggy_choice,PEGGY_CHOICE,
	(Rule *)&peggy_list_2_55,
	simplify_rule);
PRODUCTION(peggy_nonws_printable,PEGGY_NONWS_PRINTABLE,
	(Rule *)&peggy_peggy_nonws_printable_re);
LISTRULE(peggy_list_2_63,PEGGY_LIST_2_63,
	(Rule *)&peggy_comma,
	(Rule *)&peggy_nonws_printable);
LITERALRULE(peggy_peggy_identifier_re,PEGGY_PEGGY_IDENTIFIER_RE,
	"[A-Za-z0-9_]+");
PRODUCTION(peggy_string_literal,PEGGY_STRING_LITERAL,
	(Rule *)&peggy_peggy_string_literal_re);
LITERALRULE(peggy_comma,PEGGY_COMMA,
	",");
PRODUCTION(peggy_special_production,PEGGY_SPECIAL_PRODUCTION,
	(Rule *)&peggy_choice_2_57);
SEQUENCERULE(peggy_seq_4_65,PEGGY_SEQ_4_65,
	(Rule *)&peggy_identifier,
	(Rule *)&peggy_rep_0_1_66,
	(Rule *)&peggy_colon,
	(Rule *)&peggy_choice);
LITERALRULE(peggy_keyword_kw,PEGGY_KEYWORD_KW,
	"keyword");
PRODUCTION(peggy_lookahead_rule,PEGGY_LOOKAHEAD_RULE,
	(Rule *)&peggy_seq_2_41,
	simplify_rule);
CHOICERULE(peggy_choice_4_49,PEGGY_CHOICE_4_49,
	(Rule *)&peggy_plus,
	(Rule *)&peggy_asterisk,
	(Rule *)&peggy_question,
	(Rule *)&peggy_seq_5_50);
LITERALRULE(peggy_peggy_whitespace_re,PEGGY_PEGGY_WHITESPACE_RE,
	"([ \t\r\n\v\f]+|(//[^\n]*\n)|(/\\*((\\*[^/])|[^*])*\\*/))+");
LITERALRULE(peggy_lbrace,PEGGY_LBRACE,
	"\\{");
CHOICERULE(peggy_choice_2_60,PEGGY_CHOICE_2_60,
	(Rule *)&peggy_punctuator_kw,
	(Rule *)&peggy_keyword_kw);
LITERALRULE(peggy_equals,PEGGY_EQUALS,
	"=");
PRODUCTION(peggy_identifier,PEGGY_IDENTIFIER,
	(Rule *)&peggy_peggy_identifier_re);
LITERALRULE(peggy_peggy_digit_seq_re,PEGGY_PEGGY_DIGIT_SEQ_RE,
	"[0-9]+");
LITERALRULE(peggy_ampersand,PEGGY_AMPERSAND,
	"&");
LISTRULE(peggy_list_2_45,PEGGY_LIST_2_45,
	(Rule *)&peggy_period,
	(Rule *)&peggy_lookahead_rule);
SEQUENCERULE(peggy_seq_3_67,PEGGY_SEQ_3_67,
	(Rule *)&peggy_lparen,
	(Rule *)&peggy_transform_functions,
	(Rule *)&peggy_rparen);
LITERALRULE(peggy_period,PEGGY_PERIOD,
	"\\.");


Rule * peggyrules[PEGGY_NRULES + 1] = {
	[PEGGY_VBAR] = (Rule *)&peggy_vbar,
	[PEGGY_TOKEN] = (Rule *)&peggy_token,
	[PEGGY_CHOICE_2_57] = (Rule *)&peggy_choice_2_57,
	[PEGGY_SEQUENCE] = (Rule *)&peggy_sequence,
	[PEGGY_CHOICE_2_43] = (Rule *)&peggy_choice_2_43,
	[PEGGY_WHITESPACE] = (Rule *)&peggy_whitespace,
	[PEGGY_RBRACE] = (Rule *)&peggy_rbrace,
	[PEGGY_PEGGY] = (Rule *)&peggy_peggy,
	[PEGGY_QUESTION] = (Rule *)&peggy_question,
	[PEGGY_SEQ_3_59] = (Rule *)&peggy_seq_3_59,
	[PEGGY_PEGGY_NONWS_PRINTABLE_RE] = (Rule *)&peggy_peggy_nonws_printable_re,
	[PEGGY_CHOICE_7_30] = (Rule *)&peggy_choice_7_30,
	[PEGGY_CHOICE_3_37] = (Rule *)&peggy_choice_3_37,
	[PEGGY_LPAREN] = (Rule *)&peggy_lparen,
	[PEGGY_LIST_2_61] = (Rule *)&peggy_list_2_61,
	[PEGGY_REP_0_1_42] = (Rule *)&peggy_rep_0_1_42,
	[PEGGY_REP_0_1_48] = (Rule *)&peggy_rep_0_1_48,
	[PEGGY_PUNCTUATOR] = (Rule *)&peggy_punctuator,
	[PEGGY_SEQ_3_58] = (Rule *)&peggy_seq_3_58,
	[PEGGY_LIST_2_55] = (Rule *)&peggy_list_2_55,
	[PEGGY_CONFIG] = (Rule *)&peggy_config,
	[PEGGY_LIST_2_54] = (Rule *)&peggy_list_2_54,
	[PEGGY_CHOICE_2_35] = (Rule *)&peggy_choice_2_35,
	[PEGGY_EXCLAIM] = (Rule *)&peggy_exclaim,
	[PEGGY_PUNCTUATOR_KW] = (Rule *)&peggy_punctuator_kw,
	[PEGGY_SEQ_2_47] = (Rule *)&peggy_seq_2_47,
	[PEGGY_RPAREN] = (Rule *)&peggy_rparen,
	[PEGGY_REPEATED_RULE] = (Rule *)&peggy_repeated_rule,
	[PEGGY_REP_0_1_51] = (Rule *)&peggy_rep_0_1_51,
	[PEGGY_TOKEN_KW] = (Rule *)&peggy_token_kw,
	[PEGGY_REP_1_0_71] = (Rule *)&peggy_rep_1_0_71,
	[PEGGY_SEQ_3_38] = (Rule *)&peggy_seq_3_38,
	[PEGGY_SEQ_2_41] = (Rule *)&peggy_seq_2_41,
	[PEGGY_DIGIT_SEQ] = (Rule *)&peggy_digit_seq,
	[PEGGY_ASTERISK] = (Rule *)&peggy_asterisk,
	[PEGGY_REP_0_1_52] = (Rule *)&peggy_rep_0_1_52,
	[PEGGY_SEQ_3_69] = (Rule *)&peggy_seq_3_69,
	[PEGGY_COLON] = (Rule *)&peggy_colon,
	[PEGGY_NONTERMINAL] = (Rule *)&peggy_nonterminal,
	[PEGGY_LIST_RULE] = (Rule *)&peggy_list_rule,
	[PEGGY_PEGGY_STRING_LITERAL_RE] = (Rule *)&peggy_peggy_string_literal_re,
	[PEGGY_TERMINAL] = (Rule *)&peggy_terminal,
	[PEGGY_TRANSFORM_FUNCTIONS] = (Rule *)&peggy_transform_functions,
	[PEGGY_PLUS] = (Rule *)&peggy_plus,
	[PEGGY_REGEX_LITERAL] = (Rule *)&peggy_regex_literal,
	[PEGGY_REP_0_1_66] = (Rule *)&peggy_rep_0_1_66,
	[PEGGY_BASE_RULE] = (Rule *)&peggy_base_rule,
	[PEGGY_PRODUCTION] = (Rule *)&peggy_production,
	[PEGGY_CHOICE_3_72] = (Rule *)&peggy_choice_3_72,
	[PEGGY_PEGGY_REGEX_LITERAL_RE] = (Rule *)&peggy_peggy_regex_literal_re,
	[PEGGY_KEYWORD] = (Rule *)&peggy_keyword,
	[PEGGY_SEQ_5_50] = (Rule *)&peggy_seq_5_50,
	[PEGGY_CHOICE] = (Rule *)&peggy_choice,
	[PEGGY_NONWS_PRINTABLE] = (Rule *)&peggy_nonws_printable,
	[PEGGY_LIST_2_63] = (Rule *)&peggy_list_2_63,
	[PEGGY_PEGGY_IDENTIFIER_RE] = (Rule *)&peggy_peggy_identifier_re,
	[PEGGY_STRING_LITERAL] = (Rule *)&peggy_string_literal,
	[PEGGY_COMMA] = (Rule *)&peggy_comma,
	[PEGGY_SPECIAL_PRODUCTION] = (Rule *)&peggy_special_production,
	[PEGGY_SEQ_4_65] = (Rule *)&peggy_seq_4_65,
	[PEGGY_KEYWORD_KW] = (Rule *)&peggy_keyword_kw,
	[PEGGY_LOOKAHEAD_RULE] = (Rule *)&peggy_lookahead_rule,
	[PEGGY_CHOICE_4_49] = (Rule *)&peggy_choice_4_49,
	[PEGGY_PEGGY_WHITESPACE_RE] = (Rule *)&peggy_peggy_whitespace_re,
	[PEGGY_LBRACE] = (Rule *)&peggy_lbrace,
	[PEGGY_CHOICE_2_60] = (Rule *)&peggy_choice_2_60,
	[PEGGY_EQUALS] = (Rule *)&peggy_equals,
	[PEGGY_IDENTIFIER] = (Rule *)&peggy_identifier,
	[PEGGY_PEGGY_DIGIT_SEQ_RE] = (Rule *)&peggy_peggy_digit_seq_re,
	[PEGGY_AMPERSAND] = (Rule *)&peggy_ampersand,
	[PEGGY_LIST_2_45] = (Rule *)&peggy_list_2_45,
	[PEGGY_SEQ_3_67] = (Rule *)&peggy_seq_3_67,
	[PEGGY_PERIOD] = (Rule *)&peggy_period,
	[PEGGY_NRULES] = NULL
};

void peggy_dest(void) {
	int i = 0;
	while (peggyrules[i]) {
		peggyrules[i]->_class->dest(peggyrules[i]);
		i++;
	}
}
