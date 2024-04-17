#ifndef PEGGY_RULE_H
#define PEGGY_RULE_H

/* C STD LIB */
#include <stddef.h>

/* POSIX */
#include <regex.h>

#include <peggy/utils.h>
#include <peggy/parser_gen.h>
#include <peggy/astnode.h>
#include <peggy/token.h>

/**
 * Consider:
 *      PackratCache has a different interface from a normal hash map and so should be a different structure
 * 
 * TODO:
 *      need to add a "name" to each rule for printing
 *      use an arena allocator in the node
 *
 */

/* PackratCache setup */
#define KEY_TYPE size_t
#define VALUE_TYPE pASTNode
#define KEY_COMP uint_comp
#define HASH_FUNC uint_hash
#include <peggy/hash_map.h>

typedef HASH_MAP(size_t, pASTNode) SAMap, * pSAMap;

int pToken_comp(pToken a, pToken b);
size_t pToken_hash(pToken a, size_t hash_size);

#define KEY_TYPE pToken
#define VALUE_TYPE pSAMap
#define KEY_COMP pToken_comp
#define HASH_FUNC pToken_hash
#include <peggy/hash_map.h>

typedef HASH_MAP(pToken, pSAMap) PackratCache;

/* end PackratCache setup */

/* Rule definitions and declarations */

#define Rule_DEFAULT_INIT {._class = &Rule_class, \
                           .id = Rule_DEFAULT_ID}
#define RuleType_DEFAULT_INIT {._class = &Rule_TYPE,\
                               .new = &Rule_new,\
                               .init = &Rule_init,\
                               .dest = &Rule_dest, \
                               .del = &Rule_del,\
                               .build = &Rule_build,\
                               .cache_check_ = &Rule_cache_check_,\
                               .check_cache_ = &Rule_check_cache_,\
                               .clear_cache = &Rule_clear_cache,\
                               .check_rule_ = &Rule_check_rule_,\
                               .check = &Rule_check}

//typedef struct Rule Rule; // handled in <peggy/utils.h>

struct Rule {
    struct RuleType * _class;   /* internal */
    rule_id_type id;            /* public */
    PackratCache cache_;        /* protected */
};

extern struct RuleType {
    Type const * _class;
    Rule * (*new)(int);
    err_type (*init)(Rule * self, int id);
    void (*dest)(Rule * self);
    void (*del)(Rule * self);
    err_type (*build)(Rule * self, ParserGenerator * pg, char * buffer, size_t buf_size);
    void (*cache_check_)(Rule * self, Parser * parser, size_t token_key, ASTNode * result);
    ASTNode * (*check_cache_)(Rule * self, Parser * parser, size_t token_key);
    void (*clear_cache)(Rule * self, Token * tok);
    ASTNode * (*check_rule_)(Rule * self, Parser * parser, size_t token_key, bool disable_cache_check);
    ASTNode * (*check)(Rule * self, Parser * parser, bool disable_cache_check);
} Rule_class;

Rule * Rule_new(int id);
err_type Rule_init(Rule * self, int id);
void Rule_dest(Rule * self);
void Rule_del(Rule * self);
err_type Rule_build(Rule * self, ParserGenerator * pg, char * buffer, size_t buffer_size);
void Rule_cache_check_(Rule * self, Parser * parser, size_t token_key, ASTNode * result);
ASTNode * Rule_check_cache_(Rule * self, Parser * parser, size_t token_key);
void Rule_clear_cache(Rule * self, Token * tok);
ASTNode * Rule_check_rule_(Rule * self, Parser * parser, size_t token_key, bool disable_cache_check);
ASTNode * Rule_check(Rule * self, Parser * parser, bool disable_cache_check);

/* update in C and metadata */

/* ChainRule definitions and declarations */

#define ChainRule_DEFAULT_INIT {._class = &ChainRule_class, \
                                .Rule = {._class = &(ChainRule_class.Rule_class), \
                                        .id = Rule_DEFAULT_ID \
                                }, \
                                .deps = NULL, \
                                .deps_size = 0 \
                                }
#define ChainRuleType_DEFAULT_INIT {._class = &ChainRule_TYPE, \
                                    .Rule_class = { \
                                        ._class = &ChainRule_TYPE,\
                                        .new = &Rule_new,\
                                        .init = &Rule_init,\
                                        .dest = &Rule_dest, \
                                        .del = &ChainRule_as_Rule_del,\
                                        .build = &Rule_build,\
                                        .cache_check_ = &Rule_cache_check_,\
                                        .check_cache_ = &Rule_check_cache_,\
                                        .clear_cache = &ChainRule_clear_cache,\
                                        .check_rule_ = &Rule_check_rule_,\
                                        .check = &Rule_check \
                                    }, \
                                    .new = &ChainRule_new, \
                                    .init = &ChainRule_init, \
                                    .dest = &ChainRule_dest, \
                                    .del = &ChainRule_del \
                                    }

