#ifndef P4C_RULE_H
#define P4C_RULE_H

#ifdef __linux__
#define _GNU_SOURCE
#endif

/* C standard library includes */
#include <stddef.h>

/* POSIX */
#ifdef _GNU_SOURCE
#include <regex.h>
#endif

/* lib includes */
#ifndef _GNU_SOURCE
#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif
#include <pcre2.h>
#else
#endif

/* peg4c includes */
#include "peg4c/utils.h"
#include "peg4c/parser_gen.h"
#include "peg4c/astnode.h"
#include "peg4c/token.h"

/* Rule definitions and declarations */

typedef struct RuleType RuleType;

/**
 * @struct Rule rule.h
 * @brief base Rule type for rules in a grammar
 */
struct Rule {
    RuleType * _class;  /* vtable */
    rule_id_type id;    //!< integer id for the instance of the Rule
    char const * name;  //!< a convenient name for printing the instance of  
                        //!<    the Rule
};

/**
 * vtable for Rule and an instance
 */
extern struct RuleType {
    // an integer to distinguish the different *Rule structs at runtime
    // new subrules must have a value distinct from those in enum RuleTypeID
    RuleTypeID type;
    // allocate a new Rule
    Rule * (*new)(rule_id_type id, char const * name);
    // initialize a new Rule
    err_type (*init)(Rule * self, rule_id_type id, char const * name);
    // destroy a rule
    void (*dest)(Rule * self);
    // delete a rule
    void (*del)(Rule * self);
    // evaluate the rule. Every subtype of Rule MUST override this function
    ASTNode * (*check_rule_)(Rule * self, Parser * parser);
} Rule_class;

/**
 * @brief allocate a new Rule
 * @param[in] id integer id for the instance of the Rule
 * @param[in] name a convenient name for printing the instance of the Rule
 * @returns pointer to the new Rule instance
 */
Rule * Rule_new(rule_id_type id, char const * name);

/**
 * @brief initialize a Rule
 * @param[in] self pointer to Rule
 * @param[in] id integer id for the instance of the Rule
 * @param[in] name a convenient name for printing the instance of the Rule
 * @returns non-zero on error, else 0
 */
err_type Rule_init(Rule * self, int id, char const * name);

/**
 * @brief destroy and reclaim memory of the Rule instance
 */
void Rule_dest(Rule * self);

/**
 * @brief destroy and delete the Rule instance
 */
void Rule_del(Rule * self);

/**
 * @brief dummy function. do not implement
 */
ASTNode * Rule_check_rule_(Rule * rule, Parser * parser);

/**
 * @brief The master function that evaluates any Rule *
 * @param[in] self the Rule to be evaluated
 * @param[in] parser the Parser being executed
 * @returns On failure returns a node where Parser_is_fail_node(parser, return)
 *      evaluates to true else a valid ASTNode
 */
ASTNode * Rule_check(Rule * self, Parser * parser);

/* ChainRule definitions and declarations */

typedef struct ChainRule ChainRule;

/**
 * @struct ChainRule rule.h
 * @brief A chain rule is a Rule that has a list of subrules that must be at 
 *      least in some part evaluated
 */
struct ChainRule {
    Rule Rule;                      //!< base rule instance
    struct ChainRuleType * _class;
    Rule ** deps;                   //!< the subrule dependencies
    size_t deps_size;               //!< the number of subrules
};

extern struct ChainRuleType {
    struct RuleType Rule_class;
    ChainRule * (*new)(rule_id_type id, char const * name, size_t deps_size, 
        Rule ** deps);
    err_type (*init)(ChainRule * self, rule_id_type id, char const * name, 
        size_t deps_size, Rule ** deps);
    void (*dest)(ChainRule * self);
    void (*del)(ChainRule * self);
} ChainRule_class;

/**
 * @brief allocate a new ChainRule
 * @param[in] id integer id for the instance of the ChainRule
 * @param[in] name a convenient name for printing the instance of the ChainRule
 * @param[in] deps_size the number of subrule dependencies of the ChainRule
 * @param[in] deps array of Rule * of length "deps_size"
 * @returns pointer to the new ChainRule instance
 */
ChainRule * ChainRule_new(rule_id_type id, char const * name, size_t deps_size, 
    Rule ** deps);

