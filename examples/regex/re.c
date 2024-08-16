/* this file is auto-generated, do not modify */
#include <peggy/parser_gen.h>
#include "re.h"
#include "reparser.h"
LiteralRule re_punctuator; // PUNCTUATOR
LiteralRule asterisk; // ASTERISK
LiteralRule vbar; // VBAR
LiteralRule plus; // PLUS
LiteralRule period; // PERIOD
LiteralRule caret; // CARET
LiteralRule question; // QUESTION
LiteralRule lparen; // LPAREN
LiteralRule rparen; // RPAREN
LiteralRule lbracket; // LBRACKET
LiteralRule rbracket; // RBRACKET
LiteralRule lbrace; // LBRACE
LiteralRule rbrace; // RBRACE
LiteralRule minus; // MINUS
LiteralRule comma; // COMMA
Production re_character; // CHARACTER
Production re_token; // TOKEN
Production re_escape; // ESCAPE
LiteralRule re_escape_re; // ESCAPE_RE
Production re_escaped_character; // ESCAPED_CHARACTER
SequenceRule re_seq_2_20; // SEQ_2_20
Production re_single_char; // SINGLE_CHAR
ChoiceRule re_choice_2_22; // CHOICE_2_22
Production re_symbol; // SYMBOL
ChoiceRule re_choice_2_24; // CHOICE_2_24
Production re_element; // ELEMENT
ChoiceRule re_choice_2_27; // CHOICE_2_27
SequenceRule re_seq_3_28; // SEQ_3_28
Production re_char_class; // CHAR_CLASS
SequenceRule re_seq_4_30; // SEQ_4_30
RepeatRule re_rep_0_1_31; // REP_0_1_31
ChoiceRule re_choice_2_32; // CHOICE_2_32
Production re_range; // RANGE
SequenceRule re_seq_3_34; // SEQ_3_34
Production re_choice; // CHOICE
ListRule re_list_2_35; // LIST_2_35
Production re_digit_seq; // DIGIT_SEQ
LiteralRule re_digit_seq_re; // DIGIT_SEQ_RE
Production re_sequence; // SEQUENCE
RepeatRule re_rep_1_0_39; // REP_1_0_39
Production re_repeated_rule; // REPEATED_RULE
SequenceRule re_seq_2_41; // SEQ_2_41
RepeatRule re_rep_0_1_42; // REP_0_1_42
ChoiceRule re_choice_4_43; // CHOICE_4_43
SequenceRule re_seq_5_44; // SEQ_5_44
RepeatRule re_rep_0_1_45; // REP_0_1_45
RepeatRule re_rep_0_1_46; // REP_0_1_46
Production re_re; // RE
LITERALRULE(re_digit_seq_re,DIGIT_SEQ_RE,
	"[0-9]+");
SEQUENCERULE(re_seq_2_20,SEQ_2_20,
	(Rule *)&re_escape,
	(Rule *)&re_single_char);
CHOICERULE(re_choice_4_43,CHOICE_4_43,
	(Rule *)&plus,
	(Rule *)&asterisk,
	(Rule *)&question,
	(Rule *)&re_seq_5_44);
LITERALRULE(re_escape_re,ESCAPE_RE,
	"\\");
CHOICERULE(re_choice_2_27,CHOICE_2_27,
	(Rule *)&re_seq_3_28,
	(Rule *)&re_symbol);
LITERALRULE(asterisk,ASTERISK,
	"\\*");
PRODUCTION(re_character,CHARACTER,
	(Rule *)&period);
PRODUCTION(re_range,RANGE,
	(Rule *)&re_seq_3_34);
PRODUCTION(re_repeated_rule,REPEATED_RULE,
	(Rule *)&re_seq_2_41,
	build_repeat_states);
LITERALRULE(period,PERIOD,
	"\\.");
LITERALRULE(lbrace,LBRACE,
	"\\{");
REPEATRULE(re_rep_0_1_42,REP_0_1_42,
	(Rule *)&re_choice_4_43,
	0,
	1);
PRODUCTION(re_sequence,SEQUENCE,
	(Rule *)&re_rep_1_0_39,
	build_sequence_states);
REPEATRULE(re_rep_1_0_39,REP_1_0_39,
	(Rule *)&re_repeated_rule,
	1);
SEQUENCERULE(re_seq_4_30,SEQ_4_30,
	(Rule *)&lbracket,
	(Rule *)&re_rep_0_1_31,
	(Rule *)&re_choice_2_32,
	(Rule *)&rbracket);
PRODUCTION(re_symbol,SYMBOL,
	(Rule *)&re_choice_2_24,
	build_symbol);
CHOICERULE(re_choice_2_32,CHOICE_2_32,
	(Rule *)&re_range,
	(Rule *)&re_single_char);
LITERALRULE(rbracket,RBRACKET,
	"]");
LITERALRULE(plus,PLUS,
	"\\+");
PRODUCTION(re_char_class,CHAR_CLASS,
	(Rule *)&re_seq_4_30,
	build_char_class);
PRODUCTION(re_re,RE,
	(Rule *)&re_choice);
LITERALRULE(vbar,VBAR,
	"\\|");