typedef struct ChainRule ChainRule;

struct ChainRule {
    struct ChainRuleType * _class;
    Rule Rule;
    Rule * deps;
    size_t deps_size;
};

extern struct ChainRuleType {
    Type const * _class;
    struct RuleType Rule_class;
    ChainRule * (*new)(rule_id_type id, size_t deps_size, Rule deps[deps_size]);
    err_type (*init)(ChainRule * self, rule_id_type id, size_t deps_size, Rule deps[deps_size]);
    void (*dest)(ChainRule * self);
    void (*del)(ChainRule * self);
} ChainRule_class;

ChainRule * ChainRule_new(rule_id_type id, size_t deps_size, Rule deps[deps_size]);
err_type ChainRule_init(ChainRule * self, rule_id_type id, size_t deps_size, Rule deps[deps_size]);
void ChainRule_dest(ChainRule * self);
void ChainRule_del(ChainRule * self);
void ChainRule_as_Rule_del(Rule * chain_rule);
void ChainRule_clear_cache(Rule * chain_rule, Token * tok);

/* SequenceRule definitions and declarations */

#define SequenceRule_DEFAULT_INIT {._class = &SequenceRule_class, \
                                    .ChainRule = { \
                                        ._class = &(SequenceRule_class.ChainRule_class), \
                                        .Rule = { \
                                            ._class = &(SequenceRule_class.ChainRule_class.Rule_class), \
                                            .id = Rule_DEFAULT_ID \
                                        }, \
                                        .deps = NULL, \
                                        .deps_size = 0 \
                                    } \
                                    }
#define SequenceRuleType_DEFAULT_INIT {._class = &SequenceRule_TYPE, \
                                        .ChainRule_class = { \
                                            ._class = &SequenceRule_TYPE, \
                                            .Rule_class = { \
                                                ._class = &SequenceRule_TYPE,\
                                                .new = &Rule_new,\
                                                .init = &Rule_init,\
                                                .dest = &Rule_dest, \
                                                .del = &SequenceRule_as_Rule_del,\
                                                .build = &SequenceRule_build,\
                                                .cache_check_ = &Rule_cache_check_,\
                                                .check_cache_ = &Rule_check_cache_,\
                                                .clear_cache = &ChainRule_clear_cache,\
                                                .check_rule_ = &SequenceRule_check_rule_,\
                                                .check = &Rule_check \
                                            }, \
                                            .new = &ChainRule_new, \
                                            .init = &ChainRule_init, \
                                            .dest = &ChainRule_dest, \
                                            .del = &SequenceRule_as_ChainRule_del}, \
                                        .new = &SequenceRule_new, \
                                        .init = &SequenceRule_init, \
                                        .dest = &SequenceRule_dest, \
                                        .del = &SequenceRule_del \
                                        }
          
typedef struct SequenceRule SequenceRule;

struct SequenceRule {
    struct SequenceRuleType * _class;
    ChainRule ChainRule;
};

extern struct SequenceRuleType {
    Type const * _class;
    struct ChainRuleType ChainRule_class;
    SequenceRule * (*new)(rule_id_type id, size_t deps_size, Rule deps[deps_size]);
    err_type (*init)(SequenceRule * self, rule_id_type id, size_t deps_size, Rule deps[deps_size]);
    void (*dest)(SequenceRule * self);
    void (*del)(SequenceRule * self);
} SequenceRule_class;

SequenceRule * SequenceRule_new(rule_id_type id, size_t deps_size, Rule deps[deps_size]);
err_type SequenceRule_init(SequenceRule * self, rule_id_type id, size_t deps_size, Rule deps[deps_size]);
void SequenceRule_dest(SequenceRule * self);
void SequenceRule_del(SequenceRule * self);
void SequenceRule_as_ChainRule_del(ChainRule * sequence_chain);
void SequenceRule_as_Rule_del(Rule * sequence_rule);
err_type SequenceRule_build(Rule * sequence_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * SequenceRule_check_rule_(Rule * sequence_rule, Parser * parser, size_t token_key, bool disable_cache_check);

/* ChoiceRule definitions and declarations */

#define ChoiceRule_DEFAULT_INIT {._class = &ChoiceRule_class, \
                                    .ChainRule = { \
                                        ._class = &(ChoiceRule_class.ChainRule_class), \
                                        .Rule = { \
                                            ._class = &(ChoiceRule_class.ChainRule_class.Rule_class), \
                                            .id = Rule_DEFAULT_ID \
                                        }, \
                                        .deps = NULL, \
                                        .deps_size = 0 \
                                    } \
                                    }
