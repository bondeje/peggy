/* this file is auto-generated, do not modify */
#include <peggy/parser_gen.h>
#include "peggy.h"
#include "peggytransform.h"
Production peggy_whitespace; // WHITESPACE
LiteralRule peggy_whitespace_re; // WHITESPACE_RE
Production peggy_string_literal; // STRING_LITERAL
LiteralRule peggy_string_literal_re; // STRING_LITERAL_RE
Production peggy_regex_literal; // REGEX_LITERAL
LiteralRule peggy_regex_literal_re; // REGEX_LITERAL_RE
LiteralRule peggy_punctuator; // PUNCTUATOR
LiteralRule exclaim; // EXCLAIM
LiteralRule vbar; // VBAR
LiteralRule comma; // COMMA
LiteralRule question; // QUESTION
LiteralRule period; // PERIOD
LiteralRule ampersand; // AMPERSAND
LiteralRule colon; // COLON
LiteralRule plus; // PLUS
LiteralRule asterisk; // ASTERISK
LiteralRule lparen; // LPAREN
LiteralRule rparen; // RPAREN
LiteralRule lbrace; // LBRACE
LiteralRule rbrace; // RBRACE
LiteralRule equals; // EQUALS
Production peggy_digit_seq; // DIGIT_SEQ
LiteralRule peggy_digit_seq_re; // DIGIT_SEQ_RE
LiteralRule peggy_keyword; // KEYWORD
LiteralRule punctuator_kw; // PUNCTUATOR_KW
LiteralRule keyword_kw; // KEYWORD_KW
LiteralRule token_kw; // TOKEN_KW
Production peggy_identifier; // IDENTIFIER
LiteralRule peggy_identifier_re; // IDENTIFIER_RE
Production peggy_token; // TOKEN
ChoiceRule peggy_choice_7_30; // CHOICE_7_30
Production peggy_nonws_printable; // NONWS_PRINTABLE
LiteralRule peggy_nonws_printable_re; // NONWS_PRINTABLE_RE
Production peggy_nonterminal; // NONTERMINAL
Production peggy_terminal; // TERMINAL
ChoiceRule peggy_choice_2_35; // CHOICE_2_35
Production peggy_base_rule; // BASE_RULE
ChoiceRule peggy_choice_3_37; // CHOICE_3_37
SequenceRule peggy_seq_3_38; // SEQ_3_38
Production peggy_lookahead_rule; // LOOKAHEAD_RULE
SequenceRule peggy_seq_2_41; // SEQ_2_41
RepeatRule peggy_rep_0_1_42; // REP_0_1_42
ChoiceRule peggy_choice_2_43; // CHOICE_2_43
Production peggy_list_rule; // LIST_RULE
ListRule peggy_list_2_45; // LIST_2_45
Production peggy_repeated_rule; // REPEATED_RULE
SequenceRule peggy_seq_2_47; // SEQ_2_47
RepeatRule peggy_rep_0_1_48; // REP_0_1_48
ChoiceRule peggy_choice_4_49; // CHOICE_4_49
SequenceRule peggy_seq_5_50; // SEQ_5_50
RepeatRule peggy_rep_0_1_51; // REP_0_1_51
RepeatRule peggy_rep_0_1_52; // REP_0_1_52
Production peggy_sequence; // SEQUENCE
ListRule peggy_list_2_54; // LIST_2_54
Production peggy_choice; // CHOICE
ListRule peggy_list_2_55; // LIST_2_55
Production peggy_special_production; // SPECIAL_PRODUCTION
ChoiceRule peggy_choice_2_57; // CHOICE_2_57
SequenceRule peggy_seq_3_58; // SEQ_3_58
SequenceRule peggy_seq_3_59; // SEQ_3_59
ChoiceRule peggy_choice_2_60; // CHOICE_2_60
ListRule peggy_list_2_61; // LIST_2_61
Production peggy_transform_functions; // TRANSFORM_FUNCTIONS
ListRule peggy_list_2_63; // LIST_2_63
Production peggy_production; // PRODUCTION
SequenceRule peggy_seq_4_65; // SEQ_4_65
RepeatRule peggy_rep_0_1_66; // REP_0_1_66
SequenceRule peggy_seq_3_67; // SEQ_3_67
Production peggy_config; // CONFIG
SequenceRule peggy_seq_3_69; // SEQ_3_69
Production peggy_peggy; // PEGGY
RepeatRule peggy_rep_1_0_71; // REP_1_0_71
ChoiceRule peggy_choice_3_72; // CHOICE_3_72
LITERALRULE(peggy_string_literal_re,STRING_LITERAL_RE,
	"'((\\\\)\'|[^\'])*'");
