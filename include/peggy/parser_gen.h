#include <peggy/utils.h>

// variadic is a list of Rule *
#define SEQUENCE_RULE(IDENTIFIER, ENUM_IDENTIFIER, ...) \
SequenceRule IDENTIFIER = { \
        ._class = &SequenceRule_class, \
        .ChainRule = { \
            ._class = &(SequenceRule_class.ChainRule_class), \
            .Rule = { \
                ._class = &(SequenceRule_class.ChainRule_class.Rule_class), \
                .id = ENUM_IDENTIFIER, \
                .cache_ = DEFAULT_PACKRATCACHE \
            }, \
            .deps = (Rule *[VARIADIC_SIZE(__VA_ARGS__)]) {__VA_ARGS__}, \
        .deps_size = VARIADIC_SIZE(__VA_ARGS__) \
        } \
}

// variadic is a list of Rule *
#define CHOICE_RULE(IDENTIFIER, ENUM_IDENTIFIER, ...) \
ChoiceRule IDENTIFIER = { \
    ._class = &ChoiceRule_class, \
    .ChainRule = { \
        ._class = &(ChoiceRule_class.ChainRule_class), \
        .Rule = { \
            ._class = &(ChoiceRule_class.ChainRule_class.Rule_class), \
            .id = ENUM_IDENTIFIER, \
            .cache_ = DEFAULT_PACKRATCACHE \
        }, \
        .deps = (Rule *[VARIADIC_SIZE(__VA_ARGS__)]) {__VA_ARGS__}, \
        .deps_size = VARIADIC_SIZE(__VA_ARGS__) \
    } \
}

#define LITERAL_RULE(IDENTIFIER, ENUM_IDENTIFIER, REGEX) \
LiteralRule IDENTIFIER = { \
    ._class = &LiteralRule_class, \
    .Rule = { \
        ._class = &(LiteralRule_class.Rule_class), \
        .id = ENUM_IDENTIFIER, \
        .cache_ = DEFAULT_PACKRATCACHE \
    }, \
    .regex_s = REGEX, \
    .compiled = false \
}

#define LIST_RULE(IDENTIFIER, ENUM_IDENTIFIER, pRULE_DELIM, pRULE_ELEMENT) \
ListRule IDENTIFIER = { \
    ._class = &ListRule_class, \
    .DerivedRule = { \
        ._class = &(ListRule_class.DerivedRule_class), \
        .Rule = { \
            ._class = &(ListRule_class.DerivedRule_class.Rule_class), \
            .id = ENUM_IDENTIFIER, \
            .cache_ = DEFAULT_PACKRATCACHE \
        }, \
        .rule = pRULE_ELEMENT \
    }, \
    .delim = pRULE_DELIM \
}

#define REPEAT_RULE_5(IDENTIFIER, ENUM_IDENTIFIER, pRULE, MIN_REP, MAX_REP) \
RepeatRule IDENTIFIER = { \
    ._class = &RepeatRule_class, \
    .DerivedRule = { \
        ._class = &(RepeatRule_class.DerivedRule_class), \
        .Rule = { \
            ._class = &(RepeatRule_class.DerivedRule_class.Rule_class), \
            .id = ENUM_IDENTIFIER, \
            .cache_ = DEFAULT_PACKRATCACHE \
        }, \
        .rule = pRULE \
    }, \
    .min_rep = MIN_REP, \
    .max_rep = MAX_REP \
}
#define REPEAT_RULE_4(IDENTIFIER, ENUM_IDENTIFIER, pRULE, MIN_REP) REPEAT_RULE_5(IDENTIFIER, ENUM_IDENTIFIER, pRULE, MIN_REP, 0)
#define REPEAT_RULE_3(IDENTIFIER, ENUM_IDENTIFIER, pRULE) REPEAT_RULE_5(IDENTIFIER, ENUM_IDENTIFIER, pRULE, 0, 0)
#define REPEAT_RULE(...) CAT(REPEAT_RULE_, VARIADIC_SIZE(__VA_ARGS__))(__VA_ARGS__)

#define OPTIONAL_RULE(IDENTIFIER, ENUM_IDENTIFIER, pRULE) \
OptionalRule IDENTIFIER = { \
    ._class = &OptionalRule_class, \
    .DerivedRule = { \
        ._class = &(OptionalRule_class.DerivedRule_class), \
        .Rule = { \
            ._class = &(OptionalRule_class.DerivedRule_class.Rule_class), \
            .id = ENUM_IDENTIFIER, \
            .cache_ = DEFAULT_PACKRATCACHE \
        }, \
        .rule = pRULE \
    } \
}

#define NEGATIVE_LOOKAHEAD(IDENTIFIER, ENUM_IDENTIFIER, pRULE) \
NegativeLookahead IDENTIFIER = { \
    ._class = &NegativeLookahead_class, \
    .DerivedRule = { \
        ._class = &(NegativeLookahead_class.DerivedRule_class), \
        .Rule = { \
            ._class = &(NegativeLookahead_class.DerivedRule_class.Rule_class), \
            .id = ENUM_IDENTIFIER, \
            .cache_ = DEFAULT_PACKRATCACHE \
        }, \
        .rule = pRULE \
    } \
}

#define POSITIVE_LOOKAHEAD(IDENTIFIER, ENUM_IDENTIFIER, pRULE) \
PositiveLookahead IDENTIFIER = { \
    ._class = &PositiveLookahead_class, \
    .DerivedRule = { \
        ._class = &(PositiveLookahead_class.DerivedRule_class), \
        .Rule = { \
            ._class = &(PositiveLookahead_class.DerivedRule_class.Rule_class), \
            .id = ENUM_IDENTIFIER, \
            .cache_ = DEFAULT_PACKRATCACHE \
        }, \
        .rule = pRULE \
    } \
}

#define ANONYMOUS_PRODUCTION(IDENTIFIER, ENUM_IDENTIFIER, pRULE) \
AnonymousProduction IDENTIFIER = { \
    ._class = &AnonymousProduction_class, \
    .DerivedRule = {\
        ._class = &(AnonymousProduction_class.DerivedRule_class), \
        .Rule = { \
            ._class = &(AnonymousProduction_class.DerivedRule_class.Rule_class), \
            .id = ENUM_IDENTIFIER, \
            .cache_ = DEFAULT_PACKRATCACHE \
        }, \
        .rule = pRULE \
    } \
}

#define PRODUCTION_4(IDENTIFIER, ENUM_IDENTIFIER, pRULE, BUILD_ACTION) \
Production IDENTIFIER = { \
    ._class = &Production_class, \
    .AnonymousProduction = { \
        ._class = &(Production_class.AnonymousProduction_class), \
        .DerivedRule = { \
            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), \
            .Rule = { \
                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), \
                .id = ENUM_IDENTIFIER, \
                .cache_ = DEFAULT_PACKRATCACHE \
            }, \
            .rule = pRULE \
        } \
    }, \
    .build_action = &BUILD_ACTION \
}
#define PRODUCTION_3(IDENTIFIER, ENUM_IDENTIFIER, pRULE) PRODUCTION_4(IDENTIFIER, ENUM_IDENTIFIER, pRULE, build_action_default)
#define PRODUCTION(...) CAT(PRODUCTION_, VARIADIC_SIZE(__VA_ARGS__))(__VA_ARGS__)