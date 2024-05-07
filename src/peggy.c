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
Production peggy_digit_seq; // DIGIT_SEQ
LiteralRule peggy_digit_seq_re; // DIGIT_SEQ_RE
LiteralRule peggy_keyword; // KEYWORD
LiteralRule import_kw; // IMPORT_KW
LiteralRule export_kw; // EXPORT_KW
LiteralRule punctuator_kw; // PUNCTUATOR_KW
LiteralRule keyword_kw; // KEYWORD_KW
LiteralRule token_kw; // TOKEN_KW
Production peggy_identifier; // IDENTIFIER
LiteralRule peggy_identifier_re; // IDENTIFIER_RE
Production peggy_token; // TOKEN
ChoiceRule peggy_choice_7_31; // CHOICE_7_31
Production peggy_nonws_printable; // NONWS_PRINTABLE
LiteralRule peggy_nonws_printable_re; // NONWS_PRINTABLE_RE
Production peggy_nonterminal; // NONTERMINAL
Production peggy_terminal; // TERMINAL
ChoiceRule peggy_choice_2_36; // CHOICE_2_36
Production peggy_base_rule; // BASE_RULE
ChoiceRule peggy_choice_3_38; // CHOICE_3_38
SequenceRule peggy_seq_3_39; // SEQ_3_39
Production peggy_lookahead_rule; // LOOKAHEAD_RULE
SequenceRule peggy_seq_2_42; // SEQ_2_42
RepeatRule peggy_rep_0_1_43; // REP_0_1_43
ChoiceRule peggy_choice_2_44; // CHOICE_2_44
Production peggy_list_rule; // LIST_RULE
ListRule peggy_list_2_46; // LIST_2_46
Production peggy_repeated_rule; // REPEATED_RULE
SequenceRule peggy_seq_2_48; // SEQ_2_48
RepeatRule peggy_rep_0_1_49; // REP_0_1_49
ChoiceRule peggy_choice_4_50; // CHOICE_4_50
SequenceRule peggy_seq_5_51; // SEQ_5_51
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
ChoiceRule peggy_choice_2_70; // CHOICE_2_70
Production peggy_peggy; // PEGGY
RepeatRule peggy_rep_1_0_72; // REP_1_0_72
ChoiceRule peggy_choice_3_73; // CHOICE_3_73
LISTRULE(peggy_list_2_46,LIST_2_46,
	&period.Rule,
	&peggy_lookahead_rule.DerivedRule.Rule);
LITERALRULE(peggy_string_literal_re,STRING_LITERAL_RE,
	"'((\\\\)\'|[^\'])*'");
LITERALRULE(vbar,VBAR,
	"\\|");
CHOICERULE(peggy_choice_4_50,CHOICE_4_50,
	&plus.Rule,
	&asterisk.Rule,
	&question.Rule,
	&peggy_seq_5_51.ChainRule.Rule);
PRODUCTION(peggy_token,TOKEN,
	&peggy_choice_7_31.ChainRule.Rule,
	token_action);
CHOICERULE(peggy_choice_2_60,CHOICE_2_60,
	&punctuator_kw.Rule,
	&keyword_kw.Rule);
SEQUENCERULE(peggy_seq_3_67,SEQ_3_67,
	&lparen.Rule,
	&peggy_transform_functions.DerivedRule.Rule,
	&rparen.Rule);
SEQUENCERULE(peggy_seq_3_39,SEQ_3_39,
	&lparen.Rule,
	&peggy_choice.DerivedRule.Rule,
	&rparen.Rule);
PRODUCTION(peggy_sequence,SEQUENCE,
	&peggy_list_2_54.DerivedRule.Rule,
	simplify_rule);
REPEATRULE(peggy_rep_0_1_43,REP_0_1_43,
	&peggy_choice_2_44.ChainRule.Rule,
	0,
	1);
PRODUCTION(peggy_whitespace,WHITESPACE,
	&peggy_whitespace_re.Rule,
	skip_token);
LITERALRULE(rbrace,RBRACE,
	"}");
PRODUCTION(peggy_peggy,PEGGY,
	&peggy_rep_1_0_72.DerivedRule.Rule,
	handle_peggy);