#define ChoiceRuleType_DEFAULT_INIT {._class = &ChoiceRule_TYPE, \
                                      .ChainRule_class = { \
                                        ._class = &ChoiceRule_TYPE, \
                                        .Rule_class = { \
                                            ._class = &ChoiceRule_TYPE,\
                                            .new = &Rule_new,\
                                            .init = &Rule_init,\
                                            .dest = &Rule_dest, \
                                            .del = &ChoiceRule_as_Rule_del,\
                                            .build = &ChoiceRule_build,\
                                            .cache_check_ = &Rule_cache_check_,\
                                            .check_cache_ = &Rule_check_cache_,\
                                            .clear_cache = &Rule_clear_cache,\
                                            .check_rule_ = &ChoiceRule_check_rule_,\
                                            .check = &Rule_check \
                                        }, \
                                        .new = &ChainRule_new, \
                                        .init = &ChainRule_init, \
                                        .dest = &ChainRule_dest, \
                                        .del = &ChoiceRule_as_ChainRule_del}, \
                                      .new = &ChoiceRule_new, \
                                      .init = &ChoiceRule_init, \
                                      .dest = &ChoiceRule_dest, \
                                      .del = &ChoiceRule_del \
                                      }
          
typedef struct ChoiceRule ChoiceRule;

struct ChoiceRule {
    struct ChoiceRuleType * _class;
    ChainRule ChainRule;
};

extern struct ChoiceRuleType {
    Type const * _class;
    struct ChainRuleType ChainRule_class;
    ChoiceRule * (*new)(rule_id_type id, size_t deps_size, Rule deps[deps_size]);
    err_type (*init)(ChoiceRule * self, rule_id_type id, size_t deps_size, Rule deps[deps_size]);
    void (*dest)(ChoiceRule * self);
    void (*del)(ChoiceRule * self);
} ChoiceRule_class;

ChoiceRule * ChoiceRule_new(rule_id_type id, size_t deps_size, Rule deps[deps_size]);
err_type ChoiceRule_init(ChoiceRule * self, rule_id_type id, size_t deps_size, Rule deps[deps_size]);
void ChoiceRule_dest(ChoiceRule * self);
void ChoiceRule_del(ChoiceRule * self);
void ChoiceRule_as_ChainRule_del(ChainRule * choice_chain);
void ChoiceRule_as_Rule_del(Rule * choice_rule);
err_type ChoiceRule_build(Rule * choice_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * ChoiceRule_check_rule_(Rule * choice_rule, Parser * parser, size_t token_key, bool disable_cache_check);

/* LiteralRule class definitions and declarations */

/* this captures both StringRule and RegexRule from peggen.py since I only ever use regex anyway. punctuators have to go through a regex cleanup */

#define LiteralRule_DEFAULT_INIT {._class = &LiteralRule_class, \
                                    .Rule = { \
                                        ._class = &(LiteralRule_class.Rule_class), \
                                        .id = Rule_DEFAULT_ID \
                                    }, \
                                    }
#define LiteralRuleType_DEFAULT_INIT {._class = &LiteralRule_TYPE, \
                                        .Rule_class = { \
                                            ._class = &LiteralRule_TYPE,\
                                            .new = &Rule_new,\
                                            .init = &Rule_init,\
                                            .dest = &Rule_dest, \
                                            .del = &LiteralRule_as_Rule_del,\
                                            .build = &LiteralRule_build,\
                                            .cache_check_ = &Rule_cache_check_,\
                                            .check_cache_ = &Rule_check_cache_,\
                                            .clear_cache = &Rule_clear_cache,\
                                            .check_rule_ = &LiteralRule_check_rule_,\
                                            .check = &Rule_check \
                                        }, \
                                        .new = &LiteralRule_new, \
                                        .init = &LiteralRule_init, \
                                        .dest = &LiteralRule_dest, \
                                        .del = &LiteralRule_del \
                                        }
          
typedef struct LiteralRule LiteralRule;

struct LiteralRule {
    struct LiteralRuleType * _class;
    Rule Rule;
    regex_t regex;
};

