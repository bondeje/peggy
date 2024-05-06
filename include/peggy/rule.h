#ifndef PEGGY_RULE_H
#define PEGGY_RULE_H

/* C standard library includes */
#include <stddef.h>

/* POSIX */
#ifdef Linux
#include <regex.h>
#endif

/* lib includes */
#ifndef Linux
#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif
#include <pcre2.h>
#endif

/* peggy includes */
#include <peggy/utils.h>
#include <peggy/parser_gen.h>
#include <peggy/astnode.h>
#include <peggy/token.h>

/**
 * 
 * TODO:
 *      need to add a "name" to each rule for printing
 *      use an arena allocator in the hash map
 *      fix nodes so that they are not individually allocated but managed as a single allocated stack by the Parser "node_list"
 *      fix packrat cache to only lookup rule id and index in node_list (requires above)
 *      fix token to only start a pointer to start of string and the length of the token. it will basically be another non-null terminated string with extra metadata
 */


/* Rule definitions and declarations */

#define Rule_DEFAULT_INIT {._class = &Rule_class, \
                           .id = Rule_DEFAULT_ID \
                           }
#define RuleType_DEFAULT_INIT 

//typedef struct Rule Rule; // handled in <peggy/utils.h>
typedef struct RuleType RuleType;

struct Rule {
    RuleType * _class;   /* internal */
    rule_id_type id;            /* public */
#ifndef NDEBUG
    size_t ncalls;
    size_t nevals;
#endif
};



extern struct RuleType {
    char const * type_name;
    Rule * (*new)(int);
    err_type (*init)(Rule * self, int id);
    void (*dest)(Rule * self);
    void (*del)(Rule * self);
    ASTNode * (*check_rule_)(Rule * self, Parser * parser, size_t token_key);
    ASTNode * (*check)(Rule * self, Parser * parser);
} Rule_class;

Rule * Rule_new(int id);
err_type Rule_init(Rule * self, int id);
void Rule_dest(Rule * self);
void Rule_del(Rule * self);
ASTNode * Rule_check_rule_(Rule * self, Parser * parser, size_t token_key);
ASTNode * Rule_check(Rule * self, Parser * parser);

/* update in C and metadata */

/* ChainRule definitions and declarations */

#define ChainRule_DEFAULT_INIT {.Rule = { \
                                    ._class = &(ChainRule_class.Rule_class), \
                                    id = Rule_DEFAULT_ID, \
                                }, \
                                ._class = &ChainRule_class, \
                                .deps = NULL, \
                                .deps_size = 0 \
                                }

typedef struct ChainRule ChainRule;

struct ChainRule {
    Rule Rule;
    struct ChainRuleType * _class;
    Rule ** deps;
    size_t deps_size;
};