/**
 * @brief initialize a new ChainRule
 * @param[in] self the ChainRule to initialize
 * @param[in] id integer id for the instance of the ChainRule
 * @param[in] name a convenient name for printing the instance of the ChainRule
 * @param[in] deps_size the number of subrule dependencies of the ChainRule
 * @param[in] deps array of Rule * of length "deps_size"
 * @returns non-zero on failure else 0
 */
err_type ChainRule_init(ChainRule * self, rule_id_type id, char const * name, 
    size_t deps_size, Rule ** deps);

/**
 * @brief destroy and reclaim memory of the ChainRule instance
 */
void ChainRule_dest(ChainRule * self);

/**
 * @brief destroy and delete the ChainRule instance
 */
void ChainRule_del(ChainRule * self);

/**
 * @brief destroy and delete the ChainRule instance passed as Rule *
 */
void ChainRule_as_Rule_del(Rule * chain_rule);

/* SequenceRule definitions and declarations */

typedef struct SequenceRule SequenceRule;

/**
 * @struct SequenceRule rule.h
 * @brief A specific type of ChainRule where the subrules must be evaluated in 
 *      sequence and all must succeed for the overall rule
 *      e.g. for SequenceRule a,b,c,d,e to succeed the tokens in sequence
 *          must spell "abcde"
 */
struct SequenceRule {
    ChainRule ChainRule;
    struct SequenceRuleType * _class;
};

extern struct SequenceRuleType {
    struct ChainRuleType ChainRule_class;
    SequenceRule * (*new)(rule_id_type id, char const * name, size_t deps_size, 
        Rule ** deps);
    err_type (*init)(SequenceRule * self, rule_id_type id, char const * name, 
        size_t deps_size, Rule ** deps);
    void (*dest)(SequenceRule * self);
    void (*del)(SequenceRule * self);
} SequenceRule_class;

/**
 * @brief allocate a new SequenceRule
 * @param[in] id integer id for the instance of the SequenceRule
 * @param[in] name a convenient name for printing the instance of the 
 *      SequenceRule
 * @param[in] deps_size the number of subrule dependencies of the SequenceRule
 * @param[in] deps array of Rule * of length "deps_size"
 * @returns pointer to the new SequenceRule instance
 */
SequenceRule * SequenceRule_new(rule_id_type id, char const * name, size_t deps_size, Rule ** deps);

/**
 * @brief initialize a new SequenceRule
 * @param[in] self the SequenceRule to initialize
 * @param[in] id integer id for the instance of the SequenceRule
 * @param[in] name a convenient name for printing the instance of the 
 *      SequenceRule
 * @param[in] deps_size the number of subrule dependencies of the SequenceRule
 * @param[in] deps array of Rule * of length "deps_size"
 * @returns non-zero on failure else 0
 */
err_type SequenceRule_init(SequenceRule * self, rule_id_type id, char const * name, size_t deps_size, Rule ** deps);

/**
 * @brief destroy and reclaim memory of the SequenceRule instance
 */
void SequenceRule_dest(SequenceRule * self);

/**
 * @brief destroy and delete the SequenceRule instance
 */
void SequenceRule_del(SequenceRule * self);

/**
 * @brief destroy and delete the SequenceRule instance passed as ChainRule
 */
void SequenceRule_as_ChainRule_del(ChainRule * sequence_chain);

/**
 * @brief destroy and delete the SequenceRule instance passed as Rule
 */
void SequenceRule_as_Rule_del(Rule * sequence_rule);

/**
 * @brief evaluates the specified SequenceRule
 */
ASTNode * SequenceRule_check_rule_(Rule * sequence_rule, Parser * parser);

/* ChoiceRule definitions and declarations */

typedef struct ChoiceRule ChoiceRule;

/**
 * @struct ChoiceRule rule.h
 * @brief A specific type of ChainRule where the subrules will be evaluated in
 *      order but the first successful subrule returns its node
 *      e.g. "a" "b" "c" "d" all succeed for ChoiceRule: a|b|c|d
 */
struct ChoiceRule {
    ChainRule ChainRule;
    struct ChoiceRuleType * _class;
};