LITERALRULE(question,QUESTION,
	"\\?");
LITERALRULE(peggy_nonws_printable_re,NONWS_PRINTABLE_RE,
	"[^ \t\r\n\v\f]+");
REPEATRULE(peggy_rep_0_1_66,REP_0_1_66,
	&peggy_seq_3_67.ChainRule.Rule,
	0,
	1);
LITERALRULE(lparen,LPAREN,
	"\\(");
LISTRULE(peggy_list_2_61,LIST_2_61,
	&vbar.Rule,
	&peggy_string_literal.DerivedRule.Rule);
LITERALRULE(peggy_punctuator,PUNCTUATOR,
	"!|\\||,|\\?|\\.|&|:|\\+|\\*|\\(|\\)|\\{|}");
LISTRULE(peggy_list_2_55,LIST_2_55,
	&vbar.Rule,
	&peggy_sequence.DerivedRule.Rule);
PRODUCTION(peggy_config,CONFIG,
	&peggy_seq_3_69.ChainRule.Rule);
LISTRULE(peggy_list_2_54,LIST_2_54,
	&comma.Rule,
	&peggy_repeated_rule.DerivedRule.Rule);
LITERALRULE(exclaim,EXCLAIM,
	"!");
LITERALRULE(punctuator_kw,PUNCTUATOR_KW,
	"punctuator");
LITERALRULE(rparen,RPAREN,
	"\\)");
PRODUCTION(peggy_repeated_rule,REPEATED_RULE,
	&peggy_seq_2_48.ChainRule.Rule,
	simplify_rule);
