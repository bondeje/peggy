// this is really only available on Linux
#ifdef Linux
#define _GNU_SOURCE
#endif

/* C standard library includes  */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> // for unhandled error printfs
#include <assert.h>

/* POSIX includes */
#ifdef Linux
#include <regex.h>
#endif

/* lib includes */
#include <logger.h>
#ifndef Linux
// basically anywhere that does not have _GNU_SOURCE, which is apparently everywhere other than non-Android Linux
// regex alternative
#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif
#include <pcre2.h>
#endif

/* peggy includes */
#include <peggy/utils.h>
#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/token.h>
#include <peggy/hash_map.h>

#define DEFAULT_RE_SYNTAX RE_SYNTAX_POSIX_EXTENDED | RE_BACKSLASH_ESCAPE_IN_LISTS | RE_DOT_NEWLINE

/* Rule implementations */

struct RuleType Rule_class = {
    .new = &Rule_new,
    .init = &Rule_init,
    .dest = &Rule_dest, 
    .del = &Rule_del,
    .check_rule_ = &Rule_check_rule_,
    .check = &Rule_check
};

/* id = -1 means none provided */
/* strictly speaking, unneeded */
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
/* likely unneeded */
err_type Rule_init(Rule * self, int id) {
    self->id = id;
#ifndef NDEBUG
    self->ncalls = 0;
    self->nevals = 0;
#endif
    return PEGGY_SUCCESS;
}

void Rule_dest(Rule * self) {
    /* no-op */
}

void Rule_del(Rule * self) {
    Rule_dest(self);
    free(self);
}

/* TODO: probably should change this so that Rule_check_rule_ output node is in arguments and ASTNode_fail becomes an error code */
ASTNode * Rule_check_rule_(Rule * self, Parser * parser, size_t token_keyk) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - abstract method called.\n", __func__);
    return NULL;
}

ASTNode * Rule_check(Rule * self, Parser * parser) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - rule id %d (type: %s) @%zu\n", __func__, self->id, self->_class->type_name, parser->loc_);
#ifndef NDEBUG
    self->ncalls++;
#endif
    size_t token_key = parser->_class->tell(parser);
    ASTNode * res = parser->_class->check_cache(parser, self->id, token_key);
    if (res) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - rule id %d retrieved from cache!\n", __func__, self->id);
        parser->_class->seek(parser, res->ntokens, P_SEEK_CUR);
        return res;
    }

    // check the rule and generate a node
#ifndef NDEBUG
    self->nevals++;
#endif
    res = self->_class->check_rule_(self, parser, token_key);
    // cache the result
    if (!res) {
        printf("storing a null from rule id %d!!!\n", self->id);
    }

    if (res == &ASTNode_fail) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - rule id %d failed!\n", __func__, self->id);
        // reset the parser seek because the rule check failed
        parser->_class->seek(parser, token_key, P_SEEK_SET);
    } else {
        LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - rule id %d succeeded! Now at (%zu)\n", __func__, self->id, parser->loc_);
    }

    parser->_class->cache_check(parser, self->id, token_key, res);

    return res;
}

/* ChainRule implementations */

#define ChainRule_NAME "ChainRule.Rule"

struct ChainRuleType ChainRule_class = {
    .Rule_class = {
        .type_name = ChainRule_NAME,
        .new = &Rule_new,
        .init = &Rule_init,
        .dest = &Rule_dest,
        .del = &ChainRule_as_Rule_del,
        .check_rule_ = &Rule_check_rule_,
        .check = &Rule_check
    },
    .new = &ChainRule_new,
    .init = &ChainRule_init,
    .dest = &ChainRule_dest,
    .del = &ChainRule_del
};

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
    Rule * rule = (Rule *)self;
    rule->_class->init(rule, id);
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
    ChainRule_del((ChainRule *)chain_rule);
}

/* SequenceRule implementations */
/* NOTE: was "And" in the python version */

#define SequenceRule_NAME "SequenceRule.ChainRule.Rule"