extern struct ChoiceRuleType {
    struct ChainRuleType ChainRule_class;
    ChoiceRule * (*new)(rule_id_type id, char const * name, size_t deps_size, Rule ** deps);
    err_type (*init)(ChoiceRule * self, rule_id_type id, char const * name, size_t deps_size, Rule ** deps);
    void (*dest)(ChoiceRule * self);
    void (*del)(ChoiceRule * self);
} ChoiceRule_class;

/**
 * @brief allocate a new ChoiceRule
 * @param[in] id integer id for the instance of the ChoiceRule
 * @param[in] name a convenient name for printing the instance of the 
 *      ChoiceRule
 * @param[in] deps_size the number of subrule dependencies of the ChoiceRule
 * @param[in] deps array of Rule * of length "deps_size"
 * @returns pointer to the new ChoiceRule instance
 */
ChoiceRule * ChoiceRule_new(rule_id_type id, char const * name, size_t deps_size, Rule ** deps);

/**
 * @brief initialize a new ChoiceRule
 * @param[in] self the ChoiceRule to initialize
 * @param[in] id integer id for the instance of the ChoiceRule
 * @param[in] name a convenient name for printing the instance of the 
 *      ChoiceRule
 * @param[in] deps_size the number of subrule dependencies of the ChoiceRule
 * @param[in] deps array of Rule * of length "deps_size"
 * @returns non-zero on failure else 0
 */
err_type ChoiceRule_init(ChoiceRule * self, rule_id_type id, char const * name, size_t deps_size, Rule ** deps);

/**
 * @brief destroy and reclaim memory of the ChoiceRule instance
 */
void ChoiceRule_dest(ChoiceRule * self);

/**
 * @brief destroy and delete the ChoiceRule instance
 */
void ChoiceRule_del(ChoiceRule * self);

/**
 * @brief destroy and delete the ChoiceRule instance passed as ChainRule
 */
void ChoiceRule_as_ChainRule_del(ChainRule * choice_chain);

/**
 * @brief destroy and delete the ChoiceRule instance passed as Rule
 */
void ChoiceRule_as_Rule_del(Rule * choice_rule);

/**
 * @brief evaluates the specified ChoiceRule
 */
ASTNode * ChoiceRule_check_rule_(Rule * choice_rule, Parser * parser);

/* LiteralRule class definitions and declarations */

typedef struct LiteralRule LiteralRule;

/**
 * @struct LiteralRule rule.h
 * @brief A literal rule is a Rule that tracks and executes both regex and
 *      and string literals (as regex). Under _GNU_SOURCE, it uses the glibc
 *      API for regex.h otherwise uses pcre2 as a backend
 */
struct LiteralRule {
    Rule Rule;
    struct LiteralRuleType * _class;
#ifdef _GNU_SOURCE
    char const * regex_s;
    regex_t regex;
#else
    PCRE2_SPTR regex_s;
    pcre2_code * regex;
    pcre2_match_data * match;
#endif
    bool compiled;
};

extern struct LiteralRuleType {
    struct RuleType Rule_class;
    LiteralRule * (*new)(rule_id_type id, char const * name, 
        char const * regex_s);
    err_type (*init)(LiteralRule * self, rule_id_type id, 
        char const * name, char const * regex_s);
    err_type (*compile)(LiteralRule * self);
    void (*dest)(LiteralRule * self);
    void (*del)(LiteralRule * self);
} LiteralRule_class;

/**
 * @brief allocate a new LiteralRule
 * @param[in] id integer id for the instance of the LiteralRule
 * @param[in] name a convenient name for printing the instance of the 
 *      LiteralRule
 * @param[in] regex_s the regex string to be applied
 * @returns pointer to the new LiteralRule instance
 */
LiteralRule * LiteralRule_new(rule_id_type id, char const * name, 
    char const * regex_s);

/**
 * @brief compile the underlying regex for the LiteralRule
 */
err_type LiteralRule_compile_regex(LiteralRule * self);


/**
 * @brief initialize a LiteralRule
 * @param[in] self pointer to LiteralRule
 * @param[in] id integer id for the instance of the LiteralRule
 * @param[in] name a convenient name for printing the instance of the 
 *      LiteralRule
 * @param[in] regex_s the regex string to be applied
 * @returns non-zero on error, else 0
 */