LITERALRULE(vbar,VBAR,
	"\\|");
PRODUCTION(peggy_token,TOKEN,
	(Rule *)&peggy_choice_7_30,
	token_action);
CHOICERULE(peggy_choice_2_57,CHOICE_2_57,
	(Rule *)&peggy_seq_3_58,
	(Rule *)&peggy_seq_3_59);
PRODUCTION(peggy_sequence,SEQUENCE,
	(Rule *)&peggy_list_2_54,
	simplify_rule);
CHOICERULE(peggy_choice_2_43,CHOICE_2_43,
	(Rule *)&ampersand,
	(Rule *)&exclaim);
PRODUCTION(peggy_whitespace,WHITESPACE,
	(Rule *)&peggy_whitespace_re,
	skip_token);
LITERALRULE(rbrace,RBRACE,
	"}");
PRODUCTION(peggy_peggy,PEGGY,
	(Rule *)&peggy_rep_1_0_71,
	handle_peggy);
LITERALRULE(question,QUESTION,
	"\\?");
SEQUENCERULE(peggy_seq_3_59,SEQ_3_59,
	(Rule *)&peggy_choice_2_60,
	(Rule *)&colon,
	(Rule *)&peggy_list_2_61);
LITERALRULE(peggy_nonws_printable_re,NONWS_PRINTABLE_RE,
	"[^ \t\r\n\v\f]+");
CHOICERULE(peggy_choice_7_30,CHOICE_7_30,
	(Rule *)&peggy_whitespace,
	(Rule *)&peggy_string_literal,
	(Rule *)&peggy_regex_literal,
	(Rule *)&peggy_punctuator,
	(Rule *)&peggy_identifier,
	(Rule *)&peggy_keyword,
	(Rule *)&peggy_digit_seq);
CHOICERULE(peggy_choice_3_37,CHOICE_3_37,
	(Rule *)&peggy_terminal,
	(Rule *)&peggy_nonterminal,
	(Rule *)&peggy_seq_3_38);
LITERALRULE(lparen,LPAREN,
	"\\(");
LISTRULE(peggy_list_2_61,LIST_2_61,
	(Rule *)&vbar,
	(Rule *)&peggy_string_literal);
REPEATRULE(peggy_rep_0_1_42,REP_0_1_42,
	(Rule *)&peggy_choice_2_43,
	0,
	1);
REPEATRULE(peggy_rep_0_1_48,REP_0_1_48,
	(Rule *)&peggy_choice_4_49,
	0,
	1);
LITERALRULE(peggy_punctuator,PUNCTUATOR,
	"!|\\||,|\\?|\\.|&|:|\\+|\\*|\\(|\\)|\\{|}|=");
SEQUENCERULE(peggy_seq_3_58,SEQ_3_58,
	(Rule *)&token_kw,
	(Rule *)&colon,
	(Rule *)&peggy_choice);
LISTRULE(peggy_list_2_55,LIST_2_55,
	(Rule *)&vbar,
	(Rule *)&peggy_sequence);
PRODUCTION(peggy_config,CONFIG,
	(Rule *)&peggy_seq_3_69);
LISTRULE(peggy_list_2_54,LIST_2_54,
	(Rule *)&comma,
	(Rule *)&peggy_repeated_rule);
CHOICERULE(peggy_choice_2_35,CHOICE_2_35,
	(Rule *)&peggy_string_literal,
	(Rule *)&peggy_regex_literal);
LITERALRULE(exclaim,EXCLAIM,
	"!");
LITERALRULE(punctuator_kw,PUNCTUATOR_KW,
	"punctuator");
SEQUENCERULE(peggy_seq_2_47,SEQ_2_47,
	(Rule *)&peggy_list_rule,
	(Rule *)&peggy_rep_0_1_48);
LITERALRULE(rparen,RPAREN,
	"\\)");
PRODUCTION(peggy_repeated_rule,REPEATED_RULE,
	(Rule *)&peggy_seq_2_47,
	simplify_rule);
REPEATRULE(peggy_rep_0_1_51,REP_0_1_51,
	(Rule *)&peggy_digit_seq,
	0,
	1);
LITERALRULE(token_kw,TOKEN_KW,
	"token");
REPEATRULE(peggy_rep_1_0_71,REP_1_0_71,
	(Rule *)&peggy_choice_3_72,
	1);
