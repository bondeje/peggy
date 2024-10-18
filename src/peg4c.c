/* this file is auto-generated, do not modify */
#include "peg4c/parser_gen.h"
#include "peg4c.h"
#include "peg4ctransform.h"
Production peg4c_whitespace; // PEG4C_WHITESPACE
LiteralRule peg4c_whitespace_re; // PEG4C_WHITESPACE_RE
Production peg4c_string_literal; // PEG4C_STRING_LITERAL
LiteralRule peg4c_string_literal_re; // PEG4C_STRING_LITERAL_RE
Production peg4c_regex_literal; // PEG4C_REGEX_LITERAL
LiteralRule peg4c_regex_literal_re; // PEG4C_REGEX_LITERAL_RE
LiteralRule peg4c_punctuator; // PEG4C_PUNCTUATOR
LiteralRule peg4c_exclaim; // PEG4C_EXCLAIM
LiteralRule peg4c_vbar; // PEG4C_VBAR
LiteralRule peg4c_comma; // PEG4C_COMMA
LiteralRule peg4c_question; // PEG4C_QUESTION
LiteralRule peg4c_period; // PEG4C_PERIOD
LiteralRule peg4c_ampersand; // PEG4C_AMPERSAND
LiteralRule peg4c_colon; // PEG4C_COLON
LiteralRule peg4c_plus; // PEG4C_PLUS
LiteralRule peg4c_asterisk; // PEG4C_ASTERISK
LiteralRule peg4c_lparen; // PEG4C_LPAREN
LiteralRule peg4c_rparen; // PEG4C_RPAREN
LiteralRule peg4c_lbrace; // PEG4C_LBRACE
LiteralRule peg4c_rbrace; // PEG4C_RBRACE
LiteralRule peg4c_equals; // PEG4C_EQUALS
Production peg4c_digit_seq; // PEG4C_DIGIT_SEQ
LiteralRule peg4c_digit_seq_re; // PEG4C_DIGIT_SEQ_RE
LiteralRule peg4c_keyword; // PEG4C_KEYWORD
LiteralRule peg4c_punctuator_kw; // PEG4C_PUNCTUATOR_KW
LiteralRule peg4c_keyword_kw; // PEG4C_KEYWORD_KW
LiteralRule peg4c_token_kw; // PEG4C_TOKEN_KW
Production peg4c_identifier; // PEG4C_IDENTIFIER
LiteralRule peg4c_identifier_re; // PEG4C_IDENTIFIER_RE
Production peg4c_token; // PEG4C_TOKEN
ChoiceRule peg4c_choice_7_30; // PEG4C_CHOICE_7_30
Production peg4c_nonws_printable; // PEG4C_NONWS_PRINTABLE
LiteralRule peg4c_nonws_printable_re; // PEG4C_NONWS_PRINTABLE_RE
Production peg4c_nonterminal; // PEG4C_NONTERMINAL
Production peg4c_terminal; // PEG4C_TERMINAL
ChoiceRule peg4c_choice_2_35; // PEG4C_CHOICE_2_35
Production peg4c_base_rule; // PEG4C_BASE_RULE
ChoiceRule peg4c_choice_3_37; // PEG4C_CHOICE_3_37
SequenceRule peg4c_seq_3_38; // PEG4C_SEQ_3_38
Production peg4c_lookahead_rule; // PEG4C_LOOKAHEAD_RULE
SequenceRule peg4c_seq_2_41; // PEG4C_SEQ_2_41
RepeatRule peg4c_rep_0_1_42; // PEG4C_REP_0_1_42
ChoiceRule peg4c_choice_2_43; // PEG4C_CHOICE_2_43
Production peg4c_list_rule; // PEG4C_LIST_RULE
ListRule peg4c_list_2_45; // PEG4C_LIST_2_45
Production peg4c_repeated_rule; // PEG4C_REPEATED_RULE
SequenceRule peg4c_seq_2_47; // PEG4C_SEQ_2_47
RepeatRule peg4c_rep_0_1_48; // PEG4C_REP_0_1_48
ChoiceRule peg4c_choice_4_49; // PEG4C_CHOICE_4_49
SequenceRule peg4c_seq_5_50; // PEG4C_SEQ_5_50
RepeatRule peg4c_rep_0_1_51; // PEG4C_REP_0_1_51
RepeatRule peg4c_rep_0_1_52; // PEG4C_REP_0_1_52
Production peg4c_sequence; // PEG4C_SEQUENCE
ListRule peg4c_list_2_54; // PEG4C_LIST_2_54
Production peg4c_choice; // PEG4C_CHOICE
ListRule peg4c_list_2_55; // PEG4C_LIST_2_55
Production peg4c_special_production; // PEG4C_SPECIAL_PRODUCTION
ChoiceRule peg4c_choice_2_57; // PEG4C_CHOICE_2_57
SequenceRule peg4c_seq_3_58; // PEG4C_SEQ_3_58
SequenceRule peg4c_seq_3_59; // PEG4C_SEQ_3_59
ChoiceRule peg4c_choice_2_60; // PEG4C_CHOICE_2_60
ListRule peg4c_list_2_61; // PEG4C_LIST_2_61
Production peg4c_transform_functions; // PEG4C_TRANSFORM_FUNCTIONS
ListRule peg4c_list_2_63; // PEG4C_LIST_2_63
Production peg4c_production; // PEG4C_PRODUCTION
SequenceRule peg4c_seq_4_65; // PEG4C_SEQ_4_65
RepeatRule peg4c_rep_0_1_66; // PEG4C_REP_0_1_66
SequenceRule peg4c_seq_3_67; // PEG4C_SEQ_3_67
Production peg4c_config; // PEG4C_CONFIG
SequenceRule peg4c_seq_3_69; // PEG4C_SEQ_3_69
Production peg4c_peg4c; // PEG4C_PEG4C
RepeatRule peg4c_rep_1_0_71; // PEG4C_REP_1_0_71
ChoiceRule peg4c_choice_3_72; // PEG4C_CHOICE_3_72
LITERALRULE(peg4c_vbar,PEG4C_VBAR,
	"\\|");
