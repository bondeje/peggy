/* this file is auto-generated, do not modify */
#ifndef RE_H
#define RE_H

#include <peggy/utils.h>
#include <peggy/rule.h>
#include <peggy/parser.h>

typedef enum rerule {
	PUNCTUATOR,
	ASTERISK,
	VBAR,
	PLUS,
	PERIOD,
	CARET,
	QUESTION,
	LPAREN,
	RPAREN,
	LBRACKET,
	RBRACKET,
	LBRACE,
	RBRACE,
	MINUS,
	COMMA,
	CHARACTER,
	TOKEN,
	ESCAPE,
	ESCAPE_RE,
	ESCAPED_CHARACTER,
	SEQ_2_20,
	SINGLE_CHAR,
	CHOICE_2_22,
	SYMBOL,
	CHOICE_2_24,
	ELEMENT,
	CHOICE_2_27,
	SEQ_3_28,
	CHAR_CLASS,
	SEQ_4_30,
	REP_0_1_31,
	CHOICE_2_32,
	RANGE,
	SEQ_3_34,
	CHOICE,
	LIST_2_35,
	DIGIT_SEQ,
	DIGIT_SEQ_RE,
	SEQUENCE,
	REP_1_0_39,
	REPEATED_RULE,
	SEQ_2_41,
	REP_0_1_42,
	CHOICE_4_43,
	SEQ_5_44,
	REP_0_1_45,
	REP_0_1_46,
	RE,
	RE_NRULES
} rerule;

extern Production re_token;
extern Production re_re;

void re_dest(void);

#endif //RE_H