PRODUCTION(re_token,TOKEN,
	(Rule *)&re_character,
	token_action);
SEQUENCERULE(re_seq_3_28,SEQ_3_28,
	(Rule *)&lparen,
	(Rule *)&re_choice,
	(Rule *)&rparen);
SEQUENCERULE(re_seq_2_41,SEQ_2_41,
	(Rule *)&re_element,
	(Rule *)&re_rep_0_1_42);
SEQUENCERULE(re_seq_3_34,SEQ_3_34,
	(Rule *)&re_character,
	(Rule *)&minus,
	(Rule *)&re_character);
PRODUCTION(re_choice,CHOICE,
	(Rule *)&re_list_2_35,
	build_choice_states);
REPEATRULE(re_rep_0_1_31,REP_0_1_31,
	(Rule *)&caret,
	0,
	1);
SEQUENCERULE(re_seq_5_44,SEQ_5_44,
	(Rule *)&lbrace,
	(Rule *)&re_rep_0_1_45,
	(Rule *)&re_rep_0_1_46,
	(Rule *)&re_rep_0_1_45,
	(Rule *)&rbrace);
LITERALRULE(question,QUESTION,
	"\\?");
LITERALRULE(lparen,LPAREN,
	"\\(");
LISTRULE(re_list_2_35,LIST_2_35,
	(Rule *)&vbar,
	(Rule *)&re_sequence);
LITERALRULE(caret,CARET,
	"\\^");
LITERALRULE(comma,COMMA,
	",");
PRODUCTION(re_single_char,SINGLE_CHAR,
	(Rule *)&re_choice_2_22);
PRODUCTION(re_escape,ESCAPE,
	(Rule *)&re_escape_re);
LITERALRULE(rbrace,RBRACE,
	"}");
PRODUCTION(re_digit_seq,DIGIT_SEQ,
	(Rule *)&re_digit_seq_re);
LITERALRULE(re_punctuator,PUNCTUATOR,
	"\\*|\\||\\+|\\.|\\^|\\?|\\(|\\)|\\[|]|\\{|}|-|,");
REPEATRULE(re_rep_0_1_46,REP_0_1_46,
	(Rule *)&comma,
	0,
	1);
LITERALRULE(lbracket,LBRACKET,
	"\\[");
LITERALRULE(rparen,RPAREN,
	"\\)");
CHOICERULE(re_choice_2_24,CHOICE_2_24,
	(Rule *)&re_char_class,
	(Rule *)&re_single_char);
CHOICERULE(re_choice_2_22,CHOICE_2_22,
	(Rule *)&re_escaped_character,
	(Rule *)&re_character);
LITERALRULE(minus,MINUS,
	"-");
PRODUCTION(re_element,ELEMENT,
	(Rule *)&re_choice_2_27);
PRODUCTION(re_escaped_character,ESCAPED_CHARACTER,
	(Rule *)&re_seq_2_20);
REPEATRULE(re_rep_0_1_45,REP_0_1_45,
	(Rule *)&re_digit_seq,
	0,
	1);


Rule * rerules[RE_NRULES + 1] = {
	(Rule *)&re_digit_seq_re,
	(Rule *)&re_seq_2_20,
	(Rule *)&re_choice_4_43,
	(Rule *)&re_escape_re,
	(Rule *)&re_choice_2_27,
	(Rule *)&asterisk,
	(Rule *)&re_character,
	(Rule *)&re_range,
	(Rule *)&re_repeated_rule,
	(Rule *)&period,
	(Rule *)&lbrace,
	(Rule *)&re_rep_0_1_42,
	(Rule *)&re_sequence,
	(Rule *)&re_rep_1_0_39,
	(Rule *)&re_seq_4_30,
	(Rule *)&re_symbol,
	(Rule *)&re_choice_2_32,
	(Rule *)&rbracket,
	(Rule *)&plus,
	(Rule *)&re_char_class,
	(Rule *)&re_re,
	(Rule *)&vbar,
	(Rule *)&re_token,
	(Rule *)&re_seq_3_28,
	(Rule *)&re_seq_2_41,
	(Rule *)&re_seq_3_34,
	(Rule *)&re_choice,
	(Rule *)&re_rep_0_1_31,
	(Rule *)&re_seq_5_44,
	(Rule *)&question,
	(Rule *)&lparen,
	(Rule *)&re_list_2_35,
	(Rule *)&caret,
	(Rule *)&comma,
	(Rule *)&re_single_char,
	(Rule *)&re_escape,
	(Rule *)&rbrace,
	(Rule *)&re_digit_seq,
	(Rule *)&re_punctuator,
	(Rule *)&re_rep_0_1_46,
	(Rule *)&lbracket,
	(Rule *)&rparen,
	(Rule *)&re_choice_2_24,
	(Rule *)&re_choice_2_22,
	(Rule *)&minus,
	(Rule *)&re_element,
	(Rule *)&re_escaped_character,
	(Rule *)&re_rep_0_1_45,
	NULL
};

void re_dest(void) {
	int i = 0;
	while (rerules[i]) {
		rerules[i]->_class->dest(rerules[i]);
		i++;
	}
}