PRODUCTION(peg4c_token,PEG4C_TOKEN,
	(Rule *)&peg4c_choice_7_30,
	token_action);
CHOICERULE(peg4c_choice_2_57,PEG4C_CHOICE_2_57,
	(Rule *)&peg4c_seq_3_58,
	(Rule *)&peg4c_seq_3_59);
PRODUCTION(peg4c_sequence,PEG4C_SEQUENCE,
	(Rule *)&peg4c_list_2_54,
	simplify_rule);
CHOICERULE(peg4c_choice_2_43,PEG4C_CHOICE_2_43,
	(Rule *)&peg4c_ampersand,
	(Rule *)&peg4c_exclaim);
PRODUCTION(peg4c_whitespace,PEG4C_WHITESPACE,
	(Rule *)&peg4c_whitespace_re,
	skip_token);
LITERALRULE(peg4c_rbrace,PEG4C_RBRACE,
	"}");
LITERALRULE(peg4c_question,PEG4C_QUESTION,
	"\\?");
SEQUENCERULE(peg4c_seq_3_59,PEG4C_SEQ_3_59,
	(Rule *)&peg4c_choice_2_60,
	(Rule *)&peg4c_colon,
	(Rule *)&peg4c_list_2_61);
LITERALRULE(peg4c_nonws_printable_re,PEG4C_NONWS_PRINTABLE_RE,
	"[^ \t\r\n\v\f]+");
CHOICERULE(peg4c_choice_7_30,PEG4C_CHOICE_7_30,
	(Rule *)&peg4c_whitespace,
	(Rule *)&peg4c_string_literal,
	(Rule *)&peg4c_regex_literal,
	(Rule *)&peg4c_punctuator,
	(Rule *)&peg4c_identifier,
	(Rule *)&peg4c_keyword,
	(Rule *)&peg4c_digit_seq);
CHOICERULE(peg4c_choice_3_37,PEG4C_CHOICE_3_37,
	(Rule *)&peg4c_terminal,
	(Rule *)&peg4c_nonterminal,
	(Rule *)&peg4c_seq_3_38);
LITERALRULE(peg4c_lparen,PEG4C_LPAREN,
	"\\(");
