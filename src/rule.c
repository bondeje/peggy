
/* C STD LIB */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> // for unhandled error printfs

/* POSIX */
#include <regex.h>

#include <peggy/utils.h>
#include <peggy/type.h>
#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/token.h>
#include <peggy/hash_map.h>

/* PackratCache wrapper implementations */

#define PACKRAT_CACHE_INIT_SIZE_OUTER 3
#define PACKRAT_CACHE_INIT_SIZE_INNER 0

int pToken_comp(pToken a, pToken b) {
    return address_comp(a->string, b->string);
}

size_t pToken_hash(pToken a, size_t hash_size) {
    return address_hash(a->string, hash_size);
}

/*
// probably do not need
bool PackratCache_in(PackratCache * cache, Token * tok);
hash_map_err PackratCache_pop(PackratCache * cache, Token * tok, ASTNode ** node);
hash_map_err PackratCache_remove(PackratCache * cache, Token * tok);
void PackratCache_for_each(PackratCache * cache, int (*handle_item)(void * data, KEY_TYPE key, VALUE_TYPE value), void * data);
*/
hash_map_err PackratCache_init(PackratCache * cache) {
    return HASH_MAP_INIT(pToken, pSAMap)(cache, PACKRAT_CACHE_INIT_SIZE_OUTER);
}
hash_map_err PackratCache_get(PackratCache * cache, Token * tok, ASTNode ** node) {
    hash_map_err status = HM_SUCCESS;
    SAMap * psamap; /* map tok->start : ASTNode * */
    if ((status = cache->_class->get(cache, tok, &psamap))) { /* retrieves map tok->string : (map tok->start : ASTNode *) and checks status */
        return status;
    }
    return psamap->_class->get(psamap, tok->start, node);
}
hash_map_err PackratCache_set(PackratCache * cache, Token * tok, ASTNode * node) {
    SAMap * psamap; /* map tok->start : ASTNode * */
    hash_map_err status = cache->_class->get(cache, tok, &psamap); /* retrieves map tok->string : (map tok->start : ASTNode *) and checks status */
    switch (status) {
        case HM_KEY_NOT_FOUND: { /* need to make a new SAMap and insert it*/
            psamap = malloc(sizeof(*psamap));
            if (!psamap) {
                return HM_MALLOC_MAP_FAILURE;
            }
            HASH_MAP_INIT(size_t, pASTNode)(psamap, PACKRAT_CACHE_INIT_SIZE_INNER);
            cache->_class->set(cache, tok, psamap);
            /* psamap is now valid, fall through */
        }
            __attribute__((fallthrough)); // otherwise gcc -pedantic complains about fallthrough
        case HM_SUCCESS: { /* psamap is valid, simply insert new size_t : ASTNode * into samap */
            psamap->_class->set(psamap, tok->start, node);
            break;
        }
        default: {
            return status;
        }
    }
    return HM_SUCCESS;
}

int PackratCache_handle_ASTNode(void * data, size_t loc, ASTNode * node) {
    if (node->children) {
        free(node->children);
        node->children = NULL;
        node->nchildren = 0;
    }
    node->_class->del(node);
    return 0;
}

void PackratCache_clear_token(PackratCache * cache, Token * tok) { /* semantically clears out the cache for the particular string being parsed*/
    //printf("clearing token\n");

    if (!cache->capacity) {
        return;
    }
    SAMap * psamap;
    hash_map_err status = cache->_class->pop(cache, tok, &psamap);
    //psamap->_class->for_each(psamap, &PackratCache_handle_ASTNode, NULL); // trying to clear out ASTNodes
    if (status == HM_KEY_NOT_FOUND) {
        return; // no-op
    }
    // psamap should be valid. get can only return HM_KEY_NOT_FOUND or HM_SUCCESS

    /* clear out ASTNodes *. cannot use this. causes use after free issues */
    //psamap->_class->for_each(psamap, &PackratCache_handle_ASTNode, NULL);

    psamap->_class->dest(psamap); // clear the internal map. NOTE: this leaves all ASTNode *s in heap memory 
    free(psamap); // the psamap was allocated
    //printf("done clearing token\n");
    
}

int PackratCache_clear_handle(void * data, Token * tok, SAMap * inner_map) {
    //printf("clearing inner cache associated with token");
    //Token_print(tok);
    //printf("\n");
    inner_map->_class->dest(inner_map);
    free(inner_map);
    return 0;
}
void PackratCache_dest(PackratCache * cache) {
    //printf("clearing cache\n");
    cache->_class->for_each(cache, &PackratCache_clear_handle, NULL);
    cache->_class->dest(cache);
}

/* Rule implementations */

/* Type system metadata for Rule */
#define Rule_NAME "Rule"

Type const Rule_TYPE = {._class = &Type_class,
                        .type_name = Rule_NAME};
/* END Type system metadata for Rule */

/* for tracking rule IDs and equality */
int rule_id = 0;

struct RuleType Rule_class = RuleType_DEFAULT_INIT;

/* id = -1 means none provided */
Rule * Rule_new(int id) {
    Rule * ret = malloc(sizeof(Rule));
    if (!ret) {
        return NULL;
    }
    *ret = (Rule) Rule_DEFAULT_INIT;
    if (Rule_init(ret, id)) {
        free(ret);
        return NULL;
    }
    return ret;
}

/* id = -1 means none provided */
err_type Rule_init(Rule * self, int id) {
    self->id = id;
    if (PackratCache_init(&self->cache_)) {
        return PEGGY_FAILURE;
    }
    return PEGGY_SUCCESS;
}

void Rule_dest(Rule * self) {
    //printf("destroying rule id %d\n", self->id);
    PackratCache_dest(&(self->cache_));
}

void Rule_del(Rule * self) {
    /* TODO: all children need to call a finalizer of some kind */
    Rule_dest(self);
    free(self);
}

/*
err_type Rule_build(Rule * self, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    return PEGGY_NOT_IMPLEMENTED;
}
*/

