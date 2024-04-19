#ifndef IPV4PARSER_H
#define IPV4PARSER_H

#include <peggy/utils.h>
#include <peggy/rule.h>
#include <peggy/parser.h>

typedef enum ipv4rule {
    PERIOD,
    OCTET,
    LIST_OCTET,
    IPV4,
    CHOICE_OCTET_PERIOD,
    TOKEN,
    NRULES
} ipv4rule;

extern char const * ipv4rule_names[NRULES];
extern Rule * ipv4rules[NRULES];

extern LiteralRule ipv4_octet;
extern LiteralRule ipv4_period;
extern Production ipv4_token;
extern Production ipv4_ipv4;

err_type ipv4_init(void);
void ipv4_clear_token(Token * tok);
void ipv4_dest(void);

#endif
