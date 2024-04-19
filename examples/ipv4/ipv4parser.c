#include "ipv4parser.h"

char const * ipv4rule_names[NRULES] = {
    "period",
    "octet", 
    "list_octet", 
    "ipv4", 
    "choice_octet_period",
    "token"
};

LiteralRule ipv4_octet = {._class = &LiteralRule_class, \
                            .Rule = { \
                                ._class = &(LiteralRule_class.Rule_class), \
                                .id = OCTET, \
                                .cache_ = DEFAULT_PACKRATCACHE \
                            }, \
                            .regex_s = "^((2([0-4][0-9]|5[0-5]))|(1[0-9][0-9])|([0-9][0-9])|[0-9])", \
                            .compiled = false
                            };

LiteralRule ipv4_period = {._class = &LiteralRule_class, \
                            .Rule = { \
                                ._class = &(LiteralRule_class.Rule_class), \
                                .id = PERIOD, \
                                .cache_ = DEFAULT_PACKRATCACHE \
                            }, \
                            .regex_s = "^\\.", \
                            .compiled = false
                            };
ChoiceRule ipv4_choice_octet_period = {
                                    ._class = &ChoiceRule_class, \
                                    .ChainRule = { \
                                        ._class = &(ChoiceRule_class.ChainRule_class), \
                                        .Rule = { \
                                            ._class = &(ChoiceRule_class.ChainRule_class.Rule_class), \
                                            .id = CHOICE_OCTET_PERIOD, \
                                            .cache_ = DEFAULT_PACKRATCACHE \
                                        }, \
                                        .deps = (Rule * [2]) {&(ipv4_octet.Rule), &(ipv4_period.Rule)}, \
                                        .deps_size = 2 \
                                    } \
                                    };
ListRule ipv4_list_octet = {._class = &ListRule_class, \
                            .DerivedRule = {\
                                ._class = &(ListRule_class.DerivedRule_class), \
                                .Rule = { \
                                    ._class = &(ListRule_class.DerivedRule_class.Rule_class), \
                                    .id = LIST_OCTET, \
                                    .cache_ = DEFAULT_PACKRATCACHE \
                                }, \
                                .rule = &(ipv4_octet.Rule) \
                            }, \
                            .delim = &(ipv4_period.Rule) \
                            };
Production ipv4_token = {._class = &Production_class, \
                        .AnonymousProduction = { \
                            ._class = &(Production_class.AnonymousProduction_class), \
                            .DerivedRule = {\
                                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), \
                                .Rule = { \
                                    ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), \
                                    .id = TOKEN, \
                                    .cache_ = DEFAULT_PACKRATCACHE \
                                }, \
                                .rule = &ipv4_choice_octet_period.ChainRule.Rule \
                            } \
                        }, \
                        .build_action = &token_action, \
                        };
Production ipv4_ipv4 = {._class = &Production_class, \
                        .AnonymousProduction = { \
                            ._class = &(Production_class.AnonymousProduction_class), \
                            .DerivedRule = {\
                                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), \
                                .Rule = { \
                                    ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), \
                                    .id = IPV4, \
                                    .cache_ = DEFAULT_PACKRATCACHE \
                                }, \
                                .rule = &ipv4_list_octet.DerivedRule.Rule \
                            } \
                        }, \
                        .build_action = &build_action_default, \
                        };

Rule * ipv4rules[NRULES] = {
    &ipv4_period.Rule,
    &ipv4_octet.Rule,
    &ipv4_list_octet.DerivedRule.Rule, 
    &ipv4_ipv4.AnonymousProduction.DerivedRule.Rule,
    &ipv4_choice_octet_period.ChainRule.Rule,
    &ipv4_token.AnonymousProduction.DerivedRule.Rule,
};

// must be null_terminated. not necessary. There are checks in LiteralRule_check_rule_ that will initialize as needed, but this would remove that branch
LiteralRule * ipv4_to_init[] = {
    &ipv4_octet,
    &ipv4_period,
    NULL
};

bool ipv4_initialized = false;

// not necessary to call. See note above ipv4_to_init
err_type ipv4_init(void) {
    err_type status = PEGGY_SUCCESS;
    if (!ipv4_initialized) {
        size_t i = 0;
        while (ipv4_to_init[i]) {
            if ((status = ipv4_to_init[i]->_class->compile(ipv4_to_init[i]))) {
                return status;
            }
            i++;
        }
    }
    return status;
}

void ipv4_clear_token(Token * tok) {
    printf("removing token from rule cache: ");
    Token_print(tok);
    printf("\n");
    err_type status = PEGGY_SUCCESS;
    for (int i = 0; i < NRULES; i++) {
        PackratCache_clear_token(&ipv4rules[i]->cache_, tok);
    }
}

void ipv4_dest(void) {
    printf("tearing down ipv4\n");
    err_type status = PEGGY_SUCCESS;
    for (int i = 0; i < NRULES; i++) {
        ipv4rules[i]->_class->dest(ipv4rules[i]);
    }
}