void Rule_cache_check_(Rule * self, Parser * parser, size_t token_key, ASTNode * result) {
    //printf("caching result for rule id %d with token starting at %zu\n", self->id, token_key);
    if (!self->cache_.capacity) {
        PackratCache_init(&(self->cache_));
    }
    Token * tok = NULL;
    err_type status = parser->_class->get(parser, token_key, &tok);
    if (status) {
        printf("unhandled error (%d) from token lookup in parser: Rule_cache_check_\n", status);
    }
    hash_map_err hm_status = PackratCache_set(&(self->cache_), tok, result);
    if (hm_status) {
        printf("unhandled error (%d) while saving in cache in parser: Rule_cache_check_\n", hm_status);
    }
}

ASTNode * Rule_check_cache_(Rule * self, Parser * parser, size_t token_key) {
    //printf("checking cache\n");
    if (!self->cache_.capacity) {
        return NULL; // cache is empty, cannot possibly find a result
    }
    Token * tok = NULL;
    err_type status = parser->_class->get(parser, token_key, &tok);
    if (status) {
        printf("unhandled error (%d) from token lookup in parser: Rule_check_cache_\n", status);
    }
    ASTNode * res = NULL;
    hash_map_err hm_status = PackratCache_get(&(self->cache_), tok, &res);
    if (hm_status == HM_KEY_NOT_FOUND) {
        return NULL;
    }
    if (hm_status) {
        printf("unhandled error (%d) while retrieving from cache in parser: Rule_check_cache_\n", hm_status);
    }
    if (res) { /* should be unnecessary as NULLs should never going into the cache */
        parser->_class->seek(parser, res->ntokens, P_SEEK_CUR);
    }
    return res;
}

void Rule_clear_cache(Rule * self, Token * tok) {
    //printf("clearing token from rule cache with id: %d:", self->id);
    //Token_print(tok);
    //printf("\n");
    PackratCache_clear_token(&(self->cache_), tok);
}

/* TODO: probably should change this so that Rule_check_rule_ output node is in arguments and ASTNode_fail becomes an error code */
ASTNode * Rule_check_rule_(Rule * self, Parser * parser, size_t token_key, bool disable_cache_check) {
    /* Not to be implemented */
    return NULL;
}

/* user does not need to check if result is not ASTNode_fail if they use ASTNode_del function */
/* TODO: probably should change this so that Rule_check output node is in arguments and ASTNode_fail becomes an error code */
ASTNode * Rule_check(Rule * self, Parser * parser, bool disable_cache_check) {
    //printf("checking Rule id: %d (type: %s) %s @%zu\n", self->id, self->_class->_class->type_name, disable_cache_check ? "(cache_check disabled)" : "", parser->loc_);
    size_t token_key = parser->_class->tell(parser);
    ASTNode * res = NULL;
    if (!disable_cache_check) {
        res = self->_class->check_cache_(self, parser, token_key);
        if (res) {
            //printf("successfully retrieved result of rule id %d from cache at token_key %zu\n", self->id, token_key);
            return res;
        }
    }
    // check the rule and generate a node
    res = self->_class->check_rule_(self, parser, token_key, disable_cache_check);
    if (self->id == 69) {
        //printf("initial entry finding\n");
        // break point
        //exit(EXIT_FAILURE);
    }
    // cache the result
    self->_class->cache_check_(self, parser, token_key, res);

    /* TODO: log the result */

    if (res == &ASTNode_fail) {
        //printf("rule %d failed!\n", self->id);
        // reset the parser seek because the rule check failed
        parser->_class->seek(parser, token_key, P_SEEK_SET);
    }// else {
    //    printf("rule %d passed!\n", self->id);
    //}
    return res;
}

/* ChainRule implementations */

/* Type system metadata for ChainRule */
#define ChainRule_NAME "ChainRule.Rule"
//StructInfo ChainRule_INFO = {0};
Type const ChainRule_TYPE = {//._info = &ChainRule_INFO,
                               ._class = &Type_class,
                               .type_name = ChainRule_NAME};
/* END Type system metadata for ChainRule */

struct ChainRuleType ChainRule_class = ChainRuleType_DEFAULT_INIT;

ChainRule * ChainRule_new(rule_id_type id, size_t deps_size, Rule * deps[deps_size]) {
    ChainRule * ret = malloc(sizeof(ChainRule));
    if (!ret) {
        return NULL;
    }
    *ret = (ChainRule) ChainRule_DEFAULT_INIT;
    if (ChainRule_init(ret, id, deps_size, deps)) {
        free(ret);
        return NULL;
    }
    return ret;
}
err_type ChainRule_init(ChainRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]) {
    self->_class->Rule_class.init(&(self->Rule), id);
    self->deps_size = deps_size;
    self->deps = deps;
    return PEGGY_SUCCESS;
}
void ChainRule_dest(ChainRule * self) {
    Rule_dest(&(self->Rule));
}

void ChainRule_del(ChainRule * self) {
    ChainRule_dest(self);
    free(self);
}

void ChainRule_as_Rule_del(Rule * chain_rule) {
    ChainRule_del(DOWNCAST_P(chain_rule, Rule, ChainRule));
}

void ChainRule_clear_cache(Rule * chain_rule, Token * tok) {
    //printf("clearing token from chain rule cache with id: %d:", chain_rule->id);
    //Token_print(tok);
    //printf("\n");
    ChainRule * self = DOWNCAST_P(chain_rule, Rule, ChainRule);
    for (size_t i = 0; i < self->deps_size; i++) {
        self->deps[i]->_class->clear_cache(self->deps[i], tok);
    }
    Rule_class.clear_cache(chain_rule, tok);
}

/* SequenceRule implementations */
/* NOTE: was "And" in the python version */

/* Type system metadata for SequenceRule */
#define SequenceRule_NAME "SequenceRule.ChainRule.Rule"
//StructInfo SequenceRule_INFO = {0};
Type const SequenceRule_TYPE = {//._info = &SequenceRule_INFO,
                       ._class = &Type_class,
                       .type_name = SequenceRule_NAME
                       };
/* END Type system metadata for SequenceRule */

struct SequenceRuleType SequenceRule_class = SequenceRuleType_DEFAULT_INIT;