extern struct LiteralRuleType {
    Type const * _class;
    struct RuleType Rule_class;
    LiteralRule * (*new)(rule_id_type id, regex_t regex);
    err_type (*init)(LiteralRule * self, rule_id_type id, regex_t regex);
    void (*dest)(LiteralRule * self);
    void (*del)(LiteralRule * self);
} LiteralRule_class;

LiteralRule * LiteralRule_new(rule_id_type id, regex_t regex);
err_type LiteralRule_init(LiteralRule * self, rule_id_type id, regex_t regex);
void LiteralRule_dest(LiteralRule * self);
void LiteralRule_del(LiteralRule * self);
void LiteralRule_as_Rule_del(Rule * literal_rule);
err_type LiteralRule_build(Rule * literal_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * LiteralRule_check_rule_(Rule * literal_rule, Parser * parser, size_t token_key, bool disable_cache_check);

/* NamedProduction Rule class definitions and declarations */

#define NamedProduction_DEFAULT_INIT {._class = &NamedProduction_class, \
                                        .Rule = { \
                                            ._class = &(NamedProduction_class.Rule_class), \
                                            .id = Rule_DEFAULT_ID \
                                        }, \
                                        }
#define NamedProductionType_DEFAULT_INIT {._class = &NamedProduction_TYPE, \
                                            .Rule_class = { \
                                                ._class = &NamedProduction_TYPE,\
                                                .new = &Rule_new,\
                                                .init = &Rule_init,\
                                                .dest = &Rule_dest, \
                                                .del = &NamedProduction_as_Rule_del,\
                                                .build = &NamedProduction_build,\
                                                .cache_check_ = &Rule_cache_check_,\
                                                .check_cache_ = &Rule_check_cache_,\
                                                .clear_cache = &Rule_clear_cache,\
                                                .check_rule_ = &Rule_check_rule_,\
                                                .check = &Rule_check \
                                            }, \
                                            .new = &NamedProduction_new, \
                                            .init = &NamedProduction_init, \
                                            .dest = &NamedProduction_dest, \
                                            .del = &NamedProduction_del \
                                            }

typedef struct NamedProduction NamedProduction;

struct NamedProduction {
    struct NamedProductionType * _class;
    Rule Rule;
};

extern struct NamedProductionType {
    Type const * _class;
    struct RuleType Rule_class;
    NamedProduction * (*new)(rule_id_type id);
    err_type (*init)(NamedProduction * self, rule_id_type id);
    void (*dest)(NamedProduction * self);
    void (*del)(NamedProduction * self);
} NamedProduction_class;

NamedProduction * NamedProduction_new(rule_id_type id);
err_type NamedProduction_init(NamedProduction * self, rule_id_type id);
void NamedProduction_dest(NamedProduction * self);
void NamedProduction_del(NamedProduction * self);
void NamedProduction_as_Rule_del(Rule * named_production_rule);
err_type NamedProduction_build(Rule * named_production, ParserGenerator * pg, char * buffer, size_t buffer_size);

/* DerivedRule abstract class definitions and declarations */

#define DerivedRule_DEFAULT_INIT {._class = &DerivedRule_class, \
                                    .Rule = { \
                                        ._class = &(DerivedRule_class.Rule_class), \
                                        .id = Rule_DEFAULT_ID \
                                    }, \
                                    .rule = NULL \
                                    }
#define DerivedRuleType_DEFAULT_INIT {._class = &DerivedRule_TYPE, \
                                        .Rule_class = { \
                                            ._class = &DerivedRule_TYPE,\
                                            .new = &Rule_new,\
                                            .init = &Rule_init,\
                                            .dest = &Rule_dest, \
                                            .del = &DerivedRule_as_Rule_del,\
                                            .build = &Rule_build,\
                                            .cache_check_ = &Rule_cache_check_,\
                                            .check_cache_ = &Rule_check_cache_,\
                                            .clear_cache = &DerivedRule_clear_cache,\
                                            .check_rule_ = &Rule_check_rule_,\
                                            .check = &Rule_check \
                                        }, \
                                        .new = &DerivedRule_new, \
                                        .init = &DerivedRule_init, \
                                        .dest = &DerivedRule_dest, \
                                        .del = &DerivedRule_del \
                                        }

typedef struct DerivedRule DerivedRule;

struct DerivedRule {
    struct DerivedRuleType * _class;
    Rule Rule;
    Rule * rule;
};