err_type LiteralRule_init(LiteralRule * self, rule_id_type id, 
    char const * name, char const * regex_s);

/**
 * @brief destroy and reclaim memory of the LiteralRule instance
 */
void LiteralRule_dest(LiteralRule * self);

/**
 * @brief destroy and delete the LiteralRule instance
 */
void LiteralRule_del(LiteralRule * self);

/**
 * @brief destroy and delete the LiteralRule instance passed as Rule *
 */
void LiteralRule_as_Rule_del(Rule * literal_rule);

void LiteralRule_as_Rule_dest(Rule * literal_rule);

/**
 * @brief evaluates the specified LiteralRule
 */
ASTNode * LiteralRule_check_rule_(Rule * literal_rule, Parser * parser);

/* DerivedRule abstract class definitions and declarations */

typedef struct DerivedRule DerivedRule;
typedef struct DerivedRuleType DerivedRuleType;

/**
 * @struct DeriveRule rule.h
 * @brief A derived rule is a Rule that depends in some way on a single subrule
 */
struct DerivedRule {
    Rule Rule;
    struct DerivedRuleType * _class;
    Rule * rule;    //!< the subrule dependence
};

extern struct DerivedRuleType {
    struct RuleType Rule_class;
    DerivedRule * (*new)(rule_id_type id, char const * name, Rule * rule);
    err_type (*init)(DerivedRule * self, rule_id_type id, char const * name, 
        Rule * rule);
    void (*dest)(DerivedRule * self);
    void (*del)(DerivedRule * self);
} DerivedRule_class;

/**
 * @brief allocate a new DerivedRule
 * @param[in] id integer id for the instance of the DerivedRule
 * @param[in] name a convenient name for printing the instance of the 
 *      DerivedRule
 * @param[in] rule the subrule dependency
 * @returns pointer to the new DerivedRule instance
 */
DerivedRule * DerivedRule_new(rule_id_type id, char const * name, Rule * rule);

/**
 * @brief initialize a new DerivedRule
 * @param[in] self the DerivedRule to initialize
 * @param[in] id integer id for the instance of the DerivedRule
 * @param[in] name a convenient name for printing the instance of the 
 *      DerivedRule
 * @param[in] rule the subrule dependency
 * @returns non-zero on failure else 0
 */
err_type DerivedRule_init(DerivedRule * self, rule_id_type id, 
    char const * name, Rule * rule);

/**
 * @brief destroy and reclaim memory of the DerivedRule instance
 */
void DerivedRule_dest(DerivedRule * self);

/**
 * @brief destroy and delete the DerivedRule instance
 */
void DerivedRule_del(DerivedRule * self);

/**
 * @brief destroy and delete the DerivedRule instance passed as Rule *
 */
void DerivedRule_as_Rule_del(Rule * derived_rule);

/* ListRule class definitions and declarations */

typedef struct ListRule ListRule;

/**
 * @struct ListRule rule.h
 * @brief A list rule is a DerivedRule uses two subrule dependencies where
 *      successful evaluation is a sequence of DerivedRule.rule successes
 *      separated by successful evaluation of the delim subrule
 *      e.g. "a,a,a,a,a,a" can be expressed as ','.'a'
 */
struct ListRule {
    DerivedRule DerivedRule;
    struct ListRuleType * _class;
    Rule * delim;   //!< the delimiter subrule dependence
};

extern struct ListRuleType {
    struct DerivedRuleType DerivedRule_class;
    ListRule * (*new)(rule_id_type id, char const * name, Rule * rule, 
        Rule * delim);
    err_type (*init)(ListRule * self, rule_id_type id, char const * name, 
        Rule * rule, Rule * delim);
    void (*dest)(ListRule * self);
    void (*del)(ListRule * self);
} ListRule_class;

/**
 * @brief allocate a new ListRule
 * @param[in] id integer id for the instance of the ListRule
 * @param[in] name a convenient name for printing the instance of the 
 *      ListRule
 * @param[in] rule the base rule which must succeed at least once for a ListRule
 * @param[in] delim the rule separating each successful evaluation of rule
 * @returns pointer to the new ListRule instance
 */
