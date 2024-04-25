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
LiteralRule peggy_keyword; // KEYWORD
LiteralRule import_kw; // IMPORT_KW
LiteralRule export_kw; // EXPORT_KW
LiteralRule punctuator_kw; // PUNCTUATOR_KW
LiteralRule keyword_kw; // KEYWORD_KW
LiteralRule token_kw; // TOKEN_KW
Production peggy_identifier; // IDENTIFIER
LiteralRule peggy_identifier_re; // IDENTIFIER_RE
Production peggy_token; // TOKEN
ChoiceRule peggy_choice_6_29; // CHOICE_6_29
Production peggy_nonws_printable; // NONWS_PRINTABLE
LiteralRule peggy_nonws_printable_re; // NONWS_PRINTABLE_RE
Production peggy_nonterminal; // NONTERMINAL
Production peggy_terminal; // TERMINAL
ChoiceRule peggy_choice_2_34; // CHOICE_2_34
Production peggy_base_rule; // BASE_RULE
ChoiceRule peggy_choice_3_36; // CHOICE_3_36
SequenceRule peggy_seq_3_37; // SEQ_3_37
Production peggy_lookahead_rule; // LOOKAHEAD_RULE
SequenceRule peggy_seq_2_40; // SEQ_2_40
RepeatRule peggy_rep_0_1_41; // REP_0_1_41
ChoiceRule peggy_choice_2_42; // CHOICE_2_42
Production peggy_list_rule; // LIST_RULE
ListRule peggy_list_2_44; // LIST_2_44
Production peggy_digit_seq; // DIGIT_SEQ
LiteralRule peggy_digit_seq_re; // DIGIT_SEQ_RE
Production peggy_repeated_rule; // REPEATED_RULE
SequenceRule peggy_seq_2_48; // SEQ_2_48
RepeatRule peggy_rep_0_1_49; // REP_0_1_49
ChoiceRule peggy_choice_4_50; // CHOICE_4_50
SequenceRule peggy_seq_5_51; // SEQ_5_51
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
ChoiceRule peggy_choice_2_69; // CHOICE_2_69
Production peggy_peggy; // PEGGY
RepeatRule peggy_rep_1_0_71; // REP_1_0_71
ChoiceRule peggy_choice_3_72; // CHOICE_3_72
CHOICERULE(peggy_choice_3_36,CHOICE_3_36,
	&peggy_terminal.AnonymousProduction.DerivedRule.Rule,
	&peggy_nonterminal.AnonymousProduction.DerivedRule.Rule,
	&peggy_seq_3_37.ChainRule.Rule);
PRODUCTION(peggy_whitespace,WHITESPACE,
	&peggy_whitespace_re.Rule,
	skip_token);
LISTRULE(peggy_list_2_54,LIST_2_54,
	&vbar.Rule,
	&peggy_sequence.AnonymousProduction.DerivedRule.Rule);
SEQUENCERULE(peggy_seq_3_58,SEQ_3_58,
	&peggy_choice_2_59.ChainRule.Rule,
	&colon.Rule,
	&peggy_list_2_60.DerivedRule.Rule);
LITERALRULE(rparen,RPAREN,
	"^(\\))");
CHOICERULE(peggy_choice_2_56,CHOICE_2_56,
	&peggy_seq_3_57.ChainRule.Rule,
	&peggy_seq_3_58.ChainRule.Rule);
PRODUCTION(peggy_identifier,IDENTIFIER,
	&peggy_identifier_re.Rule);
REPEATRULE(peggy_rep_0_1_41,REP_0_1_41,
	&peggy_choice_2_42.ChainRule.Rule,
	0,
	1);
PRODUCTION(peggy_sequence,SEQUENCE,
	&peggy_list_2_53.DerivedRule.Rule,
	simplify_rule);
LITERALRULE(peggy_punctuator,PUNCTUATOR,
	"^((!)|(\\|)|(,)|(\\?)|(\\.)|(&)|(:)|(\\+)|(\\*)|(\\()|(\\))|(\\{)|(}))");
LITERALRULE(asterisk,ASTERISK,
	"^(\\*)");
PRODUCTION(peggy_production,PRODUCTION,
	&peggy_seq_4_64.ChainRule.Rule);
SEQUENCERULE(peggy_seq_3_37,SEQ_3_37,
	&lparen.Rule,
	&peggy_choice.AnonymousProduction.DerivedRule.Rule,
	&rparen.Rule);
