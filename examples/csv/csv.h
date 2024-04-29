/* this file is auto-generated, do not modify */
#ifndef CSV_H
#define CSV_H

#include <peggy/utils.h>
#include <peggy/rule.h>
#include <peggy/parser.h>

typedef enum csvrule {
	PUNCTUATOR,
	COMMA,
	CSV,
	SEQ_2_3,
	LIST_2_4,
	REP_0_0_7,
	CHOICE_2_8,
	TOKEN,
	CHOICE_4_11,
	RECORD,
	LIST_2_13,
	NONSTRING_FIELD,
	NONSTRING_FIELD_RE,
	FIELD,
	CHOICE_2_16,
	WHITESPACE,
	WHITESPACE_RE,
	CRLF,
	CRLF_RE,
	STRING,
	STRING_RE,
	CSV_NRULES
} csvrule;

extern Production csv_token;
extern Production csv_csv;

void csv_dest(void);

#endif //CSV_H