struct SequenceRuleType SequenceRule_class = {
    .ChainRule_class = {
        .Rule_class = {
            .type_name = SequenceRule_NAME,
            .new = &Rule_new,
            .init = &Rule_init,
            .dest = &Rule_dest,
            .del = &SequenceRule_as_Rule_del,
            .check_rule_ = &SequenceRule_check_rule_,
            .check = &Rule_check
        },
        .new = &ChainRule_new,
        .init = &ChainRule_init,
        .dest = &ChainRule_dest,
        .del = &SequenceRule_as_ChainRule_del},
    .new = &SequenceRule_new,
    .init = &SequenceRule_init,
    .dest = &SequenceRule_dest,
    .del = &SequenceRule_del
};

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
    ChainRule * chain_rule = (ChainRule *)self;
    self->_class->init(self, id, deps_size, deps);
    return PEGGY_SUCCESS;
}

void SequenceRule_dest(SequenceRule * self) {
    ChainRule_dest(&(self->ChainRule));
}

void SequenceRule_del(SequenceRule * self) {
    SequenceRule_dest(self);
    free(self);
}

void SequenceRule_as_ChainRule_del(ChainRule * sequence_rule) {
    SequenceRule_del((SequenceRule *)sequence_rule);
}

void SequenceRule_as_Rule_del(Rule * sequence_rule) {
    SequenceRule_del((SequenceRule *)sequence_rule);
}

ASTNode * SequenceRule_check_rule_(Rule * sequence_rule, Parser * parser, size_t token_key) {
    //printf("checking sequence rule. id: %d\n", sequence_rule->id);
    SequenceRule * self = (SequenceRule *)sequence_rule;
    ASTNode child = {0};
    ASTNode * tail = &child;
    size_t nchildren = 0;
    for (size_t i = 0; i < self->ChainRule.deps_size; i++) {
        ASTNode * child_res = self->ChainRule.deps[i]->_class->check(self->ChainRule.deps[i], parser);
        if (child_res == &ASTNode_fail) {
            return child_res;
        }
        if (!is_skip_node(child_res)) { // TODO: is this really necessary?
            nchildren++;
            child_res->prev = tail;
            tail->next = child_res;
            tail = child_res;
        }
    }
    size_t token_cur = parser->_class->tell(parser);
    Token cur;
    err_type status = parser->_class->get(parser, token_cur, &cur);
    if (status) {
        printf("unhandled error (%d) in SequenceRule_check_rule_ getting current token from parser\n", status);
    }
    Token start;
    status = parser->_class->get(parser, token_key, &start);
    if (status) {
        printf("unhandled error (%d) in SequenceRule_check_rule_ getting starting token from parser\n", status);
    }
    return parser->_class->add_node(parser, sequence_rule, token_key, token_cur - token_key, (size_t)(cur.string - start.string), nchildren, child.next, 0);
}

/* ChoiceRule implementations */

#define ChoiceRule_NAME "ChoiceRule.ChainRule.Rule"

struct ChoiceRuleType ChoiceRule_class = {
    .ChainRule_class = {
        .Rule_class = {
            .type_name = ChoiceRule_NAME,
            .new = &Rule_new,
            .init = &Rule_init,
            .dest = &Rule_dest,
            .del = &ChoiceRule_as_Rule_del,
            .check_rule_ = &ChoiceRule_check_rule_,
            .check = &Rule_check
        },
        .new = &ChainRule_new,
        .init = &ChainRule_init,
        .dest = &ChainRule_dest,
        .del = &ChoiceRule_as_ChainRule_del
    },
    .new = &ChoiceRule_new,
    .init = &ChoiceRule_init,
    .dest = &ChoiceRule_dest,
    .del = &ChoiceRule_del
};

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
    ChainRule * chain_rule = (ChainRule *)self;
    chain_rule->_class->init(chain_rule, id, deps_size, deps);
    return PEGGY_SUCCESS;
}

void ChoiceRule_dest(ChoiceRule * self) {
    ChainRule_dest(&(self->ChainRule));
}

void ChoiceRule_del(ChoiceRule * self) {
    ChoiceRule_dest(self);
    free(self);
}

void ChoiceRule_as_ChainRule_del(ChainRule * choice_rule) {
    ChoiceRule_del((ChoiceRule *)choice_rule);
}

void ChoiceRule_as_Rule_del(Rule * choice_rule) {
    ChoiceRule_del((ChoiceRule *)choice_rule);
}