PRODUCTION(peggy_base_rule,BASE_RULE,
	&peggy_choice_3_36.ChainRule.Rule);
LITERALRULE(lbrace,LBRACE,
	"^(\\{)");
LISTRULE(peggy_list_2_44,LIST_2_44,
	&period.Rule,
	&peggy_lookahead_rule.AnonymousProduction.DerivedRule.Rule);
LITERALRULE(question,QUESTION,
	"^(\\?)");
LITERALRULE(plus,PLUS,
	"^(\\+)");
LITERALRULE(exclaim,EXCLAIM,
	"^(!)");
LITERALRULE(import_kw,IMPORT_KW,
	"^(import)");
LISTRULE(peggy_list_2_62,LIST_2_62,
	&comma.Rule,
	&peggy_nonws_printable.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_transform_functions,TRANSFORM_FUNCTIONS,
	&peggy_list_2_62.DerivedRule.Rule);
SEQUENCERULE(peggy_seq_5_51,SEQ_5_51,
	&lbrace.Rule,
	&peggy_digit_seq.AnonymousProduction.DerivedRule.Rule,
	&colon.Rule,
	&peggy_digit_seq.AnonymousProduction.DerivedRule.Rule,
	&rbrace.Rule);
LISTRULE(peggy_list_2_60,LIST_2_60,
	&vbar.Rule,
	&peggy_string_literal.AnonymousProduction.DerivedRule.Rule);
LITERALRULE(peggy_digit_seq_re,DIGIT_SEQ_RE,
	"^([0-9]*)");
LITERALRULE(vbar,VBAR,
	"^(\\|)");
LITERALRULE(comma,COMMA,
	"^(,)");
LITERALRULE(peggy_string_literal_re,STRING_LITERAL_RE,
	"^(^'((\\\\)\'|[^\'])*')");
PRODUCTION(peggy_nonterminal,NONTERMINAL,
	&peggy_identifier.AnonymousProduction.DerivedRule.Rule);
LITERALRULE(rbrace,RBRACE,
	"^(})");
LITERALRULE(keyword_kw,KEYWORD_KW,
	"^(keyword)");
PRODUCTION(peggy_repeated_rule,REPEATED_RULE,
	&peggy_seq_2_48.ChainRule.Rule,
	simplify_rule);
LISTRULE(peggy_list_2_53,LIST_2_53,
	&comma.Rule,
	&peggy_repeated_rule.AnonymousProduction.DerivedRule.Rule);
SEQUENCERULE(peggy_seq_3_68,SEQ_3_68,
	&peggy_choice_2_69.ChainRule.Rule,
	&colon.Rule,
	&peggy_nonws_printable.AnonymousProduction.DerivedRule.Rule);
CHOICERULE(peggy_choice_2_42,CHOICE_2_42,
	&ampersand.Rule,
	&exclaim.Rule);
CHOICERULE(peggy_choice_6_29,CHOICE_6_29,
	&peggy_whitespace.AnonymousProduction.DerivedRule.Rule,
	&peggy_string_literal.AnonymousProduction.DerivedRule.Rule,
	&peggy_regex_literal.AnonymousProduction.DerivedRule.Rule,
	&peggy_punctuator.Rule,
	&peggy_keyword.Rule,
	&peggy_identifier.AnonymousProduction.DerivedRule.Rule);
SEQUENCERULE(peggy_seq_2_40,SEQ_2_40,
	&peggy_rep_0_1_41.DerivedRule.Rule,
	&peggy_base_rule.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_choice,CHOICE,
	&peggy_list_2_54.DerivedRule.Rule,
	simplify_rule);
LITERALRULE(peggy_regex_literal_re,REGEX_LITERAL_RE,
	"^(^\"((\\\\\")|[^\"])*\")");
PRODUCTION(peggy_regex_literal,REGEX_LITERAL,
	&peggy_regex_literal_re.Rule);
SEQUENCERULE(peggy_seq_2_48,SEQ_2_48,
	&peggy_list_rule.AnonymousProduction.DerivedRule.Rule,
	&peggy_rep_0_1_49.DerivedRule.Rule);
LITERALRULE(period,PERIOD,
	"^(\\.)");
PRODUCTION(peggy_config,CONFIG,
	&peggy_seq_3_68.ChainRule.Rule);
LITERALRULE(export_kw,EXPORT_KW,
	"^(export)");
