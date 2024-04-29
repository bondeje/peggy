#define _GNU_SOURCE
/**
 * High level TODO: 
 *      in each case of adding a new node, there is a separate line of allocating a new node within a *_check_rule_ and then notifying the parser, which holes the AST. I think the *_check_rule_ should request a new ASTNode from the parser which internally makes and stores a node. The rules should not be responsible for making new ASTNodes if the Parser is the one to manage it. This is needed anyway when the arena allocation for ASTNodes is done anyway
*/


/* C STD LIB */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> // for unhandled error printfs
#include <assert.h>

/* POSIX */
#include <regex.h>
#if defined(MSYS)
#include <tre/tre.h>
#endif

#define DEFAULT_RE_SYNTAX (RE_SYNTAX_POSIX_EXTENDED | RE_CHAR_CLASSES)

#include <peggy/utils.h>
#include <peggy/type.h>
#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/token.h>
#include <peggy/hash_map.h>

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
    self->ncalls = 0;
    self->nevals = 0;
    return PEGGY_SUCCESS;
}

void Rule_dest(Rule * self) {
    //printf("destroying rule id %d\n", self->id);
}

void Rule_del(Rule * self) {
    /* TODO: all children need to call a finalizer of some kind */
    Rule_dest(self);
    free(self);
}

/* TODO: probably should change this so that Rule_check_rule_ output node is in arguments and ASTNode_fail becomes an error code */
ASTNode * Rule_check_rule_(Rule * self, Parser * parser, size_t token_keyk) {
    /* Not to be implemented */
    return NULL;
}