ListRule * ListRule_new(rule_id_type id, char const * name, Rule * rule, 
    Rule * delim);

/**
 * @brief allocate a new ListRule
 * @param[in] self ListRule pointer
 * @param[in] id integer id for the instance of the ListRule
 * @param[in] name a convenient name for printing the instance of the 
 *      ListRule
 * @param[in] rule the base rule which must succeed at least once for a ListRule
 * @param[in] delim the rule separating each successful evaluation of rule
 * @returns non-zero on failure else 0
 */
err_type ListRule_init(ListRule * self, rule_id_type id, char const * name, 
    Rule * rule, Rule * delim);

/**
 * @brief destroy and reclaim memory of the ListRule instance
 */
void ListRule_dest(ListRule * self);

/**
 * @brief destroy and delete the ListRule instance
 */
void ListRule_del(ListRule * self);

/**
 * @brief destroy and delete the ListRule instance passed as DerivedRule
 */
void ListRule_as_DerivedRule_del(DerivedRule * list_rule);

/**
 * @brief destroy and delete the ListRule instance passed as Rule
 */
void ListRule_as_Rule_del(Rule * list_rule);

/**
 * @brief evaluates the specified ListRule
 */
ASTNode * ListRule_check_rule_(Rule * list_rule, Parser * parser);

/* RepeatRule class definitions and declarations */

typedef struct RepeatRule RepeatRule;

/**
 * @struct RepeatRule rule.h
 * @brief A repated rule is a DerivedRule where successful evaluation is a
 *      specified range of successful evaluations of the DerivedRule.rule
 *      e.g. "aaaaaa" is successful for min_rep <= 6 and max_rep >= 6 or 0
 *          'a'{6}, 'a'{x,}, 'a'{x,y}, 'a'+ for x<=6 and y=0 or y>=6
 *      This also encompasses optional rules min_rep = 0, max_rep = 1
 */
struct RepeatRule {
    DerivedRule DerivedRule;
    struct RepeatRuleType * _class;
    size_t min_rep;
    size_t max_rep;
};

extern struct RepeatRuleType {
    struct DerivedRuleType DerivedRule_class;
    RepeatRule * (*new)(rule_id_type id, char const * name, Rule * rule, 
        size_t min_rep, size_t max_rep);
    err_type (*init)(RepeatRule * self, rule_id_type id, char const * name, 
        Rule * rule, size_t min_rep, size_t max_rep);
    void (*dest)(RepeatRule * self);
    void (*del)(RepeatRule * self);
} RepeatRule_class;

/**
 * @brief allocate a new RepeatRule
 * @param[in] id integer id for the instance of the RepeatRule
 * @param[in] name a convenient name for printing the instance of the 
 *      RepeatRule
 * @param[in] rule the base rule to be repeated
 * @param[in] min_rep the minimum number of repetitions required for success
 * @param[in] max_rep the maximum or infinite(0) number of repetions to capture
 * @returns pointer to the new RepeatRule instance
 */
RepeatRule * RepeatRule_new(rule_id_type id, char const * name, Rule * rule, 
    size_t min_rep, size_t max_rep);

/**
 * @brief allocate a new RepeatRule
 * @param[in] id integer id for the instance of the RepeatRule
 * @param[in] name a convenient name for printing the instance of the 
 *      RepeatRule
 * @param[in] rule the base rule to be repeated
 * @param[in] min_rep the minimum number of repetitions required for success
 * @param[in] max_rep the maximum or infinite(0) number of repetions to capture
 * @returns non-zero on failure else 0
 */
err_type RepeatRule_init(RepeatRule * selfp, rule_id_type id, char const * name,
    Rule * rule, size_t min_rep, size_t max_rep);

/**
 * @brief destroy and reclaim memory of the RepeatRule instance
 */
void RepeatRule_dest(RepeatRule * self);

/**
 * @brief destroy and delete the RepeatRule instance
 */
void RepeatRule_del(RepeatRule * self);

/**
 * @brief destroy and delete the RepeatRule instance passed as DerivedRule
 */
void RepeatRule_as_DerivedRule_del(DerivedRule * repeat_rule);

/**
 * @brief destroy and delete the RepeatRule instance passed as Rule
 */
