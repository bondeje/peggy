#include <peggy/utils.h>

#if defined(MSYS)
    #define REGEX_LEFT "^("
    #define REGEX_RIGHT ")"
#else 
    #define REGEX_LEFT 
    #define REGEX_RIGHT
#endif

// variadic is a list of Rule *
#define SEQUENCERULE(IDENTIFIER, ENUM_IDENTIFIER, ...) \
SequenceRule IDENTIFIER = { \
        .ChainRule = { \
            .Rule = { \
                ._class = &SequenceRule_class.ChainRule_class.Rule_class, \
                .id = ENUM_IDENTIFIER, \
            }, \
            ._class = &SequenceRule_class.ChainRule_class, \
            .deps = (Rule *[VARIADIC_SIZE(__VA_ARGS__)]) {__VA_ARGS__}, \
        .deps_size = VARIADIC_SIZE(__VA_ARGS__) \
        }, \
        ._class = &SequenceRule_class, \
}

// variadic is a list of Rule *
#define CHOICERULE(IDENTIFIER, ENUM_IDENTIFIER, ...) \
ChoiceRule IDENTIFIER = { \
    .ChainRule = { \
        .Rule = { \
            ._class = &ChoiceRule_class.ChainRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
        }, \
        ._class = &ChoiceRule_class.ChainRule_class, \
        .deps = (Rule *[VARIADIC_SIZE(__VA_ARGS__)]) {__VA_ARGS__}, \
        .deps_size = VARIADIC_SIZE(__VA_ARGS__) \
    }, \
    ._class = &ChoiceRule_class, \
}

#define LITERALRULE(IDENTIFIER, ENUM_IDENTIFIER, REGEX) \
LiteralRule IDENTIFIER = { \
    .Rule = { \
        ._class = &LiteralRule_class.Rule_class, \
        .id = ENUM_IDENTIFIER, \
    }, \
    ._class = &LiteralRule_class, \
    .regex_s = REGEX_LEFT REGEX REGEX_RIGHT, \
    .compiled = false \
}

#define LISTRULE(IDENTIFIER, ENUM_IDENTIFIER, pRULE_DELIM, pRULE_ELEMENT) \
ListRule IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &ListRule_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
        }, \
        ._class = &ListRule_class.DerivedRule_class, \
        .rule = pRULE_ELEMENT \
    }, \
    ._class = &ListRule_class, \
    .delim = pRULE_DELIM \
}

#define REPEATRULE_5(IDENTIFIER, ENUM_IDENTIFIER, pRULE, MIN_REP, MAX_REP) \
RepeatRule IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &RepeatRule_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
        }, \
        ._class = &RepeatRule_class.DerivedRule_class, \
        .rule = pRULE \
    }, \
    ._class = &RepeatRule_class, \
    .min_rep = MIN_REP, \
    .max_rep = MAX_REP \
}
#define REPEATRULE_4(IDENTIFIER, ENUM_IDENTIFIER, pRULE, MIN_REP) REPEATRULE_5(IDENTIFIER, ENUM_IDENTIFIER, pRULE, MIN_REP, 0)
#define REPEATRULE_3(IDENTIFIER, ENUM_IDENTIFIER, pRULE) REPEATRULE_5(IDENTIFIER, ENUM_IDENTIFIER, pRULE, 0, 0)
#define REPEATRULE(...) CAT(REPEATRULE_, VARIADIC_SIZE(__VA_ARGS__))(__VA_ARGS__)

#define NEGATIVELOOKAHEAD(IDENTIFIER, ENUM_IDENTIFIER, pRULE) \
NegativeLookahead IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &NegativeLookahead_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
        }, \
        ._class = &NegativeLookahead_class.DerivedRule_class, \
        .rule = pRULE \
    }, \
    ._class = &NegativeLookahead_class, \
}

#define POSITIVELOOKAHEAD(IDENTIFIER, ENUM_IDENTIFIER, pRULE) \
PositiveLookahead IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &PositiveLookahead_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
        }, \
        ._class = &PositiveLookahead_class.DerivedRule_class, \
        .rule = pRULE \
    }, \
    ._class = &PositiveLookahead_class, \
}

#define PRODUCTION_4(IDENTIFIER, ENUM_IDENTIFIER, pRULE, BUILD_ACTION) \
Production IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &Production_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
        }, \
        ._class = &Production_class.DerivedRule_class, \
        .rule = pRULE \
    }, \
    ._class = &Production_class, \
    .build_action = &BUILD_ACTION \
}
#define PRODUCTION_3(IDENTIFIER, ENUM_IDENTIFIER, pRULE) PRODUCTION_4(IDENTIFIER, ENUM_IDENTIFIER, pRULE, build_action_default)
#define PRODUCTION(...) CAT(PRODUCTION_, VARIADIC_SIZE(__VA_ARGS__))(__VA_ARGS__)