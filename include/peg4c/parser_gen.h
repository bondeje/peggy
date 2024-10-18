#include "peg4c/rule.h"

// maybe can eliminate this if pcre2 accepts the PCRE2_ANCHORED option as I expect
#define REGEX_LEFT 
#define REGEX_RIGHT

#ifndef _GNU_SOURCE
    #define REGEX_CAST (PCRE2_SPTR)
#else
    #define REGEX_CAST
#endif

/**
 * @brief build a static SequenceRule instance
 * @param[in] IDENTIFIER an identifier for the SequenceRule
 * @param[in] ENUM_IDENTIFIER the enum identifier to identify the rule at 
 *      runtime
 * @param[in] ... the "child" rules that must succeed in order for the 
 *      SequenceRule to succeed. Each element must be of type "Rule *"
 */
#define SEQUENCERULE(IDENTIFIER, ENUM_IDENTIFIER, ...) \
SequenceRule IDENTIFIER = { \
        .ChainRule = { \
            .Rule = { \
                ._class = &SequenceRule_class.ChainRule_class.Rule_class, \
                .id = ENUM_IDENTIFIER, \
                .name = STRINGIFY(ENUM_IDENTIFIER), \
            }, \
            ._class = &SequenceRule_class.ChainRule_class, \
            .deps = (Rule *[VARIADIC_SIZE(__VA_ARGS__)]) {__VA_ARGS__}, \
        .deps_size = VARIADIC_SIZE(__VA_ARGS__) \
        }, \
        ._class = &SequenceRule_class, \
}

/**
 * @brief build a static ChoiceRule instance
 * @param[in] IDENTIFIER an identifier for the ChoiceRule
 * @param[in] ENUM_IDENTIFIER the enum identifier to identify the rule at 
 *      runtime
 * @param[in] ... the "child" rules that must succeed in order for the 
 *      ChoiceRule to succeed. Each element must be of type "Rule *"
 */
#define CHOICERULE(IDENTIFIER, ENUM_IDENTIFIER, ...) \
ChoiceRule IDENTIFIER = { \
    .ChainRule = { \
        .Rule = { \
            ._class = &ChoiceRule_class.ChainRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
            .name = STRINGIFY(ENUM_IDENTIFIER), \
        }, \
        ._class = &ChoiceRule_class.ChainRule_class, \
        .deps = (Rule *[VARIADIC_SIZE(__VA_ARGS__)]) {__VA_ARGS__}, \
        .deps_size = VARIADIC_SIZE(__VA_ARGS__) \
    }, \
    ._class = &ChoiceRule_class, \
}

/**
 * @brief build a static LiteralRule instance
 * @param[in] IDENTIFIER an identifier for the LiteralRule
 * @param[in] ENUM_IDENTIFIER the enum identifier to identify the rule at 
 *      runtime
 * @param[in] REGEX The regex string to be matched for the LiteralRule
 */
#define LITERALRULE(IDENTIFIER, ENUM_IDENTIFIER, REGEX) \
LiteralRule IDENTIFIER = { \
    .Rule = { \
        ._class = &LiteralRule_class.Rule_class, \
        .id = ENUM_IDENTIFIER, \
        .name = STRINGIFY(ENUM_IDENTIFIER), \
    }, \
    ._class = &LiteralRule_class, \
    .regex_s = REGEX_CAST (REGEX_LEFT REGEX REGEX_RIGHT), \
    .compiled = false \
}

/**
 * @brief build a static ListRule instance
 * @param[in] IDENTIFIER an identifier for the ListRule
 * @param[in] ENUM_IDENTIFIER the enum identifier to identify the rule at 
 *      runtime
 * @param[in] pRULE_DELIM The delimeter "Rule *" instance used by the ListRule
 * @param[in] pRULE_ELEMENT The repeated element "Rule *" instance
 */
#define LISTRULE(IDENTIFIER, ENUM_IDENTIFIER, pRULE_DELIM, pRULE_ELEMENT) \
ListRule IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &ListRule_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
            .name = STRINGIFY(ENUM_IDENTIFIER), \
        }, \
        ._class = &ListRule_class.DerivedRule_class, \
        .rule = pRULE_ELEMENT \
    }, \
    ._class = &ListRule_class, \
    .delim = pRULE_DELIM \
}

