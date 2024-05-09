/* this file is auto-generated, do not modify */
#include <peggy/parser_gen.h>
#include "peggy.h"
#include "peggyparser.h"
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
Production peggy_sequence; // SEQUENCE
ListRule peggy_list_2_53; // LIST_2_53
Production peggy_choice; // CHOICE
ListRule peggy_list_2_54; // LIST_2_54
Production peggy_special_production; // SPECIAL_PRODUCTION
ChoiceRule peggy_choice_2_56; // CHOICE_2_56
SequenceRule peggy_seq_3_57; // SEQ_3_57
SequenceRule peggy_seq_3_58; // SEQ_3_58
ChoiceRule peggy_choice_2_59; // CHOICE_2_59
ListRule peggy_list_2_60; // LIST_2_60
Production peggy_transform_functions; // TRANSFORM_FUNCTIONS
ListRule peggy_list_2_62; // LIST_2_62
Production peggy_production; // PRODUCTION
SequenceRule peggy_seq_4_64; // SEQ_4_64
RepeatRule peggy_rep_0_1_65; // REP_0_1_65
SequenceRule peggy_seq_3_66; // SEQ_3_66
Production peggy_config; // CONFIG
SequenceRule peggy_seq_3_68; // SEQ_3_68
Production peggy_peggy; // PEGGY
RepeatRule peggy_rep_1_0_70; // REP_1_0_70
ChoiceRule peggy_choice_3_71; // CHOICE_3_71
LISTRULE(peggy_list_2_45,LIST_2_45,
	&period.Rule,
	&peggy_lookahead_rule.DerivedRule.Rule);
LITERALRULE(peggy_string_literal_re,STRING_LITERAL_RE,
	"'((\\\\)\'|[^\'])*'");
LITERALRULE(vbar,VBAR,
	"\\|");
CHOICERULE(peggy_choice_4_49,CHOICE_4_49,
	&plus.Rule,
	&asterisk.Rule,
	&question.Rule,
	&peggy_seq_5_50.ChainRule.Rule);
PRODUCTION(peggy_token,TOKEN,
	&peggy_choice_7_30.ChainRule.Rule,
	token_action);
CHOICERULE(peggy_choice_2_59,CHOICE_2_59,
	&punctuator_kw.Rule,
	&keyword_kw.Rule);
SEQUENCERULE(peggy_seq_3_66,SEQ_3_66,
	&lparen.Rule,
	&peggy_transform_functions.DerivedRule.Rule,
	&rparen.Rule);
SEQUENCERULE(peggy_seq_3_38,SEQ_3_38,
	&lparen.Rule,
	&peggy_choice.DerivedRule.Rule,
	&rparen.Rule);
PRODUCTION(peggy_sequence,SEQUENCE,
	&peggy_list_2_53.DerivedRule.Rule,
	simplify_rule);
REPEATRULE(peggy_rep_0_1_42,REP_0_1_42,
	&peggy_choice_2_43.ChainRule.Rule,
	0,
	1);
PRODUCTION(peggy_whitespace,WHITESPACE,
	&peggy_whitespace_re.Rule,
	skip_token);
LITERALRULE(rbrace,RBRACE,
	"}");
PRODUCTION(peggy_peggy,PEGGY,
	&peggy_rep_1_0_70.DerivedRule.Rule,
	handle_peggy);
LITERALRULE(question,QUESTION,
	"\\?");
LITERALRULE(peggy_nonws_printable_re,NONWS_PRINTABLE_RE,
	"[^ \t\r\n\v\f]+");
REPEATRULE(peggy_rep_0_1_65,REP_0_1_65,
	&peggy_seq_3_66.ChainRule.Rule,
	0,
	1);
LITERALRULE(lparen,LPAREN,
	"\\(");
LISTRULE(peggy_list_2_60,LIST_2_60,
	&vbar.Rule,
	&peggy_string_literal.DerivedRule.Rule);