LISTRULE(peg4c_list_2_61,PEG4C_LIST_2_61,
	(Rule *)&peg4c_vbar,
	(Rule *)&peg4c_string_literal);
REPEATRULE(peg4c_rep_0_1_42,PEG4C_REP_0_1_42,
	(Rule *)&peg4c_choice_2_43,
	0,
	1);
REPEATRULE(peg4c_rep_0_1_48,PEG4C_REP_0_1_48,
	(Rule *)&peg4c_choice_4_49,
	0,
	1);
LITERALRULE(peg4c_punctuator,PEG4C_PUNCTUATOR,
	"!|\\||,|\\?|\\.|&|:|\\+|\\*|\\(|\\)|\\{|}|=");
SEQUENCERULE(peg4c_seq_3_58,PEG4C_SEQ_3_58,
	(Rule *)&peg4c_token_kw,
	(Rule *)&peg4c_colon,
	(Rule *)&peg4c_choice);
LISTRULE(peg4c_list_2_55,PEG4C_LIST_2_55,
	(Rule *)&peg4c_vbar,
	(Rule *)&peg4c_sequence);
PRODUCTION(peg4c_config,PEG4C_CONFIG,
	(Rule *)&peg4c_seq_3_69);
LISTRULE(peg4c_list_2_54,PEG4C_LIST_2_54,
	(Rule *)&peg4c_comma,
	(Rule *)&peg4c_repeated_rule);
CHOICERULE(peg4c_choice_2_35,PEG4C_CHOICE_2_35,
	(Rule *)&peg4c_string_literal,
	(Rule *)&peg4c_regex_literal);
LITERALRULE(peg4c_exclaim,PEG4C_EXCLAIM,
	"!");
LITERALRULE(peg4c_punctuator_kw,PEG4C_PUNCTUATOR_KW,
	"punctuator");
SEQUENCERULE(peg4c_seq_2_47,PEG4C_SEQ_2_47,
	(Rule *)&peg4c_list_rule,
	(Rule *)&peg4c_rep_0_1_48);
LITERALRULE(peg4c_rparen,PEG4C_RPAREN,
	"\\)");
PRODUCTION(peg4c_repeated_rule,PEG4C_REPEATED_RULE,
	(Rule *)&peg4c_seq_2_47,
	simplify_rule);
REPEATRULE(peg4c_rep_0_1_51,PEG4C_REP_0_1_51,
	(Rule *)&peg4c_digit_seq,
	0,
	1);
LITERALRULE(peg4c_token_kw,PEG4C_TOKEN_KW,
	"token");
REPEATRULE(peg4c_rep_1_0_71,PEG4C_REP_1_0_71,
	(Rule *)&peg4c_choice_3_72,
	1);
SEQUENCERULE(peg4c_seq_3_38,PEG4C_SEQ_3_38,
	(Rule *)&peg4c_lparen,
	(Rule *)&peg4c_choice,
	(Rule *)&peg4c_rparen);
SEQUENCERULE(peg4c_seq_2_41,PEG4C_SEQ_2_41,
	(Rule *)&peg4c_rep_0_1_42,
	(Rule *)&peg4c_base_rule);
PRODUCTION(peg4c_digit_seq,PEG4C_DIGIT_SEQ,
	(Rule *)&peg4c_digit_seq_re);
LITERALRULE(peg4c_asterisk,PEG4C_ASTERISK,
	"\\*");
REPEATRULE(peg4c_rep_0_1_52,PEG4C_REP_0_1_52,
	(Rule *)&peg4c_comma,
	0,
	1);
SEQUENCERULE(peg4c_seq_3_69,PEG4C_SEQ_3_69,
	(Rule *)&peg4c_identifier,
	(Rule *)&peg4c_equals,
	(Rule *)&peg4c_nonws_printable);
PRODUCTION(peg4c_peg4c,PEG4C_PEG4C,
	(Rule *)&peg4c_rep_1_0_71,
	handle_peg4c);
LITERALRULE(peg4c_colon,PEG4C_COLON,
	":");
PRODUCTION(peg4c_nonterminal,PEG4C_NONTERMINAL,
	(Rule *)&peg4c_identifier);