extern struct DerivedRuleType {
    Type const * _class;
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
void DerivedRule_clear_cache(Rule * derived_rule, Token * tok);

/* ListRule class definitions and declarations */

#define ListRule_DEFAULT_INIT {._class = &ListRule_class, \
                                .DerivedRule = {\
                                    ._class = &(ListRule_class.DerivedRule_class), \
                                    .Rule = { \
                                        ._class = &(ListRule_class.DerivedRule_class.Rule_class), \
                                        .id = Rule_DEFAULT_ID \
                                    }, \
                                    .rule = NULL \
                                }, \
                                .delim = NULL \
                                }
#define ListRuleType_DEFAULT_INIT {._class = &ListRule_TYPE, \
                                   .DerivedRule_class = { \
                                        ._class = &ListRule_TYPE, \
                                        .Rule_class = { \
                                            ._class = &ListRule_TYPE,\
                                            .new = &Rule_new,\
                                            .init = &Rule_init,\
                                            .dest = &Rule_dest, \
                                            .del = &ListRule_as_Rule_del,\
                                            .build = &ListRule_build,\
                                            .cache_check_ = &Rule_cache_check_,\
                                            .check_cache_ = &Rule_check_cache_,\
                                            .clear_cache = &ListRule_clear_cache,\
                                            .check_rule_ = &ListRule_check_rule_,\
                                            .check = &Rule_check\
                                            }, \
                                        .new = &DerivedRule_new, \
                                        .init = &DerivedRule_init, \
                                        .dest = &DerivedRule_dest, \
                                        .del = &ListRule_as_DerivedRule_del \
                                        }, \
                                   .new = &ListRule_new, \
                                   .init = &ListRule_init, \
                                   .dest = &ListRule_dest, \
                                   .del = &ListRule_del \
                                    }

typedef struct ListRule ListRule;

struct ListRule {
    struct ListRuleType * _class;
    DerivedRule DerivedRule;
    Rule * delim;
};

extern struct ListRuleType {
    Type const * _class;
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
err_type ListRule_build(Rule * list_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * ListRule_check_rule_(Rule * list_rule, Parser * parser, size_t token_key, bool disable_cache_check);
void ListRule_clear_cache(Rule * list_rule, Token * tok);

/* RepeatRule class definitions and declarations */

/* TODO: HERE...need to fix the members of repeat rule to include min_rep and max_rep*/

#define RepeatRule_DEFAULT_INIT {._class = &RepeatRule_class, \
                                    .DerivedRule = {\
                                        ._class = &(RepeatRule_class.DerivedRule_class), \
                                        .Rule = { \
                                            ._class = &(RepeatRule_class.DerivedRule_class.Rule_class), \
                                            .id = Rule_DEFAULT_ID \
                                        }, \
                                        .rule = NULL \
                                    }, \
                                    .min_rep = 0, \
                                    .max_rep = 0 \
                                    }
#define RepeatRuleType_DEFAULT_INIT {._class = &RepeatRule_TYPE, \
                                    .DerivedRule_class = { \
                                        ._class = &RepeatRule_TYPE, \
                                        .Rule_class = { \
                                            ._class = &RepeatRule_TYPE,\
                                            .new = &Rule_new,\
                                            .init = &Rule_init,\
                                            .dest = &Rule_dest, \
                                            .del = &RepeatRule_as_Rule_del,\
                                            .build = &RepeatRule_build,\
                                            .cache_check_ = &Rule_cache_check_,\
                                            .check_cache_ = &Rule_check_cache_,\
                                            .clear_cache = &DerivedRule_clear_cache,\
                                            .check_rule_ = &RepeatRule_check_rule_,\
                                            .check = &Rule_check \
                                        }, \
                                        .new = &DerivedRule_new, \
                                        .init = &DerivedRule_init, \
                                        .dest = &DerivedRule_dest, \
                                        .del = &RepeatRule_as_DerivedRule_del \
                                    }, \
                                   .new = &RepeatRule_new, \
                                   .init = &RepeatRule_init, \
                                   .dest = &RepeatRule_dest, \
                                   .del = &RepeatRule_del \
                                    }

typedef struct RepeatRule RepeatRule;

struct RepeatRule {
    struct RepeatRuleType * _class;
    DerivedRule DerivedRule;
    size_t min_rep;
    size_t max_rep;
};