LITERALRULE(peggy_punctuator,PUNCTUATOR,
	"!|\\||,|\\?|\\.|&|:|\\+|\\*|\\(|\\)|\\{|}|=");
LISTRULE(peggy_list_2_54,LIST_2_54,
	&vbar.Rule,
	&peggy_sequence.DerivedRule.Rule);
PRODUCTION(peggy_config,CONFIG,
	&peggy_seq_3_68.ChainRule.Rule);
LISTRULE(peggy_list_2_53,LIST_2_53,
	&comma.Rule,
	&peggy_repeated_rule.DerivedRule.Rule);
LITERALRULE(exclaim,EXCLAIM,
	"!");
LITERALRULE(punctuator_kw,PUNCTUATOR_KW,
	"punctuator");
LITERALRULE(rparen,RPAREN,
	"\\)");
PRODUCTION(peggy_repeated_rule,REPEATED_RULE,
	&peggy_seq_2_47.ChainRule.Rule,
	simplify_rule);
REPEATRULE(peggy_rep_0_1_51,REP_0_1_51,
	&peggy_digit_seq.DerivedRule.Rule,
	0,
	1);
LITERALRULE(token_kw,TOKEN_KW,
	"token");
PRODUCTION(peggy_digit_seq,DIGIT_SEQ,
	&peggy_digit_seq_re.Rule);
LITERALRULE(asterisk,ASTERISK,
	"\\*");
LITERALRULE(colon,COLON,
	":");
CHOICERULE(peggy_choice_3_71,CHOICE_3_71,
	&peggy_config.DerivedRule.Rule,
	&peggy_special_production.DerivedRule.Rule,
	&peggy_production.DerivedRule.Rule);
PRODUCTION(peggy_nonterminal,NONTERMINAL,
	&peggy_identifier.DerivedRule.Rule);
PRODUCTION(peggy_list_rule,LIST_RULE,
	&peggy_list_2_45.DerivedRule.Rule,
	simplify_rule);
LITERALRULE(peggy_regex_literal_re,REGEX_LITERAL_RE,
	"\"((\\\\\")|[^\"])*\"");
PRODUCTION(peggy_terminal,TERMINAL,
	&peggy_choice_2_35.ChainRule.Rule);
CHOICERULE(peggy_choice_3_37,CHOICE_3_37,
	&peggy_terminal.DerivedRule.Rule,
	&peggy_nonterminal.DerivedRule.Rule,
	&peggy_seq_3_38.ChainRule.Rule);
PRODUCTION(peggy_transform_functions,TRANSFORM_FUNCTIONS,
	&peggy_list_2_62.DerivedRule.Rule);
LITERALRULE(plus,PLUS,
	"\\+");
PRODUCTION(peggy_regex_literal,REGEX_LITERAL,
	&peggy_regex_literal_re.Rule);
PRODUCTION(peggy_base_rule,BASE_RULE,
	&peggy_choice_3_37.ChainRule.Rule);
PRODUCTION(peggy_production,PRODUCTION,
	&peggy_seq_4_64.ChainRule.Rule);
LITERALRULE(peggy_keyword,KEYWORD,
	"punctuator|keyword|token");
SEQUENCERULE(peggy_seq_2_41,SEQ_2_41,
	&peggy_rep_0_1_42.DerivedRule.Rule,
	&peggy_base_rule.DerivedRule.Rule);
CHOICERULE(peggy_choice_2_43,CHOICE_2_43,
	&ampersand.Rule,
	&exclaim.Rule);
SEQUENCERULE(peggy_seq_2_47,SEQ_2_47,
	&peggy_list_rule.DerivedRule.Rule,
	&peggy_rep_0_1_48.DerivedRule.Rule);
PRODUCTION(peggy_choice,CHOICE,
	&peggy_list_2_54.DerivedRule.Rule,
	simplify_rule);
PRODUCTION(peggy_nonws_printable,NONWS_PRINTABLE,
	&peggy_nonws_printable_re.Rule);