void RepeatRule_as_Rule_del(Rule * repeat_rule);

/**
 * @brief evaluates the specified RepeatRule
 */
ASTNode * RepeatRule_check_rule_(Rule * repeat_rule, Parser * parser);

/* NegativeLookahead Rule class definitions and declarations */

typedef struct NegativeLookahead NegativeLookahead;

/**
 * @struct NegativeLookahead rule.h
 * @brief A negative lookahead is a DerivedRule where successful evaluation 
 *      requires failure of the DerivedRule.rule. No tokens are consumed
 */
struct NegativeLookahead {
    DerivedRule DerivedRule;
    struct NegativeLookaheadType * _class;
};

extern struct NegativeLookaheadType {
    struct DerivedRuleType DerivedRule_class;
    NegativeLookahead * (*new)(rule_id_type id, char const * name, Rule * rule);
    err_type (*init)(NegativeLookahead * self, rule_id_type id, char const * name, Rule * rule);
    void (*dest)(NegativeLookahead * self);
    void (*del)(NegativeLookahead * self);
} NegativeLookahead_class;

/**
 * @brief allocate a new NegativeLookahead
 * @param[in] id integer id for the instance of the NegativeLookahead
 * @param[in] name a convenient name for printing the instance of the 
 *      NegativeLookahead
 * @param[in] rule the base rule to look ahead for
 * @returns pointer to the new NegativeLookahead instance
 */
NegativeLookahead * NegativeLookahead_new(rule_id_type id, char const * name, Rule * rule);

/**
 * @brief allocate a new NegativeLookahead
 * @param[in] self the NegativeLookahead rule
 * @param[in] id integer id for the instance of the NegativeLookahead
 * @param[in] name a convenient name for printing the instance of the 
 *      NegativeLookahead
 * @param[in] rule the base rule to look ahead for
 * @returns non-zero on failure, else 0
 */
err_type NegativeLookahead_init(NegativeLookahead * self, rule_id_type id, char const * name, Rule * rule);

/**
 * @brief destroy and reclaim memory of the NegativeLookahead instance
 */
void NegativeLookahead_dest(NegativeLookahead * self);

/**
 * @brief destroy and delete the NegativeLookahead instance
 */
void NegativeLookahead_del(NegativeLookahead * self);

/**
 * @brief destroy and delete the NegativeLookahead instance passed as DerivedRule
 */
void NegativeLookahead_as_DerivedRule_del(DerivedRule * negative_lookahead);

/**
 * @brief destroy and delete the NegativeLookahead instance passed as Rule
 */
void NegativeLookahead_as_Rule_del(Rule * negative_lookahead);

/**
 * @brief evaluates the specified NegativeLookahead
 */
ASTNode * NegativeLookahead_check_rule_(Rule * negative_lookahead, Parser * parser);

/* PositiveLookahead Rule definitions and declarations */

typedef struct PositiveLookahead PositiveLookahead;

/**
 * @struct PositiveLookahead rule.h
 * @brief A positive lookahead is a DerivedRule where successful evaluation 
 *      requires success of the DerivedRule.rule. No tokens are consumed
 */
struct PositiveLookahead {
    DerivedRule DerivedRule;
    struct PositiveLookaheadType * _class;
};

extern struct PositiveLookaheadType {
    struct DerivedRuleType DerivedRule_class;
    PositiveLookahead * (*new)(rule_id_type id, char const * name, Rule * rule);
    err_type (*init)(PositiveLookahead * self, rule_id_type id, char const * name, Rule * rule);
    void (*dest)(PositiveLookahead * self);
    void (*del)(PositiveLookahead * self);
} PositiveLookahead_class;

/**
 * @brief allocate a new PositiveLookahead
 * @param[in] id integer id for the instance of the PositiveLookahead
 * @param[in] name a convenient name for printing the instance of the 
 *      PositiveLookahead
 * @param[in] rule the base rule to look ahead for
 * @returns pointer to the new PositiveLookahead instance
 */
PositiveLookahead * PositiveLookahead_new(rule_id_type id, char const * name, Rule * rule);