extern struct RepeatRuleType {
    Type const * _class;
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
err_type RepeatRule_build(Rule * repeat_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * RepeatRule_check_rule_(Rule * repeat_rule, Parser * parser, size_t token_key, bool disable_cache_check);

/* OptionalRule class definitions and declarations */

#define OptionalRule_DEFAULT_INIT {._class = &OptionalRule_class, \
                                    .DerivedRule = {\
                                        ._class = &(OptionalRule_class.DerivedRule_class), \
                                        .Rule = { \
                                            ._class = &(OptionalRule_class.DerivedRule_class.Rule_class), \
                                            .id = Rule_DEFAULT_ID \
                                        }, \
                                        .rule = NULL \
                                    } \
                                    }
#define OptionalRuleType_DEFAULT_INIT {._class = &OptionalRule_TYPE, \
                                        .DerivedRule_class = { \
                                            ._class = &OptionalRule_TYPE, \
                                            .Rule_class = { \
                                                ._class = &OptionalRule_TYPE,\
                                                .new = &Rule_new,\
                                                .init = &Rule_init,\
                                                .dest = &Rule_dest, \
                                                .del = &OptionalRule_as_Rule_del,\
                                                .build = &OptionalRule_build,\
                                                .cache_check_ = &Rule_cache_check_,\
                                                .check_cache_ = &Rule_check_cache_,\
                                                .clear_cache = &DerivedRule_clear_cache,\
                                                .check_rule_ = &OptionalRule_check_rule_,\
                                                .check = &Rule_check\
                                            }, \
                                            .new = &DerivedRule_new, \
                                            .init = &DerivedRule_init, \
                                            .dest = &DerivedRule_dest, \
                                            .del = &OptionalRule_as_DerivedRule_del \
                                        }, \
                                        .new = &OptionalRule_new, \
                                        .init = &OptionalRule_init, \
                                        .dest = &OptionalRule_dest, \
                                        .del = &OptionalRule_del \
                                        }

typedef struct OptionalRule OptionalRule;

struct OptionalRule {
    struct OptionalRuleType * _class;
    DerivedRule DerivedRule;
};

extern struct OptionalRuleType {
    Type const * _class;
    struct DerivedRuleType DerivedRule_class;
    OptionalRule * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(OptionalRule * self, rule_id_type id, Rule * rule);
    void (*dest)(OptionalRule * self);
    void (*del)(OptionalRule * self);
} OptionalRule_class;

OptionalRule * OptionalRule_new(rule_id_type id, Rule * rule);
err_type OptionalRule_init(OptionalRule * self, rule_id_type id, Rule * rule);
void OptionalRule_dest(OptionalRule * self);
void OptionalRule_del(OptionalRule * self);
void OptionalRule_as_DerivedRule_del(DerivedRule * optional_rule);
void OptionalRule_as_Rule_del(Rule * optional_rule);
err_type OptionalRule_build(Rule * optional_rule, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * OptionalRule_check_rule_(Rule * optional_rule, Parser * parser, size_t token_key, bool disable_cache_check);

/* NegativeLookahead Rule class definitions and declarations */

#define NegativeLookahead_DEFAULT_INIT {._class = &NegativeLookahead_class, \
                                        .DerivedRule = {\
                                            ._class = &(NegativeLookahead_class.DerivedRule_class), \
                                            .Rule = { \
                                                ._class = &(NegativeLookahead_class.DerivedRule_class.Rule_class), \
                                                .id = Rule_DEFAULT_ID \
                                            }, \
                                            .rule = NULL \
                                        } \
                                        }
#define NegativeLookaheadType_DEFAULT_INIT {._class = &NegativeLookahead_TYPE, \
                                            .DerivedRule_class = { \
                                                ._class = &NegativeLookahead_TYPE, \
                                                .Rule_class = { \
                                                    ._class = &NegativeLookahead_TYPE,\
                                                    .new = &Rule_new,\
                                                    .init = &Rule_init,\
                                                    .dest = &Rule_dest, \
                                                    .del = &NegativeLookahead_as_Rule_del,\
                                                    .build = &NegativeLookahead_build,\
                                                    .cache_check_ = &Rule_cache_check_,\
                                                    .check_cache_ = &Rule_check_cache_,\
                                                    .clear_cache = &DerivedRule_clear_cache,\
                                                    .check_rule_ = &NegativeLookahead_check_rule_,\
                                                    .check = &Rule_check\
                                                }, \
                                                .new = &DerivedRule_new, \
                                                .init = &DerivedRule_init, \
                                                .dest = &DerivedRule_dest, \
                                                .del = &NegativeLookahead_as_DerivedRule_del \
                                            }, \
                                            .new = &NegativeLookahead_new, \
                                            .init = &NegativeLookahead_init, \
                                            .dest = &NegativeLookahead_dest, \
                                            .del = &NegativeLookahead_del \
                                            }

typedef struct NegativeLookahead NegativeLookahead;

struct NegativeLookahead {
    struct NegativeLookaheadType * _class;
    DerivedRule DerivedRule;
};