SEQUENCERULE(peggy_seq_4_64,SEQ_4_64,
	&peggy_identifier.AnonymousProduction.DerivedRule.Rule,
	&peggy_rep_0_1_65.DerivedRule.Rule,
	&colon.Rule,
	&peggy_choice.AnonymousProduction.DerivedRule.Rule);
REPEATRULE(peggy_rep_0_1_49,REP_0_1_49,
	&peggy_choice_4_50.ChainRule.Rule,
	0,
	1);
CHOICERULE(peggy_choice_4_50,CHOICE_4_50,
	&plus.Rule,
	&asterisk.Rule,
	&question.Rule,
	&peggy_seq_5_51.ChainRule.Rule);
PRODUCTION(peggy_list_rule,LIST_RULE,
	&peggy_list_2_44.DerivedRule.Rule,
	simplify_rule);
PRODUCTION(peggy_terminal,TERMINAL,
	&peggy_choice_2_34.ChainRule.Rule);
LITERALRULE(peggy_identifier_re,IDENTIFIER_RE,
	"^([A-Za-z0-9_]+)");
PRODUCTION(peggy_lookahead_rule,LOOKAHEAD_RULE,
	&peggy_seq_2_40.ChainRule.Rule,
	simplify_rule);
CHOICERULE(peggy_choice_2_59,CHOICE_2_59,
	&punctuator_kw.Rule,
	&keyword_kw.Rule);
LITERALRULE(peggy_keyword,KEYWORD,
	"^((import)|(export)|(punctuator)|(keyword)|(token))");
LITERALRULE(token_kw,TOKEN_KW,
	"^(token)");
PRODUCTION(peggy_nonws_printable,NONWS_PRINTABLE,
	&peggy_nonws_printable_re.Rule);
REPEATRULE(peggy_rep_1_0_71,REP_1_0_71,
	&peggy_choice_3_72.ChainRule.Rule,
	1);
LITERALRULE(colon,COLON,
	"^(:)");
LITERALRULE(ampersand,AMPERSAND,
	"^(&)");
SEQUENCERULE(peggy_seq_3_66,SEQ_3_66,
	&lparen.Rule,
	&peggy_transform_functions.AnonymousProduction.DerivedRule.Rule,
	&rparen.Rule);
CHOICERULE(peggy_choice_2_69,CHOICE_2_69,
	&import_kw.Rule,
	&export_kw.Rule);
CHOICERULE(peggy_choice_2_34,CHOICE_2_34,
	&peggy_string_literal.AnonymousProduction.DerivedRule.Rule,
	&peggy_regex_literal.AnonymousProduction.DerivedRule.Rule);
SEQUENCERULE(peggy_seq_3_57,SEQ_3_57,
	&token_kw.Rule,
	&colon.Rule,
	&peggy_choice.AnonymousProduction.DerivedRule.Rule);
LITERALRULE(punctuator_kw,PUNCTUATOR_KW,
	"^(punctuator)");