SEQUENCERULE(peggy_seq_3_38,SEQ_3_38,
	(Rule *)&lparen,
	(Rule *)&peggy_choice,
	(Rule *)&rparen);
SEQUENCERULE(peggy_seq_2_41,SEQ_2_41,
	(Rule *)&peggy_rep_0_1_42,
	(Rule *)&peggy_base_rule);
PRODUCTION(peggy_digit_seq,DIGIT_SEQ,
	(Rule *)&peggy_digit_seq_re);
LITERALRULE(asterisk,ASTERISK,
	"\\*");
REPEATRULE(peggy_rep_0_1_52,REP_0_1_52,
	(Rule *)&comma,
	0,
	1);
SEQUENCERULE(peggy_seq_3_69,SEQ_3_69,
	(Rule *)&peggy_identifier,
	(Rule *)&equals,
	(Rule *)&peggy_nonws_printable);
LITERALRULE(colon,COLON,
	":");
PRODUCTION(peggy_nonterminal,NONTERMINAL,
	(Rule *)&peggy_identifier);
PRODUCTION(peggy_list_rule,LIST_RULE,
	(Rule *)&peggy_list_2_45,
	simplify_rule);
LITERALRULE(peggy_regex_literal_re,REGEX_LITERAL_RE,
	"\"((\\\\\")|[^\"])*\"");
PRODUCTION(peggy_terminal,TERMINAL,
	(Rule *)&peggy_choice_2_35);
PRODUCTION(peggy_transform_functions,TRANSFORM_FUNCTIONS,
	(Rule *)&peggy_list_2_63);
LITERALRULE(plus,PLUS,
	"\\+");
PRODUCTION(peggy_regex_literal,REGEX_LITERAL,
	(Rule *)&peggy_regex_literal_re);
REPEATRULE(peggy_rep_0_1_66,REP_0_1_66,
	(Rule *)&peggy_seq_3_67,
	0,
	1);
PRODUCTION(peggy_base_rule,BASE_RULE,
	(Rule *)&peggy_choice_3_37);
PRODUCTION(peggy_production,PRODUCTION,
	(Rule *)&peggy_seq_4_65);
CHOICERULE(peggy_choice_3_72,CHOICE_3_72,
	(Rule *)&peggy_config,
	(Rule *)&peggy_special_production,
	(Rule *)&peggy_production);
LITERALRULE(peggy_keyword,KEYWORD,
	"punctuator|keyword|token");
SEQUENCERULE(peggy_seq_5_50,SEQ_5_50,
	(Rule *)&lbrace,
	(Rule *)&peggy_rep_0_1_51,
	(Rule *)&peggy_rep_0_1_52,
	(Rule *)&peggy_rep_0_1_51,
	(Rule *)&rbrace);
PRODUCTION(peggy_choice,CHOICE,
	(Rule *)&peggy_list_2_55,
	simplify_rule);
PRODUCTION(peggy_nonws_printable,NONWS_PRINTABLE,
	(Rule *)&peggy_nonws_printable_re);
LISTRULE(peggy_list_2_63,LIST_2_63,
	(Rule *)&comma,
	(Rule *)&peggy_nonws_printable);
LITERALRULE(peggy_identifier_re,IDENTIFIER_RE,
	"[A-Za-z0-9_]+");
PRODUCTION(peggy_string_literal,STRING_LITERAL,
	(Rule *)&peggy_string_literal_re);
LITERALRULE(comma,COMMA,
	",");
PRODUCTION(peggy_special_production,SPECIAL_PRODUCTION,
	(Rule *)&peggy_choice_2_57);
SEQUENCERULE(peggy_seq_4_65,SEQ_4_65,
	(Rule *)&peggy_identifier,
	(Rule *)&peggy_rep_0_1_66,
	(Rule *)&colon,
	(Rule *)&peggy_choice);
LITERALRULE(keyword_kw,KEYWORD_KW,
	"keyword");
PRODUCTION(peggy_lookahead_rule,LOOKAHEAD_RULE,
	(Rule *)&peggy_seq_2_41,
	simplify_rule);
CHOICERULE(peggy_choice_4_49,CHOICE_4_49,
	(Rule *)&plus,
	(Rule *)&asterisk,
	(Rule *)&question,
	(Rule *)&peggy_seq_5_50);
LITERALRULE(peggy_whitespace_re,WHITESPACE_RE,
	"([ \t\r\n\v\f]+|(//[^\n]*\n)|(/\\*((\\*[^/])|[^*])*\\*/))+");