LISTRULE(peggy_list_2_62,LIST_2_62,
	&comma.Rule,
	&peggy_nonws_printable.DerivedRule.Rule);
LITERALRULE(peggy_identifier_re,IDENTIFIER_RE,
	"[A-Za-z0-9_]+");
PRODUCTION(peggy_string_literal,STRING_LITERAL,
	&peggy_string_literal_re.Rule);
LITERALRULE(comma,COMMA,
	",");
PRODUCTION(peggy_special_production,SPECIAL_PRODUCTION,
	&peggy_choice_2_56.ChainRule.Rule);
SEQUENCERULE(peggy_seq_4_64,SEQ_4_64,
	&peggy_identifier.DerivedRule.Rule,
	&peggy_rep_0_1_65.DerivedRule.Rule,
	&colon.Rule,
	&peggy_choice.DerivedRule.Rule);
SEQUENCERULE(peggy_seq_3_58,SEQ_3_58,
	&peggy_choice_2_59.ChainRule.Rule,
	&colon.Rule,
	&peggy_list_2_60.DerivedRule.Rule);
LITERALRULE(keyword_kw,KEYWORD_KW,
	"keyword");
CHOICERULE(peggy_choice_7_30,CHOICE_7_30,
	&peggy_whitespace.DerivedRule.Rule,
	&peggy_string_literal.DerivedRule.Rule,
	&peggy_regex_literal.DerivedRule.Rule,
	&peggy_punctuator.Rule,
	&peggy_keyword.Rule,
	&peggy_identifier.DerivedRule.Rule,
	&peggy_digit_seq.DerivedRule.Rule);
PRODUCTION(peggy_lookahead_rule,LOOKAHEAD_RULE,
	&peggy_seq_2_41.ChainRule.Rule,
	simplify_rule);
REPEATRULE(peggy_rep_1_0_70,REP_1_0_70,
	&peggy_choice_3_71.ChainRule.Rule,
	1);
LITERALRULE(peggy_whitespace_re,WHITESPACE_RE,
	"([ \t\r\n\v\f]+|(//[^\n]*\n)|(/\\*((\\*[^/])|[^*])*\\*/))+");
LITERALRULE(lbrace,LBRACE,
	"\\{");
CHOICERULE(peggy_choice_2_56,CHOICE_2_56,
	&peggy_seq_3_57.ChainRule.Rule,
	&peggy_seq_3_58.ChainRule.Rule);
LITERALRULE(equals,EQUALS,
	"=");
REPEATRULE(peggy_rep_0_1_48,REP_0_1_48,
	&peggy_choice_4_49.ChainRule.Rule,
	0,
	1);
PRODUCTION(peggy_identifier,IDENTIFIER,
	&peggy_identifier_re.Rule);
LITERALRULE(peggy_digit_seq_re,DIGIT_SEQ_RE,
	"[0-9]+");
LITERALRULE(ampersand,AMPERSAND,
	"&");
SEQUENCERULE(peggy_seq_3_68,SEQ_3_68,
	&peggy_identifier.DerivedRule.Rule,
	&equals.Rule,
	&peggy_nonws_printable.DerivedRule.Rule);
SEQUENCERULE(peggy_seq_5_50,SEQ_5_50,
	&lbrace.Rule,
	&peggy_rep_0_1_51.DerivedRule.Rule,
	&colon.Rule,
	&peggy_rep_0_1_51.DerivedRule.Rule,
	&rbrace.Rule);
CHOICERULE(peggy_choice_2_35,CHOICE_2_35,
	&peggy_string_literal.DerivedRule.Rule,
	&peggy_regex_literal.DerivedRule.Rule);
LITERALRULE(period,PERIOD,
	"\\.");
SEQUENCERULE(peggy_seq_3_57,SEQ_3_57,
	&token_kw.Rule,
	&colon.Rule,
	&peggy_choice.DerivedRule.Rule);