ASTNode * ChoiceRule_check_rule_(Rule * choice_rule, Parser * parser, size_t token_key) {
    //printf("checking choice rule. id: %d\n", choice_rule->id);
    ChoiceRule * self = (ChoiceRule *)choice_rule;
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

#define LiteralRule_NAME "LiteralRule.Rule"

struct LiteralRuleType LiteralRule_class = {
    .Rule_class = {
        .type_name = LiteralRule_NAME,
        .new = &Rule_new,
        .init = &Rule_init,
        .dest = &LiteralRule_as_Rule_dest,
        .del = &LiteralRule_as_Rule_del,
        .check_rule_ = &LiteralRule_check_rule_,
        .check = &Rule_check
    },
    .new = &LiteralRule_new,
    .init = &LiteralRule_init,
    .dest = &LiteralRule_dest,
    .del = &LiteralRule_del,
    .compile = &LiteralRule_compile_regex
};

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

#ifndef Linux
PCRE2_SIZE pcre2_err_offset;
int pcre2_err_code;
#endif

err_type LiteralRule_compile_regex(LiteralRule * self) {
    LOG_EVENT(NULL, LOG_LEVEL_DEBUG, "INFO: %s - compiling regex %s\n", __func__, self->regex_s);
    //printf("
#ifdef Linux
    re_set_syntax(DEFAULT_RE_SYNTAX);
    char const * err_message = re_compile_pattern(self->regex_s, strlen(self->regex_s), 
                    &self->regex);
    if (err_message) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - regex compile error message for pattern %s: %s\n", __func__, self->regex_s, err_message);
        return PEGGY_REGEX_FAILURE;
    }
#else 
    self->regex = pcre2_compile(self->regex_s, 
        PCRE2_ZERO_TERMINATED,
        PCRE2_ANCHORED /* must search only at start of string */ | PCRE2_DOTALL /* .* matches newlines */,
        &pcre2_err_code,    /* for error number */
        &pcre2_err_offset,  /* for error offset */
        NULL);              /* use default compile context */
    if (!self->regex) {
        LOG_EVENT(NULL, LOG_LEVEL_ERROR, "ERROR: %s - pcre2 regex compile error code for pattern %s: %d\n", __func__, (char *)self->regex_s, pcre2_err_code);
        return PEGGY_REGEX_FAILURE;
    }
    self->match = pcre2_match_data_create(1, NULL); // only include the first match
#endif    
    self->compiled = true;
    return PEGGY_SUCCESS;
}

err_type LiteralRule_init(LiteralRule * self, rule_id_type id, char const * regex_s) {
    Rule * rule = (Rule *)self;
    rule->_class->init(rule, id);
#ifndef Linux
    self->regex_s = (PCRE2_SPTR)regex_s;
    /* TODO: add failure check */
#else
    self->regex_s = regex_s;
#endif
    self->compiled = false;
    if (regex_s) {
        LiteralRule_compile_regex(self);
    }
    return PEGGY_SUCCESS;
}

void LiteralRule_dest(LiteralRule * self) {
    LOG_EVENT(NULL, LOG_LEVEL_DEBUG, "INFO: %s - freeing regex data for rule id %d with regex %s\n", __func__, self->Rule.id, self->regex_s);
#ifdef Linux
    regfree(&(self->regex));
#else
    pcre2_code_free(self->regex);
    pcre2_match_data_free(self->match);
#endif
    self->compiled = false;
    Rule_dest(&(self->Rule));
}

void LiteralRule_del(LiteralRule * self) {
    LiteralRule_dest(self);
    free(self);
}

void LiteralRule_as_Rule_del(Rule * literal_rule) {
    LiteralRule_del((LiteralRule *)literal_rule);
}

void LiteralRule_as_Rule_dest(Rule * literal_rule) {
    LiteralRule_dest((LiteralRule *)literal_rule);
}