CHOICERULE(peggy_choice_3_72,CHOICE_3_72,
	&peggy_config.AnonymousProduction.DerivedRule.Rule,
	&peggy_special_production.AnonymousProduction.DerivedRule.Rule,
	&peggy_production.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(peggy_string_literal,STRING_LITERAL,
	&peggy_string_literal_re.Rule);
PRODUCTION(peggy_digit_seq,DIGIT_SEQ,
	&peggy_digit_seq_re.Rule);
LITERALRULE(peggy_nonws_printable_re,NONWS_PRINTABLE_RE,
	"^([^[:space:]]+)");
PRODUCTION(peggy_special_production,SPECIAL_PRODUCTION,
	&peggy_choice_2_56.ChainRule.Rule);
PRODUCTION(peggy_peggy,PEGGY,
	&peggy_rep_1_0_71.DerivedRule.Rule,
	handle_peggy);
PRODUCTION(peggy_token,TOKEN,
	&peggy_choice_6_29.ChainRule.Rule,
	token_action);
LITERALRULE(lparen,LPAREN,
	"^(\\()");
REPEATRULE(peggy_rep_0_1_65,REP_0_1_65,
	&peggy_seq_3_66.ChainRule.Rule,
	0,
	1);
LITERALRULE(peggy_whitespace_re,WHITESPACE_RE,
	"^(([[:space:]]+|(//[^\n]*\n)|(/\\*((\\*[^/])|[^*])*\\*/))+)");


Rule * peggyrules[PEGGY_NRULES + 1] = {
	&peggy_choice_3_36.ChainRule.Rule,
	&peggy_whitespace.AnonymousProduction.DerivedRule.Rule,
	&peggy_list_2_54.DerivedRule.Rule,
	&peggy_seq_3_58.ChainRule.Rule,
	&rparen.Rule,
	&peggy_choice_2_56.ChainRule.Rule,
	&peggy_identifier.AnonymousProduction.DerivedRule.Rule,
	&peggy_rep_0_1_41.DerivedRule.Rule,
	&peggy_sequence.AnonymousProduction.DerivedRule.Rule,
	&peggy_punctuator.Rule,
	&asterisk.Rule,
	&peggy_production.AnonymousProduction.DerivedRule.Rule,
	&peggy_seq_3_37.ChainRule.Rule,
	&peggy_base_rule.AnonymousProduction.DerivedRule.Rule,
	&lbrace.Rule,
	&peggy_list_2_44.DerivedRule.Rule,
	&question.Rule,
	&plus.Rule,
	&exclaim.Rule,
	&import_kw.Rule,
	&peggy_list_2_62.DerivedRule.Rule,
	&peggy_transform_functions.AnonymousProduction.DerivedRule.Rule,
	&peggy_seq_5_51.ChainRule.Rule,
	&peggy_list_2_60.DerivedRule.Rule,
	&peggy_digit_seq_re.Rule,
	&vbar.Rule,
	&comma.Rule,
	&peggy_string_literal_re.Rule,
	&peggy_nonterminal.AnonymousProduction.DerivedRule.Rule,
	&rbrace.Rule,
	&keyword_kw.Rule,
	&peggy_repeated_rule.AnonymousProduction.DerivedRule.Rule,
	&peggy_list_2_53.DerivedRule.Rule,
	&peggy_seq_3_68.ChainRule.Rule,
	&peggy_choice_2_42.ChainRule.Rule,
	&peggy_choice_6_29.ChainRule.Rule,
	&peggy_seq_2_40.ChainRule.Rule,
	&peggy_choice.AnonymousProduction.DerivedRule.Rule,
	&peggy_regex_literal_re.Rule,
	&peggy_regex_literal.AnonymousProduction.DerivedRule.Rule,
	&peggy_seq_2_48.ChainRule.Rule,
	&period.Rule,
	&peggy_config.AnonymousProduction.DerivedRule.Rule,
	&export_kw.Rule,
	&peggy_seq_4_64.ChainRule.Rule,
	&peggy_rep_0_1_49.DerivedRule.Rule,
	&peggy_choice_4_50.ChainRule.Rule,
	&peggy_list_rule.AnonymousProduction.DerivedRule.Rule,
	&peggy_terminal.AnonymousProduction.DerivedRule.Rule,
	&peggy_identifier_re.Rule,
	&peggy_lookahead_rule.AnonymousProduction.DerivedRule.Rule,
	&peggy_choice_2_59.ChainRule.Rule,
	&peggy_keyword.Rule,
	&token_kw.Rule,
	&peggy_nonws_printable.AnonymousProduction.DerivedRule.Rule,
	&peggy_rep_1_0_71.DerivedRule.Rule,
	&colon.Rule,
	&ampersand.Rule,
	&peggy_seq_3_66.ChainRule.Rule,
	&peggy_choice_2_69.ChainRule.Rule,
	&peggy_choice_2_34.ChainRule.Rule,
	&peggy_seq_3_57.ChainRule.Rule,
	&punctuator_kw.Rule,
	&peggy_choice_3_72.ChainRule.Rule,
	&peggy_string_literal.AnonymousProduction.DerivedRule.Rule,
	&peggy_digit_seq.AnonymousProduction.DerivedRule.Rule,
	&peggy_nonws_printable_re.Rule,
	&peggy_special_production.AnonymousProduction.DerivedRule.Rule,
	&peggy_peggy.AnonymousProduction.DerivedRule.Rule,
	&peggy_token.AnonymousProduction.DerivedRule.Rule,
	&lparen.Rule,
	&peggy_rep_0_1_65.DerivedRule.Rule,
	&peggy_whitespace_re.Rule,
	NULL
};

void peggy_dest(void) {
	int i = 0;
	while (peggyrules[i]) {
		peggyrules[i]->_class->dest(peggyrules[i]);
		i++;
	}
}