Rule * peggyrules[PEGGY_NRULES + 1] = {
	&peggy_list_2_45.DerivedRule.Rule,
	&peggy_string_literal_re.Rule,
	&vbar.Rule,
	&peggy_choice_4_49.ChainRule.Rule,
	&peggy_token.DerivedRule.Rule,
	&peggy_choice_2_59.ChainRule.Rule,
	&peggy_seq_3_66.ChainRule.Rule,
	&peggy_seq_3_38.ChainRule.Rule,
	&peggy_sequence.DerivedRule.Rule,
	&peggy_rep_0_1_42.DerivedRule.Rule,
	&peggy_whitespace.DerivedRule.Rule,
	&rbrace.Rule,
	&peggy_peggy.DerivedRule.Rule,
	&question.Rule,
	&peggy_nonws_printable_re.Rule,
	&peggy_rep_0_1_65.DerivedRule.Rule,
	&lparen.Rule,
	&peggy_list_2_60.DerivedRule.Rule,
	&peggy_punctuator.Rule,
	&peggy_list_2_54.DerivedRule.Rule,
	&peggy_config.DerivedRule.Rule,
	&peggy_list_2_53.DerivedRule.Rule,
	&exclaim.Rule,
	&punctuator_kw.Rule,
	&rparen.Rule,
	&peggy_repeated_rule.DerivedRule.Rule,
	&peggy_rep_0_1_51.DerivedRule.Rule,
	&token_kw.Rule,
	&peggy_digit_seq.DerivedRule.Rule,
	&asterisk.Rule,
	&colon.Rule,
	&peggy_choice_3_71.ChainRule.Rule,
	&peggy_nonterminal.DerivedRule.Rule,
	&peggy_list_rule.DerivedRule.Rule,
	&peggy_regex_literal_re.Rule,
	&peggy_terminal.DerivedRule.Rule,
	&peggy_choice_3_37.ChainRule.Rule,
	&peggy_transform_functions.DerivedRule.Rule,
	&plus.Rule,
	&peggy_regex_literal.DerivedRule.Rule,
	&peggy_base_rule.DerivedRule.Rule,
	&peggy_production.DerivedRule.Rule,
	&peggy_keyword.Rule,
	&peggy_seq_2_41.ChainRule.Rule,
	&peggy_choice_2_43.ChainRule.Rule,
	&peggy_seq_2_47.ChainRule.Rule,
	&peggy_choice.DerivedRule.Rule,
	&peggy_nonws_printable.DerivedRule.Rule,
	&peggy_list_2_62.DerivedRule.Rule,
	&peggy_identifier_re.Rule,
	&peggy_string_literal.DerivedRule.Rule,
	&comma.Rule,
	&peggy_special_production.DerivedRule.Rule,
	&peggy_seq_4_64.ChainRule.Rule,
	&peggy_seq_3_58.ChainRule.Rule,
	&keyword_kw.Rule,
	&peggy_choice_7_30.ChainRule.Rule,
	&peggy_lookahead_rule.DerivedRule.Rule,
	&peggy_rep_1_0_70.DerivedRule.Rule,
	&peggy_whitespace_re.Rule,
	&lbrace.Rule,
	&peggy_choice_2_56.ChainRule.Rule,
	&equals.Rule,
	&peggy_rep_0_1_48.DerivedRule.Rule,
	&peggy_identifier.DerivedRule.Rule,
	&peggy_digit_seq_re.Rule,
	&ampersand.Rule,
	&peggy_seq_3_68.ChainRule.Rule,
	&peggy_seq_5_50.ChainRule.Rule,
	&peggy_choice_2_35.ChainRule.Rule,
	&period.Rule,
	&peggy_seq_3_57.ChainRule.Rule,
	NULL
};

void peggy_dest(void) {
	int i = 0;
	while (peggyrules[i]) {
		peggyrules[i]->_class->dest(peggyrules[i]);
		i++;
	}
}