ASTNode * LiteralRule_check_rule_(Rule * literal_rule, Parser * parser, size_t token_key) {
    LiteralRule * self = (LiteralRule *)literal_rule;
    LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - checking literal rule. id: %d, %s\n", __func__, literal_rule->id, self->regex_s);
    err_type status = PEGGY_SUCCESS;
    if (!self->compiled) {
        if ((status = LiteralRule_compile_regex(self))) {
            return &ASTNode_fail; // failed to compile the regular expression
        }
    }
    
    Token tok;
    status = parser->_class->get(parser, token_key, &tok);
    if (status) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - failed to get token_key %zu from parser tokens list of length %zu\n", __func__, token_key, parser->tokens.fill);
        return NULL;
    }
    if (tok._class->len(&tok) == 0) { // token retrieved is empty
        return &ASTNode_fail;
    }
#ifdef Linux
    int length = re_match(&self->regex,
          tok.string, tok.length, 
          0, NULL);
    if (length >= 0) {
        parser->_class->seek(parser, 1, P_SEEK_CUR);
        LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - literal rule (id %d) regex %s matched with length %d!\n", __func__, literal_rule->id, self->regex_s, length);
        return parser->_class->add_node(parser, literal_rule, token_key, length ? 1 : 0, length, 0, NULL, 0);
    }
#else 
    int rc = pcre2_match(
        self->regex,                   /* the compiled pattern */
        (PCRE2_SPTR)tok.string,              /* the subject string */
        (PCRE2_SIZE)tok.length,       /* the length of the subject */
        0,                    /* start at offset 0 in the subject */
        0,                    /* default options. Don't need to enforce PCRE2_ANCHORED when pattern was compiled as such */
        self->match,           /* block for storing the result */
        NULL                  /* use default match context */
        );
    if (rc >= 0) { // I think this means successful return. The official documentation does not say anything along the lines of "returns X on success, Y on failure", but pcre2demo handles errors when rc < 0
        PCRE2_SIZE * result = pcre2_get_ovector_pointer(self->match);
        int length = (int)(result[1] - result[0]);
        parser->_class->seek(parser, 1, P_SEEK_CUR);
        LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - literal rule (id %d) regex %s matched with length %d!\n", __func__, literal_rule->id, (char *)self->regex_s, length);
        return parser->_class->add_node(parser, literal_rule, token_key, length ? 1 : 0, length, 0, NULL, 0);
    }
#endif
    return &ASTNode_fail;
}

/* DerivedRule implementations */

#define DerivedRule_NAME "Rule.DerivedRule"

struct DerivedRuleType DerivedRule_class = {  
    .Rule_class = {
        .type_name = DerivedRule_NAME,
        .new = &Rule_new,
        .init = &Rule_init,
        .dest = &Rule_dest,
        .del = &DerivedRule_as_Rule_del,
        .check_rule_ = &Rule_check_rule_,
        .check = &Rule_check
    },
    .new = &DerivedRule_new,
    .init = &DerivedRule_init,
    .dest = &DerivedRule_dest,
    .del = &DerivedRule_del
};

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
    Rule * rule_ = (Rule *)self;
    rule_->_class->init(rule_, id);
    self->rule = rule;
    return PEGGY_SUCCESS;
}

void DerivedRule_dest(DerivedRule * self) {
    Rule_dest((Rule *)self);
}

void DerivedRule_del(DerivedRule * self) {
    DerivedRule_dest(self);
    free(self);
}

void DerivedRule_as_Rule_del(Rule * derived_rule) {
    DerivedRule_del((DerivedRule *)derived_rule);
}

/* ListRule implementations */

#define ListRule_NAME "ListRule.DerivedRule.Rule"

struct ListRuleType ListRule_class = { 
    .DerivedRule_class = {
        .Rule_class = {
            .type_name = ListRule_NAME,
            .new = &Rule_new,
            .init = &Rule_init,
            .dest = &Rule_dest,
            .del = &ListRule_as_Rule_del,
            .check_rule_ = &ListRule_check_rule_,
            .check = &Rule_check
            },
        .new = &DerivedRule_new,
        .init = &DerivedRule_init,
        .dest = &DerivedRule_dest,
        .del = &ListRule_as_DerivedRule_del
    },
    .new = &ListRule_new,
    .init = &ListRule_init,
    .dest = &ListRule_dest,
    .del = &ListRule_del
};

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
    DerivedRule * derived = (DerivedRule *)self;
    derived->_class->init(derived, id, rule);
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
    ListRule_del((ListRule *)list_rule);
}
void ListRule_as_Rule_del(Rule * list_rule) {
    ListRule_del((ListRule *)list_rule);
}

