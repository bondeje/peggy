/* this file is auto-generated, do not modify */
#include <peggy/parser_gen.h>
#include "csv.h"
#include "csvparser.h"
LiteralRule csv_punctuator; // PUNCTUATOR
LiteralRule comma; // COMMA
Production csv_csv; // CSV
SequenceRule csv_seq_2_3; // SEQ_2_3
ListRule csv_list_2_4; // LIST_2_4
RepeatRule csv_rep_0_0_7; // REP_0_0_7
ChoiceRule csv_choice_2_8; // CHOICE_2_8
Production csv_token; // TOKEN
ChoiceRule csv_choice_4_11; // CHOICE_4_11
Production csv_record; // RECORD
ListRule csv_list_2_13; // LIST_2_13
Production csv_nonstring_field; // NONSTRING_FIELD
LiteralRule csv_nonstring_field_re; // NONSTRING_FIELD_RE
Production csv_field; // FIELD
ChoiceRule csv_choice_2_16; // CHOICE_2_16
Production csv_whitespace; // WHITESPACE
LiteralRule csv_whitespace_re; // WHITESPACE_RE
Production csv_crlf; // CRLF
LiteralRule csv_crlf_re; // CRLF_RE
Production csv_string; // STRING
LiteralRule csv_string_re; // STRING_RE
PRODUCTION(csv_whitespace,WHITESPACE,
	&csv_whitespace_re.Rule,
	skip_token);
LISTRULE(csv_list_2_4,LIST_2_4,
	&csv_crlf.AnonymousProduction.DerivedRule.Rule,
	&csv_record.AnonymousProduction.DerivedRule.Rule);
PRODUCTION(csv_string,STRING,
	&csv_string_re.Rule);
SEQUENCERULE(csv_seq_2_3,SEQ_2_3,
	&csv_list_2_4.DerivedRule.Rule,
	&csv_rep_0_0_7.DerivedRule.Rule);
PRODUCTION(csv_nonstring_field,NONSTRING_FIELD,
	&csv_nonstring_field_re.Rule);
CHOICERULE(csv_choice_4_11,CHOICE_4_11,
	&csv_whitespace.AnonymousProduction.DerivedRule.Rule,
	&csv_crlf.AnonymousProduction.DerivedRule.Rule,
	&csv_punctuator.Rule,
	&csv_field.AnonymousProduction.DerivedRule.Rule);
CHOICERULE(csv_choice_2_16,CHOICE_2_16,
	&csv_string.AnonymousProduction.DerivedRule.Rule,
	&csv_nonstring_field.AnonymousProduction.DerivedRule.Rule);
LITERALRULE(csv_nonstring_field_re,NONSTRING_FIELD_RE,
	"[^,\r\n]+");
PRODUCTION(csv_csv,CSV,
	&csv_seq_2_3.ChainRule.Rule,
	handle_csv);
PRODUCTION(csv_crlf,CRLF,
	&csv_crlf_re.Rule);
PRODUCTION(csv_token,TOKEN,
	&csv_choice_4_11.ChainRule.Rule,
	token_action);
PRODUCTION(csv_field,FIELD,
	&csv_choice_2_16.ChainRule.Rule);
LITERALRULE(csv_string_re,STRING_RE,
	"\"((\\\\\")|[^\"])*\"");
REPEATRULE(csv_rep_0_0_7,REP_0_0_7,
	&csv_choice_2_8.ChainRule.Rule);
LISTRULE(csv_list_2_13,LIST_2_13,
	&comma.Rule,
	&csv_field.AnonymousProduction.DerivedRule.Rule);
LITERALRULE(comma,COMMA,
	",");
LITERALRULE(csv_crlf_re,CRLF_RE,
	"\r\n");
LITERALRULE(csv_punctuator,PUNCTUATOR,
	",");
LITERALRULE(csv_whitespace_re,WHITESPACE_RE,
	"[ \t\f\v]+");
PRODUCTION(csv_record,RECORD,
	&csv_list_2_13.DerivedRule.Rule);
CHOICERULE(csv_choice_2_8,CHOICE_2_8,
	&csv_crlf.AnonymousProduction.DerivedRule.Rule,
	&csv_whitespace.AnonymousProduction.DerivedRule.Rule);


Rule * csvrules[CSV_NRULES + 1] = {
	&csv_whitespace.AnonymousProduction.DerivedRule.Rule,
	&csv_list_2_4.DerivedRule.Rule,
	&csv_string.AnonymousProduction.DerivedRule.Rule,
	&csv_seq_2_3.ChainRule.Rule,
	&csv_nonstring_field.AnonymousProduction.DerivedRule.Rule,
	&csv_choice_4_11.ChainRule.Rule,
	&csv_choice_2_16.ChainRule.Rule,
	&csv_nonstring_field_re.Rule,
	&csv_csv.AnonymousProduction.DerivedRule.Rule,
	&csv_crlf.AnonymousProduction.DerivedRule.Rule,
	&csv_token.AnonymousProduction.DerivedRule.Rule,
	&csv_field.AnonymousProduction.DerivedRule.Rule,
	&csv_string_re.Rule,
	&csv_rep_0_0_7.DerivedRule.Rule,
	&csv_list_2_13.DerivedRule.Rule,
	&comma.Rule,
	&csv_crlf_re.Rule,
	&csv_punctuator.Rule,
	&csv_whitespace_re.Rule,
	&csv_record.AnonymousProduction.DerivedRule.Rule,
	&csv_choice_2_8.ChainRule.Rule,
	NULL
};

void csv_dest(void) {
	int i = 0;
	while (csvrules[i]) {
		csvrules[i]->_class->dest(csvrules[i]);
		i++;
	}
}