PRODUCTION(peg4c_list_rule,PEG4C_LIST_RULE,
	(Rule *)&peg4c_list_2_45,
	simplify_rule);
LITERALRULE(peg4c_string_literal_re,PEG4C_STRING_LITERAL_RE,
	"'((\\\\')|[^'])*'");
PRODUCTION(peg4c_terminal,PEG4C_TERMINAL,
	(Rule *)&peg4c_choice_2_35);
PRODUCTION(peg4c_transform_functions,PEG4C_TRANSFORM_FUNCTIONS,
	(Rule *)&peg4c_list_2_63);
LITERALRULE(peg4c_plus,PEG4C_PLUS,
	"\\+");
PRODUCTION(peg4c_regex_literal,PEG4C_REGEX_LITERAL,
	(Rule *)&peg4c_regex_literal_re);
REPEATRULE(peg4c_rep_0_1_66,PEG4C_REP_0_1_66,
	(Rule *)&peg4c_seq_3_67,
	0,
	1);
PRODUCTION(peg4c_base_rule,PEG4C_BASE_RULE,
	(Rule *)&peg4c_choice_3_37);
PRODUCTION(peg4c_production,PEG4C_PRODUCTION,
	(Rule *)&peg4c_seq_4_65);
CHOICERULE(peg4c_choice_3_72,PEG4C_CHOICE_3_72,
	(Rule *)&peg4c_config,
	(Rule *)&peg4c_special_production,
	(Rule *)&peg4c_production);
LITERALRULE(peg4c_regex_literal_re,PEG4C_REGEX_LITERAL_RE,
	"\"([^\"\\\\]|(\\\\.))*\"");
LITERALRULE(peg4c_keyword,PEG4C_KEYWORD,
	"punctuator|keyword|token");
SEQUENCERULE(peg4c_seq_5_50,PEG4C_SEQ_5_50,
	(Rule *)&peg4c_lbrace,
	(Rule *)&peg4c_rep_0_1_51,
	(Rule *)&peg4c_rep_0_1_52,
	(Rule *)&peg4c_rep_0_1_51,
	(Rule *)&peg4c_rbrace);
PRODUCTION(peg4c_choice,PEG4C_CHOICE,
	(Rule *)&peg4c_list_2_55,
	simplify_rule);
PRODUCTION(peg4c_nonws_printable,PEG4C_NONWS_PRINTABLE,
	(Rule *)&peg4c_nonws_printable_re);
LISTRULE(peg4c_list_2_63,PEG4C_LIST_2_63,
	(Rule *)&peg4c_comma,
	(Rule *)&peg4c_nonws_printable);
LITERALRULE(peg4c_identifier_re,PEG4C_IDENTIFIER_RE,
	"[A-Za-z0-9_]+");
PRODUCTION(peg4c_string_literal,PEG4C_STRING_LITERAL,
	(Rule *)&peg4c_string_literal_re);
LITERALRULE(peg4c_comma,PEG4C_COMMA,
	",");
PRODUCTION(peg4c_special_production,PEG4C_SPECIAL_PRODUCTION,
	(Rule *)&peg4c_choice_2_57);
SEQUENCERULE(peg4c_seq_4_65,PEG4C_SEQ_4_65,
	(Rule *)&peg4c_identifier,
	(Rule *)&peg4c_rep_0_1_66,
	(Rule *)&peg4c_colon,
	(Rule *)&peg4c_choice);
LITERALRULE(peg4c_keyword_kw,PEG4C_KEYWORD_KW,
	"keyword");
PRODUCTION(peg4c_lookahead_rule,PEG4C_LOOKAHEAD_RULE,
	(Rule *)&peg4c_seq_2_41,
	simplify_rule);
CHOICERULE(peg4c_choice_4_49,PEG4C_CHOICE_4_49,
	(Rule *)&peg4c_plus,
	(Rule *)&peg4c_asterisk,
	(Rule *)&peg4c_question,
	(Rule *)&peg4c_seq_5_50);
LITERALRULE(peg4c_whitespace_re,PEG4C_WHITESPACE_RE,
	"([ \t\r\n\v\f]+|(//[^\n]*\n)|(/\\*((\\*[^/])|[^*])*\\*/))+");