/**
 * @brief allocate a new PositiveLookahead
 * @param[in] self the PositiveLookahead rule
 * @param[in] id integer id for the instance of the PositiveLookahead
 * @param[in] name a convenient name for printing the instance of the 
 *      PositiveLookahead
 * @param[in] rule the base rule to look ahead for
 * @returns non-zero on failure, else 0
 */
err_type PositiveLookahead_init(PositiveLookahead * self, rule_id_type id, char const * name, Rule * rule);

/**
 * @brief destroy and reclaim memory of the PositiveLookahead instance
 */
void PositiveLookahead_dest(PositiveLookahead * self);

/**
 * @brief destroy and delete the PositiveLookahead instance
 */
void PositiveLookahead_del(PositiveLookahead * self);

/**
 * @brief destroy and delete the PositiveLookahead instance passed as DerivedRule
 */
void PositiveLookahead_as_DerivedRule_del(DerivedRule * positive_lookahead);

/**
 * @brief destroy and delete the PositiveLookahead instance passed as Rule
 */
void PositiveLookahead_as_Rule_del(Rule * positive_lookahead);

/**
 * @brief evaluates the specified PositiveLookahead
 */
ASTNode * PositiveLookahead_check_rule_(Rule * positive_lookahead, Parser * parser);

/* Production Rule abstract class definitions and declarations */

typedef struct Production Production;

/**
 * @brief the default build action function for productions
 * @param production the Production rule whose node is being built
 * @param parser the executing parser
 * @param node the dependent node of the production node being built
 * @returns the newly built ASTNode
 */
ASTNode * build_action_default(Production * production, Parser * parser, ASTNode * node);
typedef ASTNode * (*build_action_ftype)(Production * production, Parser * parser, ASTNode *node);

/**
 * @struct Production rule.h
 * @brief A production is a DerivedRule where successful evaluation 
 *      requires success of the DerivedRule.rule whose resulting node is modified
 *      by a build_action function during parsing stage
 */
struct Production {
    DerivedRule DerivedRule;
    struct ProductionType * _class;
    build_action_ftype build_action;
};

extern struct ProductionType {
    struct DerivedRuleType DerivedRule_class;
    Production * (*new)(rule_id_type id, char const * name, Rule * rule, build_action_ftype build_action);
    err_type (*init)(Production * self, rule_id_type id, char const * name, Rule * rule, build_action_ftype build_action);
    void (*dest)(Production * self);
    void (*del)(Production * self);
} Production_class;

/**
 * @brief allocate a new Production
 * @param[in] id integer id for the instance of the Production
 * @param[in] name a convenient name for printing the instance of the 
 *      Production
 * @param[in] rule the base rule to look ahead for
 * @param[in] build_action the build action function for the production
 *      @brief the default build action function for productions
 *      @param production the Production rule whose node is being built
 *      @param parser the executing parser
 *      @param node the dependent node of the production node being built
 *      @returns the newly built ASTNode
 * @returns pointer to the new Production instance
 */
Production * Production_new(rule_id_type id, char const * name, Rule * rule, build_action_ftype build_action);

/**
 * @brief intialize a new Production
 * @param[in] self the Production rule
 * @param[in] id integer id for the instance of the Production
 * @param[in] name a convenient name for printing the instance of the 
 *      Production
 * @param[in] rule the base rule to look ahead for
 * @param[in] build_action the build action function for the production
 *      @brief the default build action function for productions
 *      @param production the Production rule whose node is being built
 *      @param parser the executing parser
 *      @param node the dependent node of the production node being built
 *      @returns the newly built ASTNode
 * @returns non-zero on failure else 0
 */
err_type Production_init(Production * self, rule_id_type id, char const * name, Rule * rule, build_action_ftype build_action);

/**
 * @brief destroy and reclaim memory of the Production instance
 */
void Production_dest(Production * self);

/**
 * @brief destroy and delete the Production instance
 */
void Production_del(Production * self);

/**
 * @brief destroy and delete the Production instance passed as DerivedRule
 */
void Production_as_DerivedRule_del(DerivedRule * production);

/**
 * @brief destroy and delete the Production instance passed as Rule
 */
void Production_as_Rule_del(Rule * production);

/**
 * @brief evaluates the specified Production
 */
ASTNode * Production_check_rule_(Rule * production, Parser * parser);

#endif // P4C_RULE_H