extern struct NegativeLookaheadType {
    Type const * _class;
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
err_type NegativeLookahead_build(Rule * negative_lookahead, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * NegativeLookahead_check_rule_(Rule * negative_lookahead, Parser * parser, size_t token_key, bool disable_cache_check);

/* PositiveLookahead Rule definitions and declarations */

#define PositiveLookahead_DEFAULT_INIT {._class = &PositiveLookahead_class, \
                                        .DerivedRule = {\
                                            ._class = &(PositiveLookahead_class.DerivedRule_class), \
                                            .Rule = { \
                                                ._class = &(PositiveLookahead_class.DerivedRule_class.Rule_class), \
                                                .id = Rule_DEFAULT_ID \
                                            }, \
                                            .rule = NULL \
                                        } \
                                        }
#define PositiveLookaheadType_DEFAULT_INIT {._class = &PositiveLookahead_TYPE, \
                                            .DerivedRule_class = { \
                                                ._class = &PositiveLookahead_TYPE, \
                                                .Rule_class = { \
                                                    ._class = &PositiveLookahead_TYPE,\
                                                    .new = &Rule_new,\
                                                    .init = &Rule_init,\
                                                    .dest = &Rule_dest, \
                                                    .del = &PositiveLookahead_as_Rule_del,\
                                                    .build = &PositiveLookahead_build,\
                                                    .cache_check_ = &Rule_cache_check_,\
                                                    .check_cache_ = &Rule_check_cache_,\
                                                    .clear_cache = &DerivedRule_clear_cache,\
                                                    .check_rule_ = &PositiveLookahead_check_rule_,\
                                                    .check = &Rule_check\
                                                }, \
                                                .new = &DerivedRule_new, \
                                                .init = &DerivedRule_init, \
                                                .dest = &DerivedRule_dest, \
                                                .del = &PositiveLookahead_as_DerivedRule_del \
                                            }, \
                                            .new = &PositiveLookahead_new, \
                                            .init = &PositiveLookahead_init, \
                                            .dest = &PositiveLookahead_dest, \
                                            .del = &PositiveLookahead_del \
                                            }

typedef struct PositiveLookahead PositiveLookahead;

struct PositiveLookahead {
    struct PositiveLookaheadType * _class;
    DerivedRule DerivedRule;
};

extern struct PositiveLookaheadType {
    Type const * _class;
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
err_type PositiveLookahead_build(Rule * positive_lookahead, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * PositiveLookahead_check_rule_(Rule * positive_lookahead, Parser * parser, size_t token_key, bool disable_cache_check);

/* AnonymousProduction Rule abstract class definitions and declarations */

#define AnonymousProduction_DEFAULT_INIT {._class = &AnonymousProduction_class, \
                                            .DerivedRule = {\
                                                ._class = &(AnonymousProduction_class.DerivedRule_class), \
                                                .Rule = { \
                                                    ._class = &(AnonymousProduction_class.DerivedRule_class.Rule_class), \
                                                    .id = Rule_DEFAULT_ID \
                                                }, \
                                                .rule = NULL \
                                            } \
                                            }
#define AnonymousProductionType_DEFAULT_INIT {._class = &AnonymousProduction_TYPE, \
                                                .DerivedRule_class = { \
                                                    ._class = &AnonymousProduction_TYPE, \
                                                    .Rule_class = { \
                                                        ._class = &AnonymousProduction_TYPE,\
                                                        .new = &Rule_new,\
                                                        .init = &Rule_init,\
                                                        .dest = &Rule_dest, \
                                                        .del = &AnonymousProduction_as_Rule_del,\
                                                        .build = &AnonymousProduction_build,\
                                                        .cache_check_ = &Rule_cache_check_,\
                                                        .check_cache_ = &Rule_check_cache_,\
                                                        .clear_cache = &DerivedRule_clear_cache,\
                                                        .check_rule_ = &Rule_check_rule_,\
                                                        .check = &Rule_check \
                                                    }, \
                                                    .new = &DerivedRule_new, \
                                                    .init = &DerivedRule_init, \
                                                    .dest = &DerivedRule_dest, \
                                                    .del = &AnonymousProduction_as_DerivedRule_del \
                                                }, \
                                                .new = &AnonymousProduction_new, \
                                                .init = &AnonymousProduction_init, \
                                                .dest = &AnonymousProduction_dest, \
                                                .del = &AnonymousProduction_del \
                                                }

typedef struct AnonymousProduction AnonymousProduction;

struct AnonymousProduction {
    struct AnonymousProductionType * _class;
    DerivedRule DerivedRule;
};

