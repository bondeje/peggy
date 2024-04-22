#ifndef PEGGYPARSER_H
#define PEGGYPARSER_H

#include <peggy/utils.h>
#include <peggy/rule.h>
#include <peggy/parser.h>

typedef enum peggyrule {
    EXCLAIM, // = 0
    AMPERSAND,
    LPAREN,
    RPAREN,
    ASTERISK,
    PLUS,
    COMMA,
    PERIOD,
    COLON,
    QUESTION,
    LBRACE,// = 10
    VBAR,
    RBRACE,
    PUNCTUATOR,
    IMPORT_KW,
    EXPORT_KW,
    PUNCTUATOR_KW,
    KEYWORD_KW,
    TOKEN_KW,
    ROOT_KW,
    KEYWORD,// = 20
    WHITESPACE_RE,
    WHITESPACE,
    NONWS_PRINTABLE_RE,
    NONWS_PRINTABLE,
    IDENTIFIER_RE,
    IDENTIFIER,
    NONTERMINAL,
    REGEX_LITERAL_RE,
    REGEX_LITERAL,
    STRING_LITERAL_RE,// = 30
    STRING_LITERAL,
    CHOICE_LITERALS,
    TERMINAL,
    CHOICE_EXPRESSION,
    CHOICE_TERM_NONTERM_CHOICE_EXPR,
    BASE_RULE,
    CHOICE_POSI_NEGA,
    OPT_CHOICE_POSI_NEGA,
    OPT_CHOICE_POSI_NEGA_BASE,
    LOOKAHEAD_RULE,// = 40
    LIST_PERIOD_LOOKAHEAD,
    LIST_RULE,
    DIGIT_SEQ,
    REPEAT_PARAMS,
    REPEAT_OPS,
    OPT_REPEAT_OPS,
    OPT_REPEAT_OPS_LIST,
    REPEATED_RULE,
    LIST_REPEATED,
    SEQUENCE, // = 50
    LIST_SEQUENCE,
    CHOICE,
    CHOICE_TOKENS,
    TOKEN,
    ROOT_OR_TOKEN,
    SEQ_ROOT_OR_TOKEN,
    PUNC_OR_KW,
    LIST_STRING,
    SEQ_PUNC_OR_KW,
    CHOICE_SPEC_PROD,// = 60
    SPECIAL_PRODUCTION,
    LIST_NONWS_PRINT,
    TRANSFORM_FUNCTIONS,
    TRANSFORM_FUNC_EXPR,
    OPT_TRANSFORM_FUNC_EXPR,
    SEQ_PROD_RULES,
    PRODUCTION,
    IMPORT_EXPORT,
    SEQ_CONFIG,
    CONFIG, // = 70
    ENTRY,
    REP_ENTRY,
    PEGGY,
    NRULES // should be last element in enum
} peggyrule;

extern char const * peggyrule_names[NRULES];
//extern Rule * peggyrules[NRULES]; // probably don't need

extern Production peggy_token;
extern Production peggy_peggy;

void peggy_dest(void);

#endif // PEGGYPARSER_H