ASTNode * ListRule_check_rule_(Rule * list_rule, Parser * parser, size_t token_key) {
    //printf("checking list rule. id: %d\n", list_rule->id);
    ListRule * self = (ListRule *) list_rule;

    // call alternating checks for the delimiter and base rule to count the number of children
    ASTNode child = {0};
    ASTNode * tail = &child;
    size_t nchildren = 0;
    Rule * derived_rule = ((DerivedRule *)self)->rule;
    ASTNode * (*derived_rule_check)(Rule *, Parser *) = derived_rule->_class->check;
    Rule * delim_rule = self->delim;
    ASTNode * (*delim_rule_check)(Rule *, Parser *) = delim_rule->_class->check;
    ASTNode * node = derived_rule_check(derived_rule, parser);
    //ASTNode * check_node = NULL;
    ASTNode * delim = &ASTNode_fail;
    size_t token_index = token_key;
    while (node != &ASTNode_fail) {
        //check_node = parser->_class->check_cache(parser, node->rule->id, node->token_key);
        //DEBUG_ASSERT(node == check_node, "failed to retrieve delimiter node from cache\n");
        //printf("verified check cache for rule id %d at %zu. %p == %p\n", node->rule->id, node->token_key, (void *)node, (void *)check_node);
        node->prev = tail;
        tail->next = node;
        tail = node;
        token_index += tail->ntokens;
        nchildren++;
        delim = delim_rule_check(delim_rule, parser);
        if (delim == &ASTNode_fail) {
            node = &ASTNode_fail;
        } else {
            //check_node = parser->_class->check_cache(parser, delim->rule->id, delim->token_key);
            //DEBUG_ASSERT(delim == check_node, "failed to retrieve delimiter node from cache\n");
            //printf("verified check cache for rule id %d at %zu. %p == %p\n", delim->rule->id, delim->token_key, (void *)node, (void *)check_node);
            delim->prev = tail;
            tail->next = delim;
            tail = delim;
            token_index += tail->ntokens;
            nchildren++;
            node = derived_rule_check(derived_rule, parser);
        }
    }
    if (!nchildren) {
        return &ASTNode_fail;
    }
    if (delim != &ASTNode_fail) {
        parser->_class->seek(parser, -delim->ntokens, P_SEEK_CUR);
        token_index -= tail->ntokens;
        tail = tail->prev;
        tail->next->prev = NULL;
        tail->next = NULL;
        nchildren--; // don't include the successful delim the preceeded the failed node
    }

    Token token_cur;
    err_type status = parser->_class->get(parser, token_key, &token_cur);
    if (status) {
        printf("unhandled error (%d) in ListRule_check_rule_ getting current token from parser\n", status);
    }
    Token token_start;
    status = parser->_class->get(parser, token_key, &token_start);
    if (status) {
        printf("unhandled error (%d) in ListRule_check_rule_ getting starting token from parser\n", status);
    }
    //printf("list rule building new node\n");
    return parser->_class->add_node(parser, list_rule, token_key, token_index - token_key, (size_t)(token_cur.string - token_start.string), nchildren, child.next, 0);
}

/* RepeatRule implementations */

#define RepeatRule_NAME "RepeatRule.DerivedRule.Rule"

struct RepeatRuleType RepeatRule_class = {
    .DerivedRule_class = {
        .Rule_class = {
            .type_name = RepeatRule_NAME,
            .new = &Rule_new,
            .init = &Rule_init,
            .dest = &Rule_dest,
            .del = &RepeatRule_as_Rule_del,
            .check_rule_ = &RepeatRule_check_rule_,
            .check = &Rule_check
        },
        .new = &DerivedRule_new,
        .init = &DerivedRule_init,
        .dest = &DerivedRule_dest,
        .del = &RepeatRule_as_DerivedRule_del
    },
    .new = &RepeatRule_new,
    .init = &RepeatRule_init,
    .dest = &RepeatRule_dest,
    .del = &RepeatRule_del
};

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
    DerivedRule * derived = (DerivedRule *)self;
    derived->_class->init(derived, id, rule);
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
    RepeatRule_del((RepeatRule *)repeat_rule);
}
void RepeatRule_as_Rule_del(Rule * repeat_rule) {
    RepeatRule_del((RepeatRule *)repeat_rule);
}

