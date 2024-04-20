#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <peggy/utils.h>
#include <peggy/rule.h>
#include <peggy/parser.h>

typedef enum csvrule {
    PUNCTUATOR_RE,
    PUNCTUATOR,
    CRLF_RE,
    CRLF,
    WHITESPACE_RE,
    WHITESPACE,
    CHOICE_CRLF_WHITESPACE,
    REPEAT_CHOICE_CRLF_WHITESPACE,
    STRING_RE,
    STRING,
    NONSTRING_FIELD_RE,
    NONSTRING_FIELD,
    CHOICE_STRING_NONSTRING_FIELD,
    FIELD,
    CHOICE_CRLF_PUNCTUATOR_WHITESPACE_FIELD,
    TOKEN,
    LIST_FIELD,
    RECORD,
    LIST_CRLF_RECORD,
    SEQUENCE_LIST_CRLF_RECORD_REPEAT_CHOICE_CRLF_WHITESPACE,
    CSV,
    NRULES // should be last element in enum
} csvrule;

extern char const * csvrule_names[NRULES];
//extern Rule * csvrules[NRULES]; // probably don't need

extern Production csv_token;
extern Production csv_csv;

void csv_dest(void);

#endif // CSVPARSER_H