/**
 * @brief build a static RepeatRule instance
 * @param[in] IDENTIFIER an identifier for the RepeatRule
 * @param[in] ENUM_IDENTIFIER the enum identifier to identify the rule at 
 *      runtime
 * @param[in] pRULE The repeated element "Rule *" instance
 * @param[in] MIN_REP (optional) minimum number of repetitions for RepeatRule 
 *      to succeed. Defaults to 0
 * @param[in] MAX_REP (optional) maximum number of repetitions for RepeatRule 
 *      to succeed. Defaults to 0. 0 means infinite maximum value
 */
#define REPEATRULE_5(IDENTIFIER, ENUM_IDENTIFIER, pRULE, MIN_REP, MAX_REP) \
RepeatRule IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &RepeatRule_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
            .name = STRINGIFY(ENUM_IDENTIFIER), \
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

/**
 * @brief build a static NegativeLookahead instance
 * @param[in] IDENTIFIER an identifier for the NegativeLookahead
 * @param[in] ENUM_IDENTIFIER the enum identifier to identify the rule at 
 *      runtime
 * @param[in] pRULE The Rule * which, if it succeeds, causes this rule to fail
 */
#define NEGATIVELOOKAHEAD(IDENTIFIER, ENUM_IDENTIFIER, pRULE) \
NegativeLookahead IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &NegativeLookahead_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
            .name = STRINGIFY(ENUM_IDENTIFIER), \
        }, \
        ._class = &NegativeLookahead_class.DerivedRule_class, \
        .rule = pRULE \
    }, \
    ._class = &NegativeLookahead_class, \
}

/**
 * @brief build a static PositiveLookahead instance
 * @param[in] IDENTIFIER an identifier for the PositiveLookahead
 * @param[in] ENUM_IDENTIFIER the enum identifier to identify the rule at 
 *      runtime
 * @param[in] pRULE The Rule * which, if it fails, causes this rule to succeed
 */
#define POSITIVELOOKAHEAD(IDENTIFIER, ENUM_IDENTIFIER, pRULE) \
PositiveLookahead IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &PositiveLookahead_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
            .name = STRINGIFY(ENUM_IDENTIFIER), \
        }, \
        ._class = &PositiveLookahead_class.DerivedRule_class, \
        .rule = pRULE \
    }, \
    ._class = &PositiveLookahead_class, \
}

/**
 * @brief build a static PositiveLookahead instance
 * @param[in] IDENTIFIER an identifier for the PositiveLookahead
 * @param[in] ENUM_IDENTIFIER the enum identifier to identify the rule at 
 *      runtime
 * @param[in] pRULE The child Rule * of the production
 * @param[in] BUILD_ACTION (optional) the function to customize building AST 
 *      nodes. It must have declaration:
 *      ASTNode * BUILD_ACTION(Production * prod, Parser * parser, ASTNode * node)
 *      Default value is "build_action_default"
 *      @brief use the context of the production rule and parser state to 
 *          generate/modify a node at build time
 *      @param[in] prod the production that is succeeding
 *      @param[in] parser the parser being run
 *      @param[in] node the child node (corresponding to pRULE above) that has 
 *          succeeded
 *      @returns the node to be placed in the AST and cache
 */
#define PRODUCTION_4(IDENTIFIER, ENUM_IDENTIFIER, pRULE, BUILD_ACTION) \
Production IDENTIFIER = { \
    .DerivedRule = { \
        .Rule = { \
            ._class = &Production_class.DerivedRule_class.Rule_class, \
            .id = ENUM_IDENTIFIER, \
            .name = STRINGIFY(ENUM_IDENTIFIER), \
        }, \
        ._class = &Production_class.DerivedRule_class, \
        .rule = pRULE \
    }, \
    ._class = &Production_class, \
    .build_action = &BUILD_ACTION \
}
#define PRODUCTION_3(IDENTIFIER, ENUM_IDENTIFIER, pRULE) PRODUCTION_4(IDENTIFIER, ENUM_IDENTIFIER, pRULE, build_action_default)
#define PRODUCTION(...) CAT(PRODUCTION_, VARIADIC_SIZE(__VA_ARGS__))(__VA_ARGS__)