// MAJOR TODO: get rid of realloc. Follow ListRule_check_rule_ implementation
ASTNode * RepeatRule_check_rule_(Rule * repeat_rule, Parser * parser, size_t token_key) {
    //printf("checking repeat rule. id: %d\n", repeat_rule->id);
    RepeatRule * self = (RepeatRule *)repeat_rule;
    size_t nchildren = 0;
    Rule * derived_rule = ((DerivedRule *)self)->rule;
    ASTNode * (*derived_rule_check)(Rule *, Parser *) = derived_rule->_class->check;

    ASTNode child = {0};
    ASTNode * tail = &child;
    size_t token_index = token_key;

    ASTNode * node = derived_rule_check(derived_rule, parser);
    while (node != &ASTNode_fail) {
        node->prev = tail;
        tail->next = node;
        tail = node;
        token_index += tail->ntokens;
        nchildren++;
        node = derived_rule_check(derived_rule, parser);
    }
    //printf("repeat rule %d finished initial check with %zu children\n", repeat_rule->id, nchildren);

    // check for failure of the RepeatRule before allocating any memory
    if (nchildren < self->min_rep || (self->max_rep && nchildren > self->max_rep)) {
        return &ASTNode_fail;
    }
    
    Token token_cur;
    err_type status = parser->_class->get(parser, token_key, &token_cur);
    if (status) {
        printf("unhandled error (%d) in RepeatRule_check_rule_ getting current token from parser\n", status);
    }
    Token token_start;
    status = parser->_class->get(parser, token_key, &token_start);
    if (status) {
        printf("unhandled error (%d) in RepeatRule_check_rule_ getting starting token from parser\n", status);
    }
    return parser->_class->add_node(parser, repeat_rule, token_key, token_index - token_key, (size_t)(token_cur.string - token_start.string), nchildren, child.next, 0);
}

/* NegativeLookahead implementations */

#define NegativeLookahead_NAME "NegativeLookahead.DerivedRule.Rule"

struct NegativeLookaheadType NegativeLookahead_class = {
    .DerivedRule_class = {
        .Rule_class = {
            .type_name = NegativeLookahead_NAME,
            .new = &Rule_new,
            .init = &Rule_init,
            .dest = &Rule_dest,
            .del = &NegativeLookahead_as_Rule_del,
            .check_rule_ = &NegativeLookahead_check_rule_,
            .check = &Rule_check
        },
        .new = &DerivedRule_new,
        .init = &DerivedRule_init,
        .dest = &DerivedRule_dest,
        .del = &NegativeLookahead_as_DerivedRule_del
    },
    .new = &NegativeLookahead_new,
    .init = &NegativeLookahead_init,
    .dest = &NegativeLookahead_dest,
    .del = &NegativeLookahead_del
};

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
    DerivedRule * derived = (DerivedRule *)self;
    derived->_class->init(derived, id, rule);
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
    NegativeLookahead_del((NegativeLookahead *)negative_lookahead);
}
void NegativeLookahead_as_Rule_del(Rule * negative_lookahead) {
    NegativeLookahead_del((NegativeLookahead *)negative_lookahead);
}

ASTNode * NegativeLookahead_check_rule_(Rule * negative_lookahead, Parser * parser, size_t token_key) {
    //printf("checking negative lookahead rule. id: %d\n", negative_lookahead->id);
    NegativeLookahead * self = (NegativeLookahead *)negative_lookahead;
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
    if (node == &ASTNode_fail) {
        return &ASTNode_lookahead;
    }
    parser->_class->seek(parser, token_key, P_SEEK_SET); // pretty sure this isn't necessary
    return &ASTNode_fail;
}
/* PositiveLookahead implementations */

#define PositiveLookahead_NAME "PositiveLookahead.DerivedRule.Rule"