LITERALRULE(peg4c_lbrace,PEG4C_LBRACE,
	"\\{");
CHOICERULE(peg4c_choice_2_60,PEG4C_CHOICE_2_60,
	(Rule *)&peg4c_punctuator_kw,
	(Rule *)&peg4c_keyword_kw);
LITERALRULE(peg4c_equals,PEG4C_EQUALS,
	"=");
PRODUCTION(peg4c_identifier,PEG4C_IDENTIFIER,
	(Rule *)&peg4c_identifier_re);
LITERALRULE(peg4c_digit_seq_re,PEG4C_DIGIT_SEQ_RE,
	"[0-9]+");
LITERALRULE(peg4c_ampersand,PEG4C_AMPERSAND,
	"&");
LISTRULE(peg4c_list_2_45,PEG4C_LIST_2_45,
	(Rule *)&peg4c_period,
	(Rule *)&peg4c_lookahead_rule);
SEQUENCERULE(peg4c_seq_3_67,PEG4C_SEQ_3_67,
	(Rule *)&peg4c_lparen,
	(Rule *)&peg4c_transform_functions,
	(Rule *)&peg4c_rparen);
LITERALRULE(peg4c_period,PEG4C_PERIOD,
	"\\.");


Rule * peg4crules[PEG4C_NRULES + 1] = {
	[PEG4C_VBAR] = (Rule *)&peg4c_vbar,
	[PEG4C_TOKEN] = (Rule *)&peg4c_token,
	[PEG4C_CHOICE_2_57] = (Rule *)&peg4c_choice_2_57,
	[PEG4C_SEQUENCE] = (Rule *)&peg4c_sequence,
	[PEG4C_CHOICE_2_43] = (Rule *)&peg4c_choice_2_43,
	[PEG4C_WHITESPACE] = (Rule *)&peg4c_whitespace,
	[PEG4C_RBRACE] = (Rule *)&peg4c_rbrace,
	[PEG4C_QUESTION] = (Rule *)&peg4c_question,
	[PEG4C_SEQ_3_59] = (Rule *)&peg4c_seq_3_59,
	[PEG4C_NONWS_PRINTABLE_RE] = (Rule *)&peg4c_nonws_printable_re,
	[PEG4C_CHOICE_7_30] = (Rule *)&peg4c_choice_7_30,
	[PEG4C_CHOICE_3_37] = (Rule *)&peg4c_choice_3_37,
	[PEG4C_LPAREN] = (Rule *)&peg4c_lparen,
	[PEG4C_LIST_2_61] = (Rule *)&peg4c_list_2_61,
	[PEG4C_REP_0_1_42] = (Rule *)&peg4c_rep_0_1_42,
	[PEG4C_REP_0_1_48] = (Rule *)&peg4c_rep_0_1_48,
	[PEG4C_PUNCTUATOR] = (Rule *)&peg4c_punctuator,
	[PEG4C_SEQ_3_58] = (Rule *)&peg4c_seq_3_58,
	[PEG4C_LIST_2_55] = (Rule *)&peg4c_list_2_55,
	[PEG4C_CONFIG] = (Rule *)&peg4c_config,
	[PEG4C_LIST_2_54] = (Rule *)&peg4c_list_2_54,
	[PEG4C_CHOICE_2_35] = (Rule *)&peg4c_choice_2_35,
	[PEG4C_EXCLAIM] = (Rule *)&peg4c_exclaim,
	[PEG4C_PUNCTUATOR_KW] = (Rule *)&peg4c_punctuator_kw,
	[PEG4C_SEQ_2_47] = (Rule *)&peg4c_seq_2_47,
	[PEG4C_RPAREN] = (Rule *)&peg4c_rparen,
	[PEG4C_REPEATED_RULE] = (Rule *)&peg4c_repeated_rule,
	[PEG4C_REP_0_1_51] = (Rule *)&peg4c_rep_0_1_51,
	[PEG4C_TOKEN_KW] = (Rule *)&peg4c_token_kw,
	[PEG4C_REP_1_0_71] = (Rule *)&peg4c_rep_1_0_71,
	[PEG4C_SEQ_3_38] = (Rule *)&peg4c_seq_3_38,
	[PEG4C_SEQ_2_41] = (Rule *)&peg4c_seq_2_41,
	[PEG4C_DIGIT_SEQ] = (Rule *)&peg4c_digit_seq,
	[PEG4C_ASTERISK] = (Rule *)&peg4c_asterisk,
	[PEG4C_REP_0_1_52] = (Rule *)&peg4c_rep_0_1_52,
	[PEG4C_SEQ_3_69] = (Rule *)&peg4c_seq_3_69,
	[PEG4C_PEG4C] = (Rule *)&peg4c_peg4c,
	[PEG4C_COLON] = (Rule *)&peg4c_colon,
	[PEG4C_NONTERMINAL] = (Rule *)&peg4c_nonterminal,
	[PEG4C_LIST_RULE] = (Rule *)&peg4c_list_rule,
	[PEG4C_STRING_LITERAL_RE] = (Rule *)&peg4c_string_literal_re,
	[PEG4C_TERMINAL] = (Rule *)&peg4c_terminal,
	[PEG4C_TRANSFORM_FUNCTIONS] = (Rule *)&peg4c_transform_functions,
	[PEG4C_PLUS] = (Rule *)&peg4c_plus,
	[PEG4C_REGEX_LITERAL] = (Rule *)&peg4c_regex_literal,
	[PEG4C_REP_0_1_66] = (Rule *)&peg4c_rep_0_1_66,
	[PEG4C_BASE_RULE] = (Rule *)&peg4c_base_rule,
	[PEG4C_PRODUCTION] = (Rule *)&peg4c_production,
	[PEG4C_CHOICE_3_72] = (Rule *)&peg4c_choice_3_72,
	[PEG4C_REGEX_LITERAL_RE] = (Rule *)&peg4c_regex_literal_re,
	[PEG4C_KEYWORD] = (Rule *)&peg4c_keyword,
	[PEG4C_SEQ_5_50] = (Rule *)&peg4c_seq_5_50,
	[PEG4C_CHOICE] = (Rule *)&peg4c_choice,
	[PEG4C_NONWS_PRINTABLE] = (Rule *)&peg4c_nonws_printable,
	[PEG4C_LIST_2_63] = (Rule *)&peg4c_list_2_63,
	[PEG4C_IDENTIFIER_RE] = (Rule *)&peg4c_identifier_re,
	[PEG4C_STRING_LITERAL] = (Rule *)&peg4c_string_literal,
	[PEG4C_COMMA] = (Rule *)&peg4c_comma,
	[PEG4C_SPECIAL_PRODUCTION] = (Rule *)&peg4c_special_production,
	[PEG4C_SEQ_4_65] = (Rule *)&peg4c_seq_4_65,
	[PEG4C_KEYWORD_KW] = (Rule *)&peg4c_keyword_kw,
	[PEG4C_LOOKAHEAD_RULE] = (Rule *)&peg4c_lookahead_rule,
	[PEG4C_CHOICE_4_49] = (Rule *)&peg4c_choice_4_49,
	[PEG4C_WHITESPACE_RE] = (Rule *)&peg4c_whitespace_re,
	[PEG4C_LBRACE] = (Rule *)&peg4c_lbrace,
	[PEG4C_CHOICE_2_60] = (Rule *)&peg4c_choice_2_60,
	[PEG4C_EQUALS] = (Rule *)&peg4c_equals,
	[PEG4C_IDENTIFIER] = (Rule *)&peg4c_identifier,
	[PEG4C_DIGIT_SEQ_RE] = (Rule *)&peg4c_digit_seq_re,
	[PEG4C_AMPERSAND] = (Rule *)&peg4c_ampersand,
	[PEG4C_LIST_2_45] = (Rule *)&peg4c_list_2_45,
	[PEG4C_SEQ_3_67] = (Rule *)&peg4c_seq_3_67,
	[PEG4C_PERIOD] = (Rule *)&peg4c_period,
	[PEG4C_NRULES] = NULL
};

void peg4c_dest(void) {
	int i = 0;
	while (peg4crules[i]) {
		peg4crules[i]->_class->dest(peg4crules[i]);
		i++;
	}
}