SequenceRule * SequenceRule_new(rule_id_type id, size_t deps_size, Rule * deps[deps_size]) {
    SequenceRule * ret = malloc(sizeof(SequenceRule));
    if (!ret) {
        return NULL;
    }
    *ret = (SequenceRule) SequenceRule_DEFAULT_INIT;
    if (SequenceRule_init(ret, id, deps_size, deps)) {
        free(ret);
        return NULL;
    }
    return ret;
}

err_type SequenceRule_init(SequenceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]) {
    self->_class->ChainRule_class.init(&(self->ChainRule), id, deps_size, deps);
    return PEGGY_SUCCESS;
}

void SequenceRule_dest(SequenceRule * self) {
    ChainRule_dest(&(self->ChainRule));
}

void SequenceRule_del(SequenceRule * self) {
    SequenceRule_dest(self);
    free(self);
}

void SequenceRule_as_ChainRule_del(ChainRule * sequence_chain) {
    SequenceRule_del(DOWNCAST_P(sequence_chain, ChainRule, SequenceRule));
}

void SequenceRule_as_Rule_del(Rule * sequence_rule) {
    SequenceRule_del(DOWNCAST_P(DOWNCAST_P(sequence_rule, Rule, ChainRule), ChainRule, SequenceRule));
}

/*
err_type SequenceRule_build(Rule * sequence_rule, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    SequenceRule * self = DOWNCAST_P(DOWNCAST_P(sequence_rule, Rule, ChainRule), ChainRule, SequenceRule);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

ASTNode * SequenceRule_check_rule_(Rule * sequence_rule, Parser * parser, size_t token_key, bool disable_cache_check) {
    //printf("checking sequence rule. id: %d\n", sequence_rule->id);
    SequenceRule * self = DOWNCAST_P(DOWNCAST_P(sequence_rule, Rule, ChainRule), ChainRule, SequenceRule);
    ASTNode ** children = malloc(sizeof(ASTNode *) * self->ChainRule.deps_size);
    if (!children) {
        return &ASTNode_fail;
    }
    size_t nchildren = 0;
    for (size_t i = 0; i < self->ChainRule.deps_size; i++) {
        ASTNode * child_res = self->ChainRule.deps[i]->_class->check(self->ChainRule.deps[i], parser, disable_cache_check);
        if (child_res == &ASTNode_fail) {
            //printf("sequence rule id %d failed at index %zu\n", sequence_rule->id, i);
            free(children);
            return child_res;
        }// else {
            //printf("index %zu in sequence %d succeeds\n", i, sequence_rule->id);
        //}
        if (!is_skip_node(child_res)) {
            children[nchildren++] = child_res;
        }
    }
    size_t token_cur = parser->_class->tell(parser);
    Token * cur = NULL;
    err_type status = parser->_class->get(parser, token_cur, &cur);
    if (status) {
        printf("unhandled error (%d) in SequenceRule_check_rule_ getting current token from parser\n", status);
    }
    Token * start = NULL;
    status = parser->_class->get(parser, token_key, &start);
    if (status) {
        printf("unhandled error (%d) in SequenceRule_check_rule_ getting starting token from parser\n", status);
    }
    ASTNode * res = ASTNode_class.new(sequence_rule, token_key, token_cur - token_key, cur->start - start->start, nchildren, children);
    //if (res) {
    //    printf("sequence rule (%d) check passed (%zu\n", sequence_rule->id, self->ChainRule.deps_size);
    //}
    parser->_class->add_node(parser, res);
    return res;
}

/* ChoiceRule implementations */

/* Type system metadata for ChoiceRule */
#define ChoiceRule_NAME "ChoiceRule.ChainRule.Rule"
//StructInfo ChoiceRule_INFO = {0};
Type const ChoiceRule_TYPE = {//._info = &ChoiceRule_INFO,
                            ._class = &Type_class,
                            .type_name = ChoiceRule_NAME
                            };
/* END Type system metadata for ChoiceRule */

struct ChoiceRuleType ChoiceRule_class = ChoiceRuleType_DEFAULT_INIT;

ChoiceRule * ChoiceRule_new(rule_id_type id, size_t deps_size, Rule * deps[deps_size]) {
    ChoiceRule * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (ChoiceRule) ChoiceRule_DEFAULT_INIT;
    if (ChoiceRule_init(ret, id, deps_size, deps)) {
        free(ret);
        return NULL;
    }
    return ret;
}

err_type ChoiceRule_init(ChoiceRule * self, rule_id_type id, size_t deps_size, Rule * deps[deps_size]) {
    self->_class->ChainRule_class.init(&(self->ChainRule), id, deps_size, deps);
    return PEGGY_SUCCESS;
}

void ChoiceRule_dest(ChoiceRule * self) {
    ChainRule_dest(&(self->ChainRule));
}

void ChoiceRule_del(ChoiceRule * self) {
    ChoiceRule_dest(self);
    free(self);
}

void ChoiceRule_as_ChainRule_del(ChainRule * choice_chain) {
    ChoiceRule_del(DOWNCAST_P(choice_chain, ChainRule, ChoiceRule));
}

void ChoiceRule_as_Rule_del(Rule * choice_rule) {
    ChoiceRule_del(DOWNCAST_P(DOWNCAST_P(choice_rule, Rule, ChainRule), ChainRule, ChoiceRule));
}

/*
err_type ChoiceRule_build(Rule * choice_rule, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    ChoiceRule * self = DOWNCAST_P(DOWNCAST_P(choice_rule, Rule, ChainRule), ChainRule, ChoiceRule);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

ASTNode * ChoiceRule_check_rule_(Rule * choice_rule, Parser * parser, size_t token_key, bool disable_cache_check) {
    //printf("checking choice rule. id: %d\n", choice_rule->id);
    ChoiceRule * self = DOWNCAST_P(DOWNCAST_P(choice_rule, Rule, ChainRule), ChainRule, ChoiceRule);
    for (size_t i = 0; i < self->ChainRule.deps_size; i++) {
        ASTNode * child_res = self->ChainRule.deps[i]->_class->check(self->ChainRule.deps[i], parser, disable_cache_check);
        if (child_res != &ASTNode_fail) {
            return child_res;
        }
    }
    //printf("choice rule with id %d failed\n", choice_rule->id);
    return &ASTNode_fail;
}

/* LiteralRule implementations */