ASTNode * Rule_check(Rule * self, Parser * parser) {
    //printf("checking Rule id: %d (type: %s) @%zu\n", self->id, self->_class->_class->type_name, parser->loc_);
    self->ncalls++;
    size_t token_key = parser->_class->tell(parser);
    ASTNode * res = parser->_class->check_cache(parser, self->id, token_key);
    if (res) {
        //printf("rule %d retrieved from cache!\n", self->id);
        parser->_class->seek(parser, res->ntokens, P_SEEK_CUR);
        return res;
    }
    //printf("check cache failed\n");
    // check the rule and generate a node
    self->nevals++;
    res = self->_class->check_rule_(self, parser, token_key);
    // cache the result
    if (!res) {
        printf("storing a null from rule id %d!!!\n", self->id);
    }
    parser->_class->cache_check(parser, self->id, token_key, res);

    /* TODO: log the result */

    if (res == &ASTNode_fail) {
        //printf("rule %d failed!\n", self->id);
        // reset the parser seek because the rule check failed
        parser->_class->seek(parser, token_key, P_SEEK_SET);
    }
    //printf("rule %d succeeded! Now at (%zu)\n", self->id, parser->loc_);
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

ASTNode * SequenceRule_check_rule_(Rule * sequence_rule, Parser * parser, size_t token_key) {
    //printf("checking sequence rule. id: %d\n", sequence_rule->id);
    SequenceRule * self = DOWNCAST_P(DOWNCAST_P(sequence_rule, Rule, ChainRule), ChainRule, SequenceRule);
    ASTNode ** children = malloc(sizeof(ASTNode *) * self->ChainRule.deps_size);
    if (!children) {
        return &ASTNode_fail;
    }
    size_t nchildren = 0;
    for (size_t i = 0; i < self->ChainRule.deps_size; i++) {
        ASTNode * child_res = self->ChainRule.deps[i]->_class->check(self->ChainRule.deps[i], parser);
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
    return parser->_class->add_node(parser, sequence_rule, token_key, token_cur - token_key, cur->start - start->start, nchildren, children);
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

ASTNode * ChoiceRule_check_rule_(Rule * choice_rule, Parser * parser, size_t token_key) {
    //printf("checking choice rule. id: %d\n", choice_rule->id);
    ChoiceRule * self = DOWNCAST_P(DOWNCAST_P(choice_rule, Rule, ChainRule), ChainRule, ChoiceRule);
    for (size_t i = 0; i < self->ChainRule.deps_size; i++) {
        ASTNode * child_res = self->ChainRule.deps[i]->_class->check(self->ChainRule.deps[i], parser);
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
#if defined(MSYS)
    // use the shitty POSIX API but with internal fixes
    if (regcomp(&(self->regex), self->regex_s, REG_EXTENDED)) {
        return PEGGY_REGEX_FAILURE;
    }
#else
    re_set_syntax(DEFAULT_RE_SYNTAX);
    char const * err_message = re_compile_pattern(self->regex_s, strlen(self->regex_s), 
                    &self->regex);
    if (err_message) {
        printf("regex compile error message for patther %s: %s\n", self->regex_s, err_message);
        return PEGGY_REGEX_FAILURE;
    }
#endif    
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

ASTNode * LiteralRule_check_rule_(Rule * literal_rule, Parser * parser, size_t token_key) {
    LiteralRule * self = DOWNCAST_P(literal_rule, Rule, LiteralRule);
    //printf("checking literal rule. id: %d, %s\n", literal_rule->id, self->regex_s);
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
        printf("unhandled error (%d) in LiteralRule_check_rule_ (id = %d) getting current token %zu from parser\n", status, literal_rule->id, token_key);
    }
    // since the parser->_class->get might have triggered
    //if (!parser->disable_cache_check) { // 
    //    ASTNode * res = literal_rule->_class->check_cache_(literal_rule, parser, token_key);
    //}
    if (tok->_class->len(tok) == 0) { // token retrieved is empty. This shouldn't really happen
        return &ASTNode_fail;
    }
#if defined(MSYS)
    // garbage POSIX interface, fixed up by tre
    if (!tre_regnexec(&(self->regex), tok->string + tok->start, tok->end - tok->start, LITERAL_N_MATCHES, self->match, 0)) {
        // the regex succeed to match
        parser->_class->seek(parser, 1, P_SEEK_CUR);
        size_t length = self->match[0].rm_eo - self->match[0].rm_so;
        //printf("LiteralRule (id %d) regex matched with length %zu!\n", literal_rule->id, length);
        return parser->_class->add_node(parser, literal_rule, token_key, length ? 1 : 0, length, 0, NULL);
    }
#else
    int length = re_match(&self->regex,
          tok->string + tok->start, tok->end - tok->start, 
          0, NULL);
    if (length >= 0) {
        parser->_class->seek(parser, 1, P_SEEK_CUR);
        //printf("LiteralRule (id %d) regex %s matched with length %d!\n", literal_rule->id, self->regex_s, length);
        return parser->_class->add_node(parser, literal_rule, token_key, length ? 1 : 0, length, 0, NULL);
    }
#endif
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

ASTNode * ListRule_check_rule_(Rule * list_rule, Parser * parser, size_t token_key) {
    //printf("checking list rule. id: %d\n", list_rule->id);
    ListRule * self = DOWNCAST_P(DOWNCAST_P(list_rule, Rule, DerivedRule), DerivedRule, ListRule);

    // call alternating checks for the delimiter and base rule to count the number of children
    size_t nchildren = 0;
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
    //ASTNode * check_node = NULL;
    ASTNode * delim = &ASTNode_fail;
    while (node != &ASTNode_fail) {
        //check_node = parser->_class->check_cache(parser, node->rule->id, node->token_key);
        //DEBUG_ASSERT(node == check_node, "failed to retrieve delimiter node from cache\n");
        //printf("verified check cache for rule id %d at %zu. %p == %p\n", node->rule->id, node->token_key, (void *)node, (void *)check_node);
        nchildren++;
        delim = self->delim->_class->check(self->delim, parser);
        if (delim == &ASTNode_fail) {
            node = &ASTNode_fail;
        } else {
            //check_node = parser->_class->check_cache(parser, delim->rule->id, delim->token_key);
            //DEBUG_ASSERT(delim == check_node, "failed to retrieve delimiter node from cache\n");
            //printf("verified check cache for rule id %d at %zu. %p == %p\n", delim->rule->id, delim->token_key, (void *)node, (void *)check_node);
            nchildren++;
            node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
        }
    }
    if (!nchildren) {
        return &ASTNode_fail;
    }
    if (delim != &ASTNode_fail) {
        parser->_class->seek(parser, -delim->ntokens, P_SEEK_CUR);
    }
    
    // success, so allocate memory for the children
    ASTNode ** children = malloc(sizeof(*children) * nchildren);
    if (!children) {
        printf("failed to alloc enough memory for children in List Rule\n");
        return &ASTNode_fail; // failure but for wrong reasons
    }

    // populate children
    size_t token_index = token_key;
    for (size_t i = 0; i < nchildren; i++) {
        children[i] = parser->_class->check_cache(parser, i & 1 ? self->delim->id : self->DerivedRule.rule->id, token_index); // this should never not be null
        //if (!children[i]) {
        //    printf("cache retrieval returned null pointer (%p) for child rule %d in list rule with id %d at location %zu. disable_check_cache = %d!!!!!!!\n", (void*)(children[i]), i & 1 ? self->delim->id : self->DerivedRule.rule->id, list_rule->id, token_index, parser->disable_cache_check);
        //}
        token_index += children[i]->ntokens;
    }


    /* This is part of the algorithm that causes O(n^2), I believe because of the repeated calls to realloc
    // TODO: when stack is available, replace with stack API
    size_t capacity = 3; // MAGIC_NUMBER. Frequently only one will ever be found but 2x will be common. Avoid realloc
    ASTNode ** children = malloc(sizeof(*children) * capacity);
    if (!children) {
        return &ASTNode_fail;
    }
    size_t nchildren = 0;
    children[nchildren++] = node;
    while (true) { 
        ASTNode * delim = self->delim->_class->check(self->delim, parser);
        if (delim == &ASTNode_fail) {
            //printf("delimiter failed with list length %zu\n", nchildren);
            break;
        }
        
        if (nchildren >= capacity - 1) { // realloc more space. Always make sure it can handle a delim and node in one pass
            size_t new_capacity = 4 * capacity + 1; // because of above comment, it should always have odd capacity
            //printf("realloc in ListRule_check_rule to new size %zu\n", new_capacity);
            ASTNode ** new_children = realloc(children, sizeof(*children) * new_capacity);
            if (!new_children) {
                free(children);
                return &ASTNode_fail;
            }
            children = new_children;
            capacity = new_capacity;
        }
        children[nchildren++] = delim; // append delimiter to children
        
        node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
        if (node == &ASTNode_fail) {
            parser->_class->seek(parser, -delim->ntokens, P_SEEK_CUR);
            break;
        }
        children[nchildren++] = node;
    }
    size_t token_index = parser->_class->tell(parser);
    */
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
    return parser->_class->add_node(parser, list_rule, token_key, token_index - token_key, token_cur->start - token_start->start, nchildren, children);
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

ASTNode * RepeatRule_check_rule_(Rule * repeat_rule, Parser * parser, size_t token_key) {
    //printf("checking repeat rule. id: %d\n", repeat_rule->id);
    RepeatRule * self = DOWNCAST_P(DOWNCAST_P(repeat_rule, Rule, DerivedRule), DerivedRule, RepeatRule);
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
    // TODO: when stack implementation is available, replace
    size_t capacity = 0;
    ASTNode ** node_list = NULL;
    size_t node_list_length = 0;

    while (node != &ASTNode_fail) {
        //printf("did not find fail node: trying repeat again, %p, list size %zu\n", (void*)node, node_list_length);
        
        if (node_list_length == capacity) { // realloc more space
            size_t new_capacity = (2 * capacity + 3);
            //printf("RepeatRule_check_rule_ realloc to %zu\n", new_capacity);
            ASTNode ** new_node_list = realloc(node_list, sizeof(*new_node_list) * new_capacity);
            if (!new_node_list) {
                free(node_list); // will work if node_list is NULL as well                
                return &ASTNode_fail;
            }
            node_list = new_node_list;
            capacity = new_capacity;
        }
        node_list[node_list_length++] = node; // append node
        node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);        
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
    return parser->_class->add_node(parser, repeat_rule, token_key, cur_token_key - token_key, token_cur->start - token_start->start, node_list_length, node_list);
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

ASTNode * OptionalRule_check_rule_(Rule * optional_rule, Parser * parser, size_t token_key) {
    //printf("checking optional rule. id: %d\n", optional_rule->id);
    OptionalRule * self = DOWNCAST_P(DOWNCAST_P(optional_rule, Rule, DerivedRule), DerivedRule, OptionalRule);
    ASTNode ** node_list = NULL;
    size_t node_list_length = 0;
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
    if (node != &ASTNode_fail) {
        node_list = malloc(sizeof(*node_list));
        if (!node_list) {
            return &ASTNode_fail;
        }
        node_list[node_list_length++] = node;
    }
    return parser->_class->add_node(parser, optional_rule, token_key, node->ntokens, node->str_length, node_list_length, node_list);
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

ASTNode * NegativeLookahead_check_rule_(Rule * negative_lookahead, Parser * parser, size_t token_key) {
    //printf("checking negative lookahead rule. id: %d\n", negative_lookahead->id);
    NegativeLookahead * self = DOWNCAST_P(DOWNCAST_P(negative_lookahead, Rule, DerivedRule), DerivedRule, NegativeLookahead);
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
    if (node == &ASTNode_fail) {
        return &ASTNode_lookahead;
    }
    parser->_class->seek(parser, token_key, P_SEEK_SET); // pretty sure this isn't necessary
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

ASTNode * PositiveLookahead_check_rule_(Rule * positive_lookahead, Parser * parser, size_t token_key) {
    //printf("checking positive lookahead rule. id: %d\n", positive_lookahead->id);
    PositiveLookahead * self = DOWNCAST_P(DOWNCAST_P(positive_lookahead, Rule, DerivedRule), DerivedRule, PositiveLookahead);
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
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
#define Production_NAME "Production.AnonymousProduction.DerivedRule.Rule"
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
ASTNode * Production_check_rule_(Rule * production, Parser * parser, size_t token_key) {
    //printf("checking production rule. id: %d\n", production->id);
    Production * self = DOWNCAST_P(DOWNCAST_P(DOWNCAST_P(production, Rule, DerivedRule), DerivedRule, AnonymousProduction), AnonymousProduction, Production);
    
    ASTNode * node = self->AnonymousProduction.DerivedRule.rule->_class->check(self->AnonymousProduction.DerivedRule.rule, parser);
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
                node = parser->_class->add_node(parser, production, node->token_key, node->ntokens, node->str_length, 1, node_list);
            }
        } else {
            //printf("is skip node\n");
        }
        return self->build_action(parser, node);
    }// else {
    //    printf("production rule sub rule failed; %d\n", production->id);
    //}
    return node;
}