extern struct ChainRuleType {
    struct RuleType Rule_class;
    ChainRule * (*new)(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    err_type (*init)(ChainRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    void (*dest)(ChainRule * self);
    void (*del)(ChainRule * self);
} ChainRule_class;

ChainRule * ChainRule_new(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
err_type ChainRule_init(ChainRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
void ChainRule_dest(ChainRule * self);
void ChainRule_del(ChainRule * self);
void ChainRule_as_Rule_del(Rule * chain_rule);

/* SequenceRule definitions and declarations */

#define SequenceRule_DEFAULT_INIT { .ChainRule = { \
                                        .Rule = { \
                                            ._class = &SequenceRule_class.ChainRule_class.Rule_class, \
                                            .id = Rule_DEFAULT_ID, \
                                        }, \
                                        ._class = &SequenceRule_class.ChainRule_class, \
                                        .deps = NULL, \
                                        .deps_size = 0 \
                                    }, \
                                    ._class = &SequenceRule_class \
                                    }
          
typedef struct SequenceRule SequenceRule;

struct SequenceRule {
    ChainRule ChainRule;
    struct SequenceRuleType * _class;
};

extern struct SequenceRuleType {
    struct ChainRuleType ChainRule_class;
    SequenceRule * (*new)(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    err_type (*init)(SequenceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    void (*dest)(SequenceRule * self);
    void (*del)(SequenceRule * self);
} SequenceRule_class;

SequenceRule * SequenceRule_new(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
err_type SequenceRule_init(SequenceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
void SequenceRule_dest(SequenceRule * self);
void SequenceRule_del(SequenceRule * self);
void SequenceRule_as_ChainRule_del(ChainRule * sequence_chain);
void SequenceRule_as_Rule_del(Rule * sequence_rule);
//err_type SequenceRule_build(Rule * sequence_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * SequenceRule_check_rule_(Rule * sequence_rule, Parser * parser, size_t token_key);

/* ChoiceRule definitions and declarations */

#define ChoiceRule_DEFAULT_INIT {   .ChainRule = { \
                                        .Rule = { \
                                            ._class = &ChoiceRule_class.ChainRule_class.Rule_class, \
                                            .id = Rule_DEFAULT_ID, \
                                        }, \
                                        ._class = &ChoiceRule_class.ChainRule_class, \
                                        .deps = NULL, \
                                        .deps_size = 0 \
                                    }, \
                                    ._class = &ChoiceRule_class \
                                    }
          
typedef struct ChoiceRule ChoiceRule;

struct ChoiceRule {
    ChainRule ChainRule;
    struct ChoiceRuleType * _class;
};

extern struct ChoiceRuleType {
    struct ChainRuleType ChainRule_class;
    ChoiceRule * (*new)(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    err_type (*init)(ChoiceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
    void (*dest)(ChoiceRule * self);
    void (*del)(ChoiceRule * self);
} ChoiceRule_class;

ChoiceRule * ChoiceRule_new(rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
err_type ChoiceRule_init(ChoiceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]);
void ChoiceRule_dest(ChoiceRule * self);
void ChoiceRule_del(ChoiceRule * self);
void ChoiceRule_as_ChainRule_del(ChainRule * choice_chain);
void ChoiceRule_as_Rule_del(Rule * choice_rule);
//err_type ChoiceRule_build(Rule * choice_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * ChoiceRule_check_rule_(Rule * choice_rule, Parser * parser, size_t token_key);

/* LiteralRule class definitions and declarations */

/* this captures both StringRule and RegexRule from peggen.py since I only ever use regex anyway. punctuators have to go through a regex cleanup */

#define LiteralRule_DEFAULT_INIT {  .Rule = { \
                                        ._class = &LiteralRule_class.Rule_class, \
                                        .id = Rule_DEFAULT_ID, \
                                    }, \
                                    ._class = &LiteralRule_class, \
                                    .regex_s = NULL, \
                                    .compiled = false, \
                                    }
#define LiteralRuleType_DEFAULT_INIT 
          
typedef struct LiteralRule LiteralRule;

struct LiteralRule {
    Rule Rule;
    struct LiteralRuleType * _class;
#ifdef Linux
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
    LiteralRule * (*new)(rule_id_type id, char const * regex_s);
    err_type (*init)(LiteralRule * self, rule_id_type id, char const * regex_s);
    err_type (*compile)(LiteralRule * self);
    void (*dest)(LiteralRule * self);
    void (*del)(LiteralRule * self);
} LiteralRule_class;

LiteralRule * LiteralRule_new(rule_id_type id, char const * regex_s);
err_type LiteralRule_init(LiteralRule * self, rule_id_type id, char const * regex_s);
void LiteralRule_dest(LiteralRule * self);
void LiteralRule_del(LiteralRule * self);
err_type LiteralRule_compile_regex(LiteralRule * self);
void LiteralRule_as_Rule_del(Rule * literal_rule);
void LiteralRule_as_Rule_dest(Rule * literal_rule);
//err_type LiteralRule_build(Rule * literal_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * LiteralRule_check_rule_(Rule * literal_rule, Parser * parser, size_t token_key);

/* DerivedRule abstract class definitions and declarations */

#define DerivedRule_DEFAULT_INIT {  .Rule = { \
                                        ._class = &DerivedRule_class.Rule_class, \
                                        .id = Rule_DEFAULT_ID, \
                                    }, \
                                    ._class = &DerivedRule_class, \
                                    .rule = NULL \
                                    }
#define DerivedRuleType_DEFAULT_INIT 

typedef struct DerivedRule DerivedRule;

struct DerivedRule {
    Rule Rule;
    struct DerivedRuleType * _class;
    Rule * rule;
};

extern struct DerivedRuleType {
    struct RuleType Rule_class;
    DerivedRule * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(DerivedRule * self, rule_id_type id, Rule * rule);
    void (*dest)(DerivedRule * self);
    void (*del)(DerivedRule * self);
} DerivedRule_class;

DerivedRule * DerivedRule_new(rule_id_type id, Rule * rule);
err_type DerivedRule_init(DerivedRule * self, rule_id_type id, Rule * rule);
void DerivedRule_dest(DerivedRule * self);
void DerivedRule_del(DerivedRule * self);
void DerivedRule_as_Rule_del(Rule * derived_rule);

/* ListRule class definitions and declarations */

#define ListRule_DEFAULT_INIT { .DerivedRule = {\
                                    .Rule = { \
                                        ._class = &ListRule_class.DerivedRule_class.Rule_class, \
                                        .id = Rule_DEFAULT_ID, \
                                    }, \
                                    ._class = &ListRule_class.DerivedRule_class, \
                                    .rule = NULL \
                                }, \
                                ._class = &ListRule_class, \
                                .delim = NULL \
                                }