LITERALRULE(lbrace,LBRACE,
	"\\{");
CHOICERULE(peggy_choice_2_60,CHOICE_2_60,
	(Rule *)&punctuator_kw,
	(Rule *)&keyword_kw);
LITERALRULE(equals,EQUALS,
	"=");
PRODUCTION(peggy_identifier,IDENTIFIER,
	(Rule *)&peggy_identifier_re);
LITERALRULE(peggy_digit_seq_re,DIGIT_SEQ_RE,
	"[0-9]+");
LITERALRULE(ampersand,AMPERSAND,
	"&");
LISTRULE(peggy_list_2_45,LIST_2_45,
	(Rule *)&period,
	(Rule *)&peggy_lookahead_rule);
SEQUENCERULE(peggy_seq_3_67,SEQ_3_67,
	(Rule *)&lparen,
	(Rule *)&peggy_transform_functions,
	(Rule *)&rparen);
LITERALRULE(period,PERIOD,
	"\\.");


Rule * peggyrules[PEGGY_NRULES + 1] = {
	(Rule *)&peggy_string_literal_re,
	(Rule *)&vbar,
	(Rule *)&peggy_token,
	(Rule *)&peggy_choice_2_57,
	(Rule *)&peggy_sequence,
	(Rule *)&peggy_choice_2_43,
	(Rule *)&peggy_whitespace,
	(Rule *)&rbrace,
	(Rule *)&peggy_peggy,
	(Rule *)&question,
	(Rule *)&peggy_seq_3_59,
	(Rule *)&peggy_nonws_printable_re,
	(Rule *)&peggy_choice_7_30,
	(Rule *)&peggy_choice_3_37,
	(Rule *)&lparen,
	(Rule *)&peggy_list_2_61,
	(Rule *)&peggy_rep_0_1_42,
	(Rule *)&peggy_rep_0_1_48,
	(Rule *)&peggy_punctuator,
	(Rule *)&peggy_seq_3_58,
	(Rule *)&peggy_list_2_55,
	(Rule *)&peggy_config,
	(Rule *)&peggy_list_2_54,
	(Rule *)&peggy_choice_2_35,
	(Rule *)&exclaim,
	(Rule *)&punctuator_kw,
	(Rule *)&peggy_seq_2_47,
	(Rule *)&rparen,
	(Rule *)&peggy_repeated_rule,
	(Rule *)&peggy_rep_0_1_51,
	(Rule *)&token_kw,
	(Rule *)&peggy_rep_1_0_71,
	(Rule *)&peggy_seq_3_38,
	(Rule *)&peggy_seq_2_41,
	(Rule *)&peggy_digit_seq,
	(Rule *)&asterisk,
	(Rule *)&peggy_rep_0_1_52,
	(Rule *)&peggy_seq_3_69,
	(Rule *)&colon,
	(Rule *)&peggy_nonterminal,
	(Rule *)&peggy_list_rule,
	(Rule *)&peggy_regex_literal_re,
	(Rule *)&peggy_terminal,
	(Rule *)&peggy_transform_functions,
	(Rule *)&plus,
	(Rule *)&peggy_regex_literal,
	(Rule *)&peggy_rep_0_1_66,
	(Rule *)&peggy_base_rule,
	(Rule *)&peggy_production,
	(Rule *)&peggy_choice_3_72,
	(Rule *)&peggy_keyword,
	(Rule *)&peggy_seq_5_50,
	(Rule *)&peggy_choice,
	(Rule *)&peggy_nonws_printable,
	(Rule *)&peggy_list_2_63,
	(Rule *)&peggy_identifier_re,
	(Rule *)&peggy_string_literal,
	(Rule *)&comma,
	(Rule *)&peggy_special_production,
	(Rule *)&peggy_seq_4_65,
	(Rule *)&keyword_kw,
	(Rule *)&peggy_lookahead_rule,
	(Rule *)&peggy_choice_4_49,
	(Rule *)&peggy_whitespace_re,
	(Rule *)&lbrace,
	(Rule *)&peggy_choice_2_60,
	(Rule *)&equals,
	(Rule *)&peggy_identifier,
	(Rule *)&peggy_digit_seq_re,
	(Rule *)&ampersand,
	(Rule *)&peggy_list_2_45,
	(Rule *)&peggy_seq_3_67,
	(Rule *)&period,
	NULL
};

void peggy_dest(void) {
	int i = 0;
	while (peggyrules[i]) {
		peggyrules[i]->_class->dest(peggyrules[i]);
		i++;
	}
}