REPEATRULE(peggy_rep_0_1_52,REP_0_1_52,
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
CHOICERULE(peggy_choice_3_73,CHOICE_3_73,
	&peggy_config.DerivedRule.Rule,
	&peggy_special_production.DerivedRule.Rule,
	&peggy_production.DerivedRule.Rule);
PRODUCTION(peggy_nonterminal,NONTERMINAL,
	&peggy_identifier.DerivedRule.Rule);
LITERALRULE(export_kw,EXPORT_KW,
	"export");
CHOICERULE(peggy_choice_2_70,CHOICE_2_70,
	&import_kw.Rule,
	&export_kw.Rule);
PRODUCTION(peggy_list_rule,LIST_RULE,
	&peggy_list_2_46.DerivedRule.Rule,
	simplify_rule);
LITERALRULE(peggy_regex_literal_re,REGEX_LITERAL_RE,
	"\"((\\\\\")|[^\"])*\"");
PRODUCTION(peggy_terminal,TERMINAL,
	&peggy_choice_2_36.ChainRule.Rule);
LITERALRULE(import_kw,IMPORT_KW,
	"import");
CHOICERULE(peggy_choice_3_38,CHOICE_3_38,
	&peggy_terminal.DerivedRule.Rule,
	&peggy_nonterminal.DerivedRule.Rule,
	&peggy_seq_3_39.ChainRule.Rule);
PRODUCTION(peggy_transform_functions,TRANSFORM_FUNCTIONS,
	&peggy_list_2_63.DerivedRule.Rule);
LITERALRULE(plus,PLUS,
	"\\+");
PRODUCTION(peggy_regex_literal,REGEX_LITERAL,
	&peggy_regex_literal_re.Rule);
PRODUCTION(peggy_base_rule,BASE_RULE,
	&peggy_choice_3_38.ChainRule.Rule);
PRODUCTION(peggy_production,PRODUCTION,
	&peggy_seq_4_65.ChainRule.Rule);
LITERALRULE(peggy_keyword,KEYWORD,
	"import|export|punctuator|keyword|token");
SEQUENCERULE(peggy_seq_2_42,SEQ_2_42,
	&peggy_rep_0_1_43.DerivedRule.Rule,
	&peggy_base_rule.DerivedRule.Rule);
CHOICERULE(peggy_choice_2_44,CHOICE_2_44,
	&ampersand.Rule,
	&exclaim.Rule);
SEQUENCERULE(peggy_seq_2_48,SEQ_2_48,
	&peggy_list_rule.DerivedRule.Rule,
	&peggy_rep_0_1_49.DerivedRule.Rule);
PRODUCTION(peggy_choice,CHOICE,
	&peggy_list_2_55.DerivedRule.Rule,
	simplify_rule);
PRODUCTION(peggy_nonws_printable,NONWS_PRINTABLE,
	&peggy_nonws_printable_re.Rule);
LISTRULE(peggy_list_2_63,LIST_2_63,
	&comma.Rule,
	&peggy_nonws_printable.DerivedRule.Rule);
LITERALRULE(peggy_identifier_re,IDENTIFIER_RE,
	"[A-Za-z0-9_]+");
PRODUCTION(peggy_string_literal,STRING_LITERAL,
	&peggy_string_literal_re.Rule);
LITERALRULE(comma,COMMA,
	",");
SEQUENCERULE(peggy_seq_3_69,SEQ_3_69,
	&peggy_choice_2_70.ChainRule.Rule,
	&colon.Rule,
	&peggy_nonws_printable.DerivedRule.Rule);
PRODUCTION(peggy_special_production,SPECIAL_PRODUCTION,
	&peggy_choice_2_57.ChainRule.Rule);
SEQUENCERULE(peggy_seq_4_65,SEQ_4_65,
	&peggy_identifier.DerivedRule.Rule,
	&peggy_rep_0_1_66.DerivedRule.Rule,
	&colon.Rule,
	&peggy_choice.DerivedRule.Rule);
SEQUENCERULE(peggy_seq_3_59,SEQ_3_59,
	&peggy_choice_2_60.ChainRule.Rule,
	&colon.Rule,
	&peggy_list_2_61.DerivedRule.Rule);
LITERALRULE(keyword_kw,KEYWORD_KW,
	"keyword");
CHOICERULE(peggy_choice_7_31,CHOICE_7_31,
	&peggy_whitespace.DerivedRule.Rule,
	&peggy_string_literal.DerivedRule.Rule,
	&peggy_regex_literal.DerivedRule.Rule,
	&peggy_punctuator.Rule,
	&peggy_keyword.Rule,
	&peggy_identifier.DerivedRule.Rule,
	&peggy_digit_seq.DerivedRule.Rule);
PRODUCTION(peggy_lookahead_rule,LOOKAHEAD_RULE,
	&peggy_seq_2_42.ChainRule.Rule,
	simplify_rule);
REPEATRULE(peggy_rep_1_0_72,REP_1_0_72,
	&peggy_choice_3_73.ChainRule.Rule,
	1);
LITERALRULE(peggy_whitespace_re,WHITESPACE_RE,
	"([ \t\r\n\v\f]+|(//[^\n]*\n)|(/\\*((\\*[^/])|[^*])*\\*/))+");
LITERALRULE(lbrace,LBRACE,
	"\\{");
CHOICERULE(peggy_choice_2_57,CHOICE_2_57,
	&peggy_seq_3_58.ChainRule.Rule,
	&peggy_seq_3_59.ChainRule.Rule);
REPEATRULE(peggy_rep_0_1_49,REP_0_1_49,
	&peggy_choice_4_50.ChainRule.Rule,
	0,
	1);
PRODUCTION(peggy_identifier,IDENTIFIER,
	&peggy_identifier_re.Rule);
LITERALRULE(peggy_digit_seq_re,DIGIT_SEQ_RE,
	"[0-9]+");
LITERALRULE(ampersand,AMPERSAND,
	"&");
SEQUENCERULE(peggy_seq_5_51,SEQ_5_51,
	&lbrace.Rule,
	&peggy_rep_0_1_52.DerivedRule.Rule,
	&colon.Rule,
	&peggy_rep_0_1_52.DerivedRule.Rule,
	&rbrace.Rule);
CHOICERULE(peggy_choice_2_36,CHOICE_2_36,
	&peggy_string_literal.DerivedRule.Rule,
	&peggy_regex_literal.DerivedRule.Rule);
LITERALRULE(period,PERIOD,
	"\\.");
SEQUENCERULE(peggy_seq_3_58,SEQ_3_58,
	&token_kw.Rule,
	&colon.Rule,
	&peggy_choice.DerivedRule.Rule);


Rule * peggyrules[PEGGY_NRULES + 1] = {
	&peggy_list_2_46.DerivedRule.Rule,
	&peggy_string_literal_re.Rule,
	&vbar.Rule,
	&peggy_choice_4_50.ChainRule.Rule,
	&peggy_token.DerivedRule.Rule,
	&peggy_choice_2_60.ChainRule.Rule,
	&peggy_seq_3_67.ChainRule.Rule,
	&peggy_seq_3_39.ChainRule.Rule,
	&peggy_sequence.DerivedRule.Rule,
	&peggy_rep_0_1_43.DerivedRule.Rule,
	&peggy_whitespace.DerivedRule.Rule,
	&rbrace.Rule,
	&peggy_peggy.DerivedRule.Rule,
	&question.Rule,
	&peggy_nonws_printable_re.Rule,
	&peggy_rep_0_1_66.DerivedRule.Rule,
	&lparen.Rule,
	&peggy_list_2_61.DerivedRule.Rule,
	&peggy_punctuator.Rule,
	&peggy_list_2_55.DerivedRule.Rule,
	&peggy_config.DerivedRule.Rule,
	&peggy_list_2_54.DerivedRule.Rule,
	&exclaim.Rule,
	&punctuator_kw.Rule,
	&rparen.Rule,
	&peggy_repeated_rule.DerivedRule.Rule,
	&peggy_rep_0_1_52.DerivedRule.Rule,
	&token_kw.Rule,
	&peggy_digit_seq.DerivedRule.Rule,
	&asterisk.Rule,
	&colon.Rule,
	&peggy_choice_3_73.ChainRule.Rule,
	&peggy_nonterminal.DerivedRule.Rule,
	&export_kw.Rule,
	&peggy_choice_2_70.ChainRule.Rule,
	&peggy_list_rule.DerivedRule.Rule,
	&peggy_regex_literal_re.Rule,
	&peggy_terminal.DerivedRule.Rule,
	&import_kw.Rule,
	&peggy_choice_3_38.ChainRule.Rule,
	&peggy_transform_functions.DerivedRule.Rule,
	&plus.Rule,
	&peggy_regex_literal.DerivedRule.Rule,
	&peggy_base_rule.DerivedRule.Rule,
	&peggy_production.DerivedRule.Rule,
	&peggy_keyword.Rule,
	&peggy_seq_2_42.ChainRule.Rule,
	&peggy_choice_2_44.ChainRule.Rule,
	&peggy_seq_2_48.ChainRule.Rule,
	&peggy_choice.DerivedRule.Rule,
	&peggy_nonws_printable.DerivedRule.Rule,
	&peggy_list_2_63.DerivedRule.Rule,
	&peggy_identifier_re.Rule,
	&peggy_string_literal.DerivedRule.Rule,
	&comma.Rule,
	&peggy_seq_3_69.ChainRule.Rule,
	&peggy_special_production.DerivedRule.Rule,
	&peggy_seq_4_65.ChainRule.Rule,
	&peggy_seq_3_59.ChainRule.Rule,
	&keyword_kw.Rule,
	&peggy_choice_7_31.ChainRule.Rule,
	&peggy_lookahead_rule.DerivedRule.Rule,
	&peggy_rep_1_0_72.DerivedRule.Rule,
	&peggy_whitespace_re.Rule,
	&lbrace.Rule,
	&peggy_choice_2_57.ChainRule.Rule,
	&peggy_rep_0_1_49.DerivedRule.Rule,
	&peggy_identifier.DerivedRule.Rule,
	&peggy_digit_seq_re.Rule,
	&ampersand.Rule,
	&peggy_seq_5_51.ChainRule.Rule,
	&peggy_choice_2_36.ChainRule.Rule,
	&period.Rule,
	&peggy_seq_3_58.ChainRule.Rule,
	NULL
};

void peggy_dest(void) {
	int i = 0;
	while (peggyrules[i]) {
		peggyrules[i]->_class->dest(peggyrules[i]);
		i++;
	}
}