#define ListRuleType_DEFAULT_INIT 

typedef struct ListRule ListRule;

struct ListRule {
    DerivedRule DerivedRule;
    struct ListRuleType * _class;
    Rule * delim;
};

extern struct ListRuleType {
    struct DerivedRuleType DerivedRule_class;
    ListRule * (*new)(rule_id_type id, Rule * rule, Rule * delim);
    err_type (*init)(ListRule * self, rule_id_type id, Rule * rule, Rule * delim);
    void (*dest)(ListRule * self);
    void (*del)(ListRule * self);
} ListRule_class;

ListRule * ListRule_new(rule_id_type id, Rule * rule, Rule * delim);
err_type ListRule_init(ListRule * self, rule_id_type id, Rule * rule, Rule * delim);
void ListRule_dest(ListRule * self);
void ListRule_del(ListRule * self);
void ListRule_as_DerivedRule_del(DerivedRule * list_rule);
void ListRule_as_Rule_del(Rule * list_rule);
//err_type ListRule_build(Rule * list_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * ListRule_check_rule_(Rule * list_rule, Parser * parser, size_t token_key);

/* RepeatRule class definitions and declarations */

/* TODO: HERE...need to fix the members of repeat rule to include min_rep and max_rep*/

#define RepeatRule_DEFAULT_INIT {   .DerivedRule = {\
                                        .Rule = { \
                                            ._class = &(RepeatRule_class.DerivedRule_class.Rule_class), \
                                            .id = Rule_DEFAULT_ID, \
                                        }, \
                                        ._class = &(RepeatRule_class.DerivedRule_class), \
                                        .rule = NULL \
                                    }, \
                                    ._class = &RepeatRule_class, \
                                    .min_rep = 0, \
                                    .max_rep = 0 \
                                    }
#define RepeatRuleType_DEFAULT_INIT 

typedef struct RepeatRule RepeatRule;

struct RepeatRule {
    DerivedRule DerivedRule;
    struct RepeatRuleType * _class;
    size_t min_rep;
    size_t max_rep;
};

extern struct RepeatRuleType {
    struct DerivedRuleType DerivedRule_class;
    RepeatRule * (*new)(rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep);
    err_type (*init)(RepeatRule * self, rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep);
    void (*dest)(RepeatRule * self);
    void (*del)(RepeatRule * self);
} RepeatRule_class;

RepeatRule * RepeatRule_new(rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep);
err_type RepeatRule_init(RepeatRule * selfp, rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep);
void RepeatRule_dest(RepeatRule * self);
void RepeatRule_del(RepeatRule * self);
void RepeatRule_as_DerivedRule_del(DerivedRule * repeat_rule);
void RepeatRule_as_Rule_del(Rule * repeat_rule);
//err_type RepeatRule_build(Rule * repeat_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * RepeatRule_check_rule_(Rule * repeat_rule, Parser * parser, size_t token_key);

/* NegativeLookahead Rule class definitions and declarations */

#define NegativeLookahead_DEFAULT_INIT { \
                                        .DerivedRule = {\
                                            .Rule = { \
                                                ._class = &NegativeLookahead_class.DerivedRule_class.Rule_class, \
                                                .id = Rule_DEFAULT_ID, \
                                            }, \
                                            ._class = &NegativeLookahead_class.DerivedRule_class, \
                                            .rule = NULL \
                                        }, \
                                        ._class = &NegativeLookahead_class, \
                                        }

typedef struct NegativeLookahead NegativeLookahead;

struct NegativeLookahead {
    DerivedRule DerivedRule;
    struct NegativeLookaheadType * _class;
};