/* Type system metadata for LiteralRule */
#define LiteralRule_NAME "LiteralRule.Rule"
//StructInfo LiteralRule_INFO = {0};
Type const LiteralRule_TYPE = {//._info = &LiteralRule_INFO,
                            ._class = &Type_class,
                            .type_name = LiteralRule_NAME
                            };
/* END Type system metadata for LiteralRule */

struct LiteralRuleType LiteralRule_class = LiteralRuleType_DEFAULT_INIT;

LiteralRule * LiteralRule_new(rule_id_type id, char const * regex_s) {
    LiteralRule * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (LiteralRule) LiteralRule_DEFAULT_INIT;
    if (LiteralRule_init(ret, id, regex_s)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}

err_type LiteralRule_compile_regex(LiteralRule * self) {
    //printf("compiling regex %s\n", self->regex_s);
    if (regcomp(&(self->regex), self->regex_s, REG_EXTENDED)) {
        return PEGGY_REGEX_FAILURE;
    }
    self->compiled = true;
    return PEGGY_SUCCESS;
}

err_type LiteralRule_init(LiteralRule * self, rule_id_type id, char const * regex_s) {
    self->_class->Rule_class.init(&(self->Rule), id);
    self->regex_s = regex_s;
    self->compiled = false;
    if (regex_s) {
        LiteralRule_compile_regex(self);
    }
    return PEGGY_SUCCESS;
}

void LiteralRule_dest(LiteralRule * self) {
    //printf("freeing regex data for rule id %d\n", self->Rule.id);
    regfree(&(self->regex));
    self->compiled = false;
    Rule_dest(&(self->Rule));
}

void LiteralRule_del(LiteralRule * self) {
    LiteralRule_dest(self);
    free(self);
}

void LiteralRule_as_Rule_del(Rule * literal_rule) {
    LiteralRule_del(DOWNCAST_P(literal_rule, Rule, LiteralRule));
}

void LiteralRule_as_Rule_dest(Rule * literal_rule) {
    LiteralRule_dest(DOWNCAST_P(literal_rule, Rule, LiteralRule));
}

/*
err_type LiteralRule_build(Rule * literal_rule, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    LiteralRule * self = DOWNCAST_P(literal_rule, Rule, LiteralRule);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

ASTNode * LiteralRule_check_rule_(Rule * literal_rule, Parser * parser, size_t token_key, bool disable_cache_check) {
    LiteralRule * self = DOWNCAST_P(literal_rule, Rule, LiteralRule);
    //printf("checking literal rule. id: %d, %s %s, token: ", literal_rule->id, self->regex_s, parser->disable_cache_check ? "cache check disabled" : "");
    err_type status = PEGGY_SUCCESS;
    if (!self->compiled) {
        if ((status = LiteralRule_compile_regex(self))) {
            return &ASTNode_fail; // failed to compile the regular expression
        }
    }
    Token * tok = NULL;
    status = parser->_class->get(parser, token_key, &tok);
    //printf("back in LiteralRule_check_rule_");
    //Token_print(tok);
    //printf("\n");
    if (status) {
        //printf("failed to get token_key %zu from parser tokens list of length %zu\n", token_key, parser->tokens_length);
        printf("unhandled error (%d) in LiteralRule_check_rule_ getting current token from parser\n", status);
    }
    // since the parser->_class->get might have triggered
    //if (!parser->disable_cache_check) { // 
    //    ASTNode * res = literal_rule->_class->check_cache_(literal_rule, parser, token_key);
    //}
    if (tok->_class->len(tok) == 0) { // token retrieved is empty. This shouldn't really happen
        return &ASTNode_fail;
    }
    if (!regexec(&(self->regex), tok->string + tok->start, LITERAL_N_MATCHES, self->match, 0)) {
        // the regex succeed to match
        parser->_class->seek(parser, 1, P_SEEK_CUR);
        size_t length = self->match[0].rm_eo - self->match[0].rm_so;
        //printf("LiteralRule regex matched with length %zu!\n", length);
        ASTNode * res = ASTNode_class.new(literal_rule, token_key, length ? 1 : 0, length, 0, NULL);
        parser->_class->add_node(parser, res);
        return res;
    }
    return &ASTNode_fail;
}

/* NamedProduction implementations */

/* Type system metadata for NamedProduction */
#define NamedProduction_NAME "NamedProduction.Rule"
//StructInfo NamedProduction_INFO = {0};
Type const NamedProduction_TYPE = {//._info = &NamedProduction_INFO,
                            ._class = &Type_class,
                            .type_name = NamedProduction_NAME
                            };
/* END Type system metadata for NamedProduction */

struct NamedProductionType NamedProduction_class = NamedProductionType_DEFAULT_INIT;

NamedProduction * NamedProduction_new(rule_id_type id) {
    NamedProduction * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (NamedProduction) NamedProduction_DEFAULT_INIT;
    if (NamedProduction_init(ret, id)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}

err_type NamedProduction_init(NamedProduction * self, rule_id_type id) {
    self->_class->Rule_class.init(&(self->Rule), id);
    return PEGGY_SUCCESS;
}

void NamedProduction_dest(NamedProduction * self) {
    Rule_dest(&(self->Rule));
}

void NamedProduction_del(NamedProduction * self) {
    NamedProduction_dest(self);
    free(self);
}

void NamedProduction_as_Rule_del(Rule * named_production_rule) {
    NamedProduction_del(DOWNCAST_P(named_production_rule, Rule, NamedProduction));
}

/*
err_type NamedProduction_build(Rule * named_production, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    NamedProduction * self = DOWNCAST_P(named_production, Rule, NamedProduction);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

/* DerivedRule implementations */

/* Type system metadata for DerivedRule */
#define DerivedRule_NAME "Rule.DerivedRule"
//StructInfo DerivedRule_INFO = {0};
Type const DerivedRule_TYPE = {//._info = &DerivedRule_INFO,
                            ._class = &Type_class,
                            .type_name = DerivedRule_NAME
                            };
/* END Type system metadata for DerivedRule */

struct DerivedRuleType DerivedRule_class = DerivedRuleType_DEFAULT_INIT;

DerivedRule * DerivedRule_new(rule_id_type id, Rule * rule) {
    DerivedRule * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (DerivedRule) DerivedRule_DEFAULT_INIT;
    if (DerivedRule_init(ret, id, rule)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}

err_type DerivedRule_init(DerivedRule * self, rule_id_type id, Rule * rule) {
    self->_class->Rule_class.init(&(self->Rule), id);
    self->rule = rule;
    return PEGGY_SUCCESS;
}

void DerivedRule_dest(DerivedRule * self) {
    Rule_dest(&(self->Rule));
}

void DerivedRule_del(DerivedRule * self) {
    DerivedRule_dest(self);
    free(self);
}

void DerivedRule_as_Rule_del(Rule * derived_rule) {
    DerivedRule_del(DOWNCAST_P(derived_rule, Rule, DerivedRule));
}

void DerivedRule_clear_cache(Rule * derived_rule, Token * tok) {
    //printf("clearing token from derived rule cache with id: %d:", derived_rule->id);
    //Token_print(tok);
    //printf("\n");
    DerivedRule * self = DOWNCAST_P(derived_rule, Rule, DerivedRule);
    self->rule->_class->clear_cache(self->rule, tok);
    Rule_class.clear_cache(derived_rule, tok);
}

/* ListRule implementations */

/* Type system metadata for ListRule */
#define ListRule_NAME "ListRule.DerivedRule.Rule"
//StructInfo ListRule_INFO = {0};
Type const ListRule_TYPE = {//._info = &ListRule_INFO,
                            ._class = &Type_class,
                            .type_name = ListRule_NAME
                            };
/* END Type system metadata for ListRule */

struct ListRuleType ListRule_class = ListRuleType_DEFAULT_INIT;

ListRule * ListRule_new(rule_id_type id, Rule * rule, Rule * delim) {
    ListRule * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (ListRule) ListRule_DEFAULT_INIT;
    if (ListRule_init(ret, id, rule, delim)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}
err_type ListRule_init(ListRule * self, rule_id_type id, Rule * rule, Rule * delim) {
    self->_class->DerivedRule_class.init(&(self->DerivedRule), id, rule);
    self->delim = delim;
    return PEGGY_SUCCESS;
}
void ListRule_dest(ListRule * self) {
    DerivedRule_dest(&(self->DerivedRule));
}
void ListRule_del(ListRule * self) {
    ListRule_dest(self);
    free(self);
}
void ListRule_as_DerivedRule_del(DerivedRule * list_rule) {
    ListRule_del(DOWNCAST_P(list_rule, DerivedRule, ListRule));
}
void ListRule_as_Rule_del(Rule * list_rule) {
    ListRule_del(DOWNCAST_P(DOWNCAST_P(list_rule, Rule, DerivedRule), DerivedRule, ListRule));
}

/*
err_type ListRule_build(Rule * list_rule, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    return PEGGY_NOT_IMPLEMENTED;
}
*/

ASTNode * ListRule_check_rule_(Rule * list_rule, Parser * parser, size_t token_key, bool disable_cache_check) {
    //printf("checking list rule. id: %d\n", list_rule->id);
    ListRule * self = DOWNCAST_P(DOWNCAST_P(list_rule, Rule, DerivedRule), DerivedRule, ListRule);
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser, disable_cache_check);
    if (node == &ASTNode_fail) {
        return node;
    }
    // TODO: when stack is available, replace with stack API
    size_t capacity = 3; // MAGIC_NUMBER. Frequently only one will ever be found but 2x will be common. Avoid realloc
    ASTNode ** node_list = malloc(sizeof(*node_list) * capacity);
    if (!node_list) {
        return &ASTNode_fail;
    }
    size_t node_list_length = 0;
    node_list[node_list_length++] = node;
    while (true) { 
        ASTNode * delim = self->delim->_class->check(self->delim, parser, disable_cache_check);
        if (delim == &ASTNode_fail) {
            //printf("delimiter failed with list length %zu\n", node_list_length);
            break;
        }
        if (node_list_length >= capacity - 1) { // realloc more space. Always make sure it can handle a delim and node in one pass
            size_t new_capacity = 4 * capacity + 1; // because of above comment, it should always have odd capacity
            ASTNode ** new_node_list = realloc(node_list, sizeof(*node_list) * new_capacity);
            if (!new_node_list) {
                free(node_list);
                return &ASTNode_fail;
            }
            node_list = new_node_list;
            capacity = new_capacity;
        }
        node_list[node_list_length++] = delim; // append delimiter to node_list
        node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser, disable_cache_check);
        if (node == &ASTNode_fail) {
            parser->_class->seek(parser, -delim->ntokens, P_SEEK_CUR);
            break;
        }
        node_list[node_list_length++] = node;
    }
    size_t cur_token_key = parser->_class->tell(parser);
    Token * token_cur = NULL;
    err_type status = parser->_class->get(parser, token_key, &token_cur);
    if (status) {
        printf("unhandled error (%d) in ListRule_check_rule_ getting current token from parser\n", status);
    }
    Token * token_start = NULL;
    status = parser->_class->get(parser, token_key, &token_start);
    if (status) {
        printf("unhandled error (%d) in ListRule_check_rule_ getting starting token from parser\n", status);
    }
    //printf("list rule building new node\");
    ASTNode * res = ASTNode_class.new(list_rule, token_key, cur_token_key - token_key, token_cur->start - token_start->start, node_list_length, node_list);
    parser->_class->add_node(parser, res);
    return res;
}
void ListRule_clear_cache(Rule * list_rule, Token * tok) {
    //printf("clearing token from list rule cache with id: %d: ", list_rule->id);
    //Token_print(tok);
    //printf("\n");
    ListRule * self = DOWNCAST_P(DOWNCAST_P(list_rule, Rule, DerivedRule), DerivedRule, ListRule); // not used here
    self->delim->_class->clear_cache(self->delim, tok);
    DerivedRule_class.Rule_class.clear_cache(list_rule, tok); // WARNING: this is the one that might leak if it doesn't properly get routed to DerivedRule_clear_cache
}

/* RepeatRule implementations */

/* Type system metadata for RepeatRule */
#define RepeatRule_NAME "RepeatRule.DerivedRule.Rule"
//StructInfo RepeatRule_INFO = {0};
Type const RepeatRule_TYPE = {//._info = &RepeatRule_INFO,
                            ._class = &Type_class,
                            .type_name = RepeatRule_NAME
                            };
/* END Type system metadata for RepeatRule */

struct RepeatRuleType RepeatRule_class = RepeatRuleType_DEFAULT_INIT;

RepeatRule * RepeatRule_new(rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep) {
    RepeatRule * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (RepeatRule) RepeatRule_DEFAULT_INIT;
    if (RepeatRule_init(ret, id, rule, min_rep, max_rep)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}
err_type RepeatRule_init(RepeatRule * self, rule_id_type id, Rule * rule, size_t min_rep, size_t max_rep) {
    self->_class->DerivedRule_class.init(&(self->DerivedRule), id, rule);
    self->min_rep = min_rep;
    self->max_rep = max_rep;
    return PEGGY_SUCCESS;
}
void RepeatRule_dest(RepeatRule * self) {
    DerivedRule_dest(&(self->DerivedRule));
}
void RepeatRule_del(RepeatRule * self) {
    RepeatRule_dest(self);
    free(self);
}
void RepeatRule_as_DerivedRule_del(DerivedRule * repeat_rule) {
    RepeatRule_del(DOWNCAST_P(repeat_rule, DerivedRule, RepeatRule));
}
void RepeatRule_as_Rule_del(Rule * repeat_rule) {
    RepeatRule_del(DOWNCAST_P(DOWNCAST_P(repeat_rule, Rule, DerivedRule), DerivedRule, RepeatRule));
}

/*
err_type RepeatRule_build(Rule * repeat_rule, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    RepeatRule * self = DOWNCAST_P(DOWNCAST_P(repeat_rule, Rule, DerivedRule), DerivedRule, RepeatRule);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

ASTNode * RepeatRule_check_rule_(Rule * repeat_rule, Parser * parser, size_t token_key, bool disable_cache_check) {
    //printf("checking repeat rule. id: %d\n", repeat_rule->id);
    RepeatRule * self = DOWNCAST_P(DOWNCAST_P(repeat_rule, Rule, DerivedRule), DerivedRule, RepeatRule);
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser, disable_cache_check);
    // TODO: when stack implementation is available, replace
    size_t capacity = 0;
    ASTNode ** node_list = NULL;
    size_t node_list_length = 0;

    while (node != &ASTNode_fail) {
        //printf("did not find fail node: trying repeat again, %p, list size %zu\n", (void*)node, node_list_length);
        if (node_list_length == capacity) { // realloc more space
            size_t new_capacity = (2 * capacity + 3);
            ASTNode ** new_node_list = realloc(node_list, sizeof(*new_node_list) * new_capacity);
            if (!new_node_list) {
                free(node_list); // will work if node_list is NULL as well                
                return &ASTNode_fail;
            }
            node_list = new_node_list;
            capacity = new_capacity;
        }
        node_list[node_list_length++] = node; // append node
        node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser, disable_cache_check);        
    }
    if (node_list_length < self->min_rep || (self->max_rep && node_list_length > self->max_rep)) {
        free(node_list);
        return &ASTNode_fail;
    }

    size_t cur_token_key = parser->_class->tell(parser);
    Token * token_cur = NULL;
    err_type status = parser->_class->get(parser, token_key, &token_cur);
    if (status) {
        printf("unhandled error (%d) in RepeatRule_check_rule_ getting current token from parser\n", status);
    }
    Token * token_start = NULL;
    status = parser->_class->get(parser, token_key, &token_start);
    if (status) {
        printf("unhandled error (%d) in RepeatRule_check_rule_ getting starting token from parser\n", status);
    }
    ASTNode * res = ASTNode_class.new(repeat_rule, token_key, cur_token_key - token_key, token_cur->start - token_start->start, node_list_length, node_list);
    parser->_class->add_node(parser, res);
    return res;
}

/* OptionalRule implementations */

/* Type system metadata for OptionalRule */
#define OptionalRule_NAME "OptionalRule.DerivedRule.Rule"
//StructInfo OptionalRule_INFO = {0};
Type const OptionalRule_TYPE = {//._info = &OptionalRule_INFO,
                            ._class = &Type_class,
                            .type_name = OptionalRule_NAME
                            };
/* END Type system metadata for OptionalRule */

struct OptionalRuleType OptionalRule_class = OptionalRuleType_DEFAULT_INIT;

OptionalRule * OptionalRule_new(rule_id_type id, Rule * rule) {
    OptionalRule * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (OptionalRule) OptionalRule_DEFAULT_INIT;
    if (OptionalRule_init(ret, id, rule)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}
err_type OptionalRule_init(OptionalRule * self, rule_id_type id, Rule * rule) {
    self->_class->DerivedRule_class.init(&(self->DerivedRule), id, rule);
    return PEGGY_SUCCESS;
}
void OptionalRule_dest(OptionalRule * self) {
    DerivedRule_dest(&(self->DerivedRule));
}
void OptionalRule_del(OptionalRule * self) {
    OptionalRule_dest(self);
    free(self);
}
void OptionalRule_as_DerivedRule_del(DerivedRule * optional_rule) {
    OptionalRule_del(DOWNCAST_P(optional_rule, DerivedRule, OptionalRule));
}
void OptionalRule_as_Rule_del(Rule * optional_rule) {
    OptionalRule_del(DOWNCAST_P(DOWNCAST_P(optional_rule, Rule, DerivedRule), DerivedRule, OptionalRule));
}

/*
err_type OptionalRule_build(Rule * optional_rule, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    OptionalRule * self = DOWNCAST_P(DOWNCAST_P(optional_rule, Rule, DerivedRule), DerivedRule, OptionalRule);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

ASTNode * OptionalRule_check_rule_(Rule * optional_rule, Parser * parser, size_t token_key, bool disable_cache_check) {
    //printf("checking optional rule. id: %d\n", optional_rule->id);
    OptionalRule * self = DOWNCAST_P(DOWNCAST_P(optional_rule, Rule, DerivedRule), DerivedRule, OptionalRule);
    ASTNode ** node_list = NULL;
    size_t node_list_length = 0;
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser, disable_cache_check);
    if (node != &ASTNode_fail) {
        node_list = malloc(sizeof(*node_list));
        if (!node_list) {
            return &ASTNode_fail;
        }
        node_list[node_list_length++] = node;
    }
    ASTNode * res = ASTNode_class.new(optional_rule, token_key, node->ntokens, node->str_length, node_list_length, node_list);
    parser->_class->add_node(parser, res);
    return res;
}

/* NegativeLookahead implementations */

/* Type system metadata for NegativeLookahead */
#define NegativeLookahead_NAME "NegativeLookahead.DerivedRule.Rule"
//StructInfo NegativeLookahead_INFO = {0};
Type const NegativeLookahead_TYPE = {//._info = &NegativeLookahead_INFO,
                            ._class = &Type_class,
                            .type_name = NegativeLookahead_NAME
                            };
/* END Type system metadata for NegativeLookahead */

struct NegativeLookaheadType NegativeLookahead_class = NegativeLookaheadType_DEFAULT_INIT;

NegativeLookahead * NegativeLookahead_new(rule_id_type id, Rule * rule) {
    NegativeLookahead * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (NegativeLookahead) NegativeLookahead_DEFAULT_INIT;
    if (NegativeLookahead_init(ret, id, rule)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}
err_type NegativeLookahead_init(NegativeLookahead * self, rule_id_type id, Rule * rule) {
    self->_class->DerivedRule_class.init(&(self->DerivedRule), id, rule);
    return PEGGY_SUCCESS;
}
void NegativeLookahead_dest(NegativeLookahead * self) {
    DerivedRule_dest(&(self->DerivedRule));
}
void NegativeLookahead_del(NegativeLookahead * self) {
    NegativeLookahead_dest(self);
    free(self);
}
void NegativeLookahead_as_DerivedRule_del(DerivedRule * negative_lookahead) {
    NegativeLookahead_del(DOWNCAST_P(negative_lookahead, DerivedRule, NegativeLookahead));
}
void NegativeLookahead_as_Rule_del(Rule * negative_lookahead) {
    NegativeLookahead_del(DOWNCAST_P(DOWNCAST_P(negative_lookahead, Rule, DerivedRule), DerivedRule, NegativeLookahead));
}

/*
err_type NegativeLookahead_build(Rule * negative_lookahead, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    NegativeLookahead * self = DOWNCAST_P(DOWNCAST_P(negative_lookahead, Rule, DerivedRule), DerivedRule, NegativeLookahead);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

ASTNode * NegativeLookahead_check_rule_(Rule * negative_lookahead, Parser * parser, size_t token_key, bool disable_cache_check) {
    //printf("checking negative lookahead rule. id: %d\n", negative_lookahead->id);
    NegativeLookahead * self = DOWNCAST_P(DOWNCAST_P(negative_lookahead, Rule, DerivedRule), DerivedRule, NegativeLookahead);
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser, disable_cache_check);
    if (node == &ASTNode_fail) {
        return &ASTNode_lookahead;
    }
    parser->_class->seek(parser, token_key, P_SEEK_SET);
    return &ASTNode_fail;
}
/* PositiveLookahead implementations */

/* Type system metadata for PositiveLookahead */
#define PositiveLookahead_NAME "PositiveLookahead.DerivedRule.Rule"
//StructInfo PositiveLookahead_INFO = {0};
Type const PositiveLookahead_TYPE = {//._info = &PositiveLookahead_INFO,
                            ._class = &Type_class,
                            .type_name = PositiveLookahead_NAME
                            };
/* END Type system metadata for PositiveLookahead */

struct PositiveLookaheadType PositiveLookahead_class = PositiveLookaheadType_DEFAULT_INIT;

PositiveLookahead * PositiveLookahead_new(rule_id_type id, Rule * rule) {
    PositiveLookahead * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (PositiveLookahead) PositiveLookahead_DEFAULT_INIT;
    if (PositiveLookahead_init(ret, id, rule)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}
err_type PositiveLookahead_init(PositiveLookahead * self, rule_id_type id, Rule * rule) {
    self->_class->DerivedRule_class.init(&(self->DerivedRule), id, rule);
    return PEGGY_SUCCESS;
}
void PositiveLookahead_dest(PositiveLookahead * self) {
    DerivedRule_dest(&(self->DerivedRule));
}
void PositiveLookahead_del(PositiveLookahead * self) {
    PositiveLookahead_dest(self);
    free(self);
}
void PositiveLookahead_as_DerivedRule_del(DerivedRule * positive_lookahead) {
    PositiveLookahead_del(DOWNCAST_P(positive_lookahead, DerivedRule, PositiveLookahead));
}
void PositiveLookahead_as_Rule_del(Rule * positive_lookahead) {
    PositiveLookahead_del(DOWNCAST_P(DOWNCAST_P(positive_lookahead, Rule, DerivedRule), DerivedRule, PositiveLookahead));
}

/*
err_type PositiveLookahead_build(Rule * positive_lookahead, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    PositiveLookahead * self = DOWNCAST_P(DOWNCAST_P(positive_lookahead, Rule, DerivedRule), DerivedRule, PositiveLookahead);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

ASTNode * PositiveLookahead_check_rule_(Rule * positive_lookahead, Parser * parser, size_t token_key, bool disable_cache_check) {
    //printf("checking positive lookahead rule. id: %d\n", positive_lookahead->id);
    PositiveLookahead * self = DOWNCAST_P(DOWNCAST_P(positive_lookahead, Rule, DerivedRule), DerivedRule, PositiveLookahead);
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser, disable_cache_check);
    if (node != &ASTNode_fail) {
        parser->_class->seek(parser, token_key, P_SEEK_SET);
        return &ASTNode_lookahead;
    }
    return node;
}

/* AnonymousProduction implementations */

/* Type system metadata for AnonymousProduction */
#define AnonymousProduction_NAME "AnonymousProduction.DerivedRule.Rule"
//StructInfo AnonymousProduction_INFO = {0};
Type const AnonymousProduction_TYPE = {//._info = &AnonymousProduction_INFO,
                            ._class = &Type_class,
                            .type_name = AnonymousProduction_NAME
                            };
/* END Type system metadata for AnonymousProduction */

struct AnonymousProductionType AnonymousProduction_class = AnonymousProductionType_DEFAULT_INIT;

AnonymousProduction * AnonymousProduction_new(rule_id_type id, Rule * rule) {
    AnonymousProduction * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (AnonymousProduction) AnonymousProduction_DEFAULT_INIT;
    if (AnonymousProduction_init(ret, id, rule)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}

err_type AnonymousProduction_init(AnonymousProduction * self, rule_id_type id, Rule * rule) {
    self->_class->DerivedRule_class.init(&(self->DerivedRule), id, rule);
    return PEGGY_SUCCESS;
}
void AnonymousProduction_dest(AnonymousProduction * self) {
    DerivedRule_dest(&(self->DerivedRule));
}
void AnonymousProduction_del(AnonymousProduction * self) {
    AnonymousProduction_dest(self);
    free(self);
}
void AnonymousProduction_as_DerivedRule_del(DerivedRule * anonymous_production) {
    AnonymousProduction_del(DOWNCAST_P(anonymous_production, DerivedRule, AnonymousProduction));
}
void AnonymousProduction_as_Rule_del(Rule * anonymous_production) {
    AnonymousProduction_del(DOWNCAST_P(DOWNCAST_P(anonymous_production, Rule, DerivedRule), DerivedRule, AnonymousProduction));
}

/*
err_type AnonymousProduction_build(Rule * anonymous_production, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    AnonymousProduction * self = DOWNCAST_P(DOWNCAST_P(anonymous_production, Rule, DerivedRule), DerivedRule, AnonymousProduction);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

/* Production implementations */

ASTNode * build_action_default(Parser * parser, ASTNode * node) {
    return node;
}

/* Type system metadata for AnonymousProduction */
#define Production_NAME "Produciton.AnonymousProduction.DerivedRule.Rule"
//StructInfo AnonymousProduction_INFO = {0};
Type const Production_TYPE = {//._info = &AnonymousProduction_INFO,
                            ._class = &Type_class,
                            .type_name = Production_NAME
                            };
/* END Type system metadata for AnonymousProduction */

struct ProductionType Production_class = ProductionType_DEFAULT_INIT;

Production * Production_new(rule_id_type id, Rule * rule, build_action_ftype build_action) {
    Production * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (Production) Production_DEFAULT_INIT;
    if (Production_init(ret, id, rule, build_action)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}
err_type Production_init(Production * self, rule_id_type id, Rule * rule, build_action_ftype build_action) {
    self->_class->AnonymousProduction_class.init(&(self->AnonymousProduction), id, rule);
    self->build_action = build_action;
    return PEGGY_SUCCESS;
}
void Production_dest(Production * self) {
    AnonymousProduction_dest(&(self->AnonymousProduction));
}
void Production_del(Production * self) {
    Production_dest(self);
    free(self);
}
void Production_as_AnonymousProduction_del(AnonymousProduction * production) {
    Production_del(DOWNCAST_P(production, AnonymousProduction, Production));
}
void Production_as_DerivedRule_del(DerivedRule * production) {
    Production_del(DOWNCAST_P(DOWNCAST_P(production, DerivedRule, AnonymousProduction), AnonymousProduction, Production));
}
void Production_as_Rule_del(Rule * production) {
    Production_del(DOWNCAST_P(DOWNCAST_P(DOWNCAST_P(production, Rule, DerivedRule), DerivedRule, AnonymousProduction), AnonymousProduction, Production));
}

/*
err_type Production_build(Rule * production, ParserGenerator * pg, char * buffer, size_t buffer_size) {
    Production * self = DOWNCAST_P(DOWNCAST_P(DOWNCAST_P(production, Rule, DerivedRule), DerivedRule, AnonymousProduction), AnonymousProduction, Production);
    return PEGGY_NOT_IMPLEMENTED;
}
*/

Type const * PRODUCTION_UNESCAPED_RULES[2] = {
    &Production_TYPE,
    &LiteralRule_TYPE,
};
ASTNode * Production_check_rule_(Rule * production, Parser * parser, size_t token_key, bool disable_cache_check) {
    //printf("checking production rule. id: %d\n", production->id);
    Production * self = DOWNCAST_P(DOWNCAST_P(DOWNCAST_P(production, Rule, DerivedRule), DerivedRule, AnonymousProduction), AnonymousProduction, Production);
    
    ASTNode * node = self->AnonymousProduction.DerivedRule.rule->_class->check(self->AnonymousProduction.DerivedRule.rule, parser, disable_cache_check);
    if (node != &ASTNode_fail) {
        //printf("production rule sub rule passed; %p\n", (void *) node);
        if (!is_skip_node(node)) {
            //printf("is not skip node\n");
            if (!isinstance((Type *)node->rule->_class->_class, 2, PRODUCTION_UNESCAPED_RULES)) {
                //printf("is not unescaped instance\n");

                // pretty sure this is causing my leak
                node->_class->init(node, production, node->token_key, node->ntokens, node->str_length, node->nchildren, node->children);
                //ASTNode * old_node = node;
                //node = ASTNode_class.new(production, old_node->token_key, old_node->ntokens, old_node->str_length, old_node->nchildren, old_node->children);
                //old_node->nchildren = 0;
                //old_node->children = NULL; // necessary so that I don't accidentally free the children twice or access after free
            } else {
                //printf("is unescaped instance\n");
                ASTNode ** node_list = malloc(sizeof(*node_list));
                if (!node_list) {
                    return &ASTNode_fail;
                }
                node_list[0] = node;
                node = ASTNode_class.new(production, node->token_key, node->ntokens, node->str_length, 1, node_list);
                parser->_class->add_node(parser, node);
            }
        } else {
            //printf("is skip node\n");
        }
        return self->build_action(parser, node);
    }
    return node;
}