extern struct AnonymousProductionType {
    Type const * _class;
    struct DerivedRuleType DerivedRule_class;
    AnonymousProduction * (*new)(rule_id_type id, Rule * rule);
    err_type (*init)(AnonymousProduction * self, rule_id_type id, Rule * rule);
    void (*dest)(AnonymousProduction * self);
    void (*del)(AnonymousProduction * self);
} AnonymousProduction_class;

AnonymousProduction * AnonymousProduction_new(rule_id_type id, Rule * rule);
err_type AnonymousProduction_init(AnonymousProduction * self, rule_id_type id, Rule * rule);
void AnonymousProduction_dest(AnonymousProduction * self);
void AnonymousProduction_del(AnonymousProduction * self);
void AnonymousProduction_as_DerivedRule_del(DerivedRule * anonymous_production);
void AnonymousProduction_as_Rule_del(Rule * anonymous_production);
err_type AnonymousProduction_build(Rule * anonymous_production, ParserGenerator * pg, char * buffer, size_t buffer_size);

/* Production Rule abstract class definitions and declarations */

ASTNode * build_action_default(Parser * parser, ASTNode * node);

typedef ASTNode * (*build_action_ftype)(Parser * parser, ASTNode *node);

#define Production_DEFAULT_INIT {._class = &Production_class, \
                                    .AnonymousProduction = { \
                                        ._class = &(Production_class.AnonymousProduction_class), \
                                        .DerivedRule = {\
                                            ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class), \
                                            .Rule = { \
                                                ._class = &(Production_class.AnonymousProduction_class.DerivedRule_class.Rule_class), \
                                                .id = Rule_DEFAULT_ID \
                                            }, \
                                            .rule = NULL \
                                        } \
                                    }, \
                                    .build_action = &build_action_default, \
                                    }
#define ProductionType_DEFAULT_INIT  {._class = &Production_TYPE, \
                                        .AnonymousProduction_class = { \
                                            ._class = &Production_TYPE, \
                                            .DerivedRule_class = { \
                                                ._class = &Production_TYPE, \
                                                .Rule_class = { \
                                                    ._class = &Production_TYPE,\
                                                    .new = &Rule_new,\
                                                    .init = &Rule_init,\
                                                    .dest = &Rule_dest, \
                                                    .del = &Production_as_Rule_del,\
                                                    .build = &Production_build,\
                                                    .cache_check_ = &Rule_cache_check_,\
                                                    .check_cache_ = &Rule_check_cache_,\
                                                    .clear_cache = &DerivedRule_clear_cache,\
                                                    .check_rule_ = &Production_check_rule_,\
                                                    .check = &Rule_check \
                                                    }, \
                                                .new = &DerivedRule_new, \
                                                .init = &DerivedRule_init, \
                                                .dest = &DerivedRule_dest, \
                                                .del = &Production_as_DerivedRule_del \
                                            }, \
                                            .new = &AnonymousProduction_new, \
                                            .init = &AnonymousProduction_init, \
                                            .dest = &AnonymousProduction_dest, \
                                            .del = &Production_as_AnonymousProduction_del \
                                        }, \
                                        .new = &Production_new, \
                                        .init = &Production_init, \
                                        .dest = &Production_dest, \
                                        .del = &Production_del \
                                        }

typedef struct Production Production;

struct Production {
    struct ProductionType * _class;
    AnonymousProduction AnonymousProduction;
    build_action_ftype build_action;
};

extern struct ProductionType {
    Type const * _class;
    struct AnonymousProductionType AnonymousProduction_class;
    Production * (*new)(rule_id_type id, Rule * rule, build_action_ftype build_action);
    err_type (*init)(Production * self, rule_id_type id, Rule * rule, build_action_ftype build_action);
    void (*dest)(Production * self);
    void (*del)(Production * self);
} Production_class;

Production * Production_new(rule_id_type id, Rule * rule, build_action_ftype build_action);
err_type Production_init(Production * self, rule_id_type id, Rule * rule, build_action_ftype build_action);
void Production_dest(Production * self);
void Production_del(Production * self);
void Production_as_AnonymousProduction_del(AnonymousProduction * production);
void Production_as_DerivedRule_del(DerivedRule * production);
void Production_as_Rule_del(Rule * production);
err_type Production_build(Rule * production, ParserGenerator * pg, char * buffer, size_t buffer_size);
ASTNode * Production_check_rule_(Rule * production, Parser * parser, size_t token_key, bool disable_cache_check);

#endif // PEGGY_RULE_H