extern struct NegativeLookaheadType {
    struct DerivedRuleType DerivedRule_class;
    NegativeLookahead * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(NegativeLookahead * self, rule_id_type id, Rule * rule);
    void (*dest)(NegativeLookahead * self);
    void (*del)(NegativeLookahead * self);
} NegativeLookahead_class;

NegativeLookahead * NegativeLookahead_new(rule_id_type id, Rule * rule);
err_type NegativeLookahead_init(NegativeLookahead * self, rule_id_type id, Rule * rule);
void NegativeLookahead_dest(NegativeLookahead * self);
void NegativeLookahead_del(NegativeLookahead * self);
void NegativeLookahead_as_DerivedRule_del(DerivedRule * negative_lookahead);
void NegativeLookahead_as_Rule_del(Rule * negative_lookahead);
//err_type NegativeLookahead_build(Rule * negative_lookahead, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * NegativeLookahead_check_rule_(Rule * negative_lookahead, Parser * parser, size_t token_key);

/* PositiveLookahead Rule definitions and declarations */

#define PositiveLookahead_DEFAULT_INIT {.DerivedRule = {\
                                            .Rule = { \
                                                ._class = &PositiveLookahead_class.DerivedRule_class.Rule_class, \
                                                .id = Rule_DEFAULT_ID, \
                                            }, \
                                            ._class = &PositiveLookahead_class.DerivedRule_class, \
                                            .rule = NULL \
                                        }, \
                                        ._class = &PositiveLookahead_class, \
                                        }

typedef struct PositiveLookahead PositiveLookahead;

struct PositiveLookahead {
    DerivedRule DerivedRule;
    struct PositiveLookaheadType * _class;
};

extern struct PositiveLookaheadType {
    struct DerivedRuleType DerivedRule_class;
    PositiveLookahead * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(PositiveLookahead * self, rule_id_type id, Rule * rule);
    void (*dest)(PositiveLookahead * self);
    void (*del)(PositiveLookahead * self);
} PositiveLookahead_class;

PositiveLookahead * PositiveLookahead_new(rule_id_type id, Rule * rule);
err_type PositiveLookahead_init(PositiveLookahead * self, rule_id_type id, Rule * rule);
void PositiveLookahead_dest(PositiveLookahead * self);
void PositiveLookahead_del(PositiveLookahead * self);
void PositiveLookahead_as_DerivedRule_del(DerivedRule * positive_lookahead);
void PositiveLookahead_as_Rule_del(Rule * positive_lookahead);
//err_type PositiveLookahead_build(Rule * positive_lookahead, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * PositiveLookahead_check_rule_(Rule * positive_lookahead, Parser * parser, size_t token_key);
/* Production Rule abstract class definitions and declarations */

#define Production_DEFAULT_INIT {   .DerivedRule = { \
                                        .Rule = { \
                                            ._class = &Production_class.DerivedRule_class.Rule_class, \
                                            .id = Rule_DEFAULT_ID, \
                                        }, \
                                        ._class = &Production_class.DerivedRule_class, \
                                        .rule = NULL \
                                    }, \
                                    ._class = &Production_class, \
                                    .build_action = &build_action_default, \
                                    }

typedef struct Production Production;

ASTNode * build_action_default(Production * production, Parser * parser, ASTNode * node);
typedef ASTNode * (*build_action_ftype)(Production * production, Parser * parser, ASTNode *node);

struct Production {
    DerivedRule DerivedRule;
    struct ProductionType * _class;
    build_action_ftype build_action;
};

extern struct ProductionType {
    struct DerivedRuleType DerivedRule_class;
    Production * (*new)(rule_id_type id, Rule * rule, build_action_ftype build_action);
    err_type (*init)(Production * self, rule_id_type id, Rule * rule, build_action_ftype build_action);
    void (*dest)(Production * self);
    void (*del)(Production * self);
} Production_class;

Production * Production_new(rule_id_type id, Rule * rule, build_action_ftype build_action);
err_type Production_init(Production * self, rule_id_type id, Rule * rule, build_action_ftype build_action);
void Production_dest(Production * self);
void Production_del(Production * self);
void Production_as_DerivedRule_del(DerivedRule * production);
void Production_as_Rule_del(Rule * production);
//err_type Production_build(Rule * production, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * Production_check_rule_(Rule * production, Parser * parser, size_t token_key);

#endif // PEGGY_RULE_H