struct PositiveLookaheadType PositiveLookahead_class = {
    .DerivedRule_class = {
        .Rule_class = {
            .type_name = PositiveLookahead_NAME,
            .new = &Rule_new,
            .init = &Rule_init,
            .dest = &Rule_dest,
            .del = &PositiveLookahead_as_Rule_del,
            .check_rule_ = &PositiveLookahead_check_rule_,
            .check = &Rule_check
        },
        .new = &DerivedRule_new,
        .init = &DerivedRule_init,
        .dest = &DerivedRule_dest,
        .del = &PositiveLookahead_as_DerivedRule_del
    },
    .new = &PositiveLookahead_new,
    .init = &PositiveLookahead_init,
    .dest = &PositiveLookahead_dest,
    .del = &PositiveLookahead_del
};

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
    DerivedRule * derived = (DerivedRule *)self;
    derived->_class->init(derived, id, rule);
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
    PositiveLookahead_del((PositiveLookahead *)positive_lookahead);
}
void PositiveLookahead_as_Rule_del(Rule * positive_lookahead) {
    PositiveLookahead_del((PositiveLookahead *)positive_lookahead);
}

ASTNode * PositiveLookahead_check_rule_(Rule * positive_lookahead, Parser * parser, size_t token_key) {
    //printf("checking positive lookahead rule. id: %d\n", positive_lookahead->id);
    PositiveLookahead * self = (PositiveLookahead *)positive_lookahead;
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
    if (node != &ASTNode_fail) {
        parser->_class->seek(parser, token_key, P_SEEK_SET);
        return &ASTNode_lookahead;
    }
    return node;
}

/* Production implementations */

#define Production_NAME "Production.DerivedRule.Rule"

char const * PRODUCTION_UNESCAPED_RULES[3] = {
    Production_NAME,
    LiteralRule_NAME,
    NULL
};

ASTNode * build_action_default(Production * production, Parser * parser, ASTNode * node) {
    if (!is_skip_node(node)) {
        if (!isinstance(node->rule->_class->type_name, PRODUCTION_UNESCAPED_RULES)) {
            LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - re-initializing node rule from id %d to id %d; no node generated\n", __func__, node->rule->id, ((Rule *)production)->id);
            node->rule = (Rule *)production;
        } else {
            LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - creating new rule from production %d to id %d; no node generated\n", __func__, node->rule->id, ((Rule *)production)->id);
            ASTNode * child = node;
            node = parser->_class->add_node(parser, (Rule *)production, child->token_key, child->ntokens, child->str_length, 1, child, 0);
            child->parent = node;
        }
    }
    return node;
}

struct ProductionType Production_class = {  
    .DerivedRule_class = { \
        .Rule_class = { \
            .type_name = Production_NAME,\
            .new = &Rule_new,\
            .init = &Rule_init,\
            .dest = &Rule_dest, \
            .del = &Production_as_Rule_del,\
            .check_rule_ = &Production_check_rule_,\
            .check = &Rule_check \
            }, \
        .new = &DerivedRule_new, \
        .init = &DerivedRule_init, \
        .dest = &DerivedRule_dest, \
        .del = &Production_as_DerivedRule_del \
    }, \
    .new = &Production_new, \
    .init = &Production_init, \
    .dest = &Production_dest, \
    .del = &Production_del \
};

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
    DerivedRule * derived = (DerivedRule *)self;
    derived->_class->init(derived, id, rule);
    self->build_action = build_action;
    return PEGGY_SUCCESS;
}
void Production_dest(Production * self) {
    DerivedRule_dest((DerivedRule *)self);
}
void Production_del(Production * self) {
    Production_dest(self);
    free(self);
}
void Production_as_DerivedRule_del(DerivedRule * production) {
    Production_del((Production *)production);
}
void Production_as_Rule_del(Rule * production) {
    Production_del((Production *)production);
}

ASTNode * Production_check_rule_(Rule * production, Parser * parser, size_t token_key) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - checking production rule. id: %d\n", __func__, production->id);
    Production * self = (Production *)production;
    
    ASTNode * node = self->DerivedRule.rule->_class->check(self->DerivedRule.rule, parser);
    if (node == NULL) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_ERROR, "ERROR: %s - found NULL in call to Rule_check for production id %d\n", __func__, production->id);
    }
    if (node != &ASTNode_fail) {
        return self->build_action(self, parser, node);
    }
    return node;
}
