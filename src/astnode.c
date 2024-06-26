#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <peggy/astnode.h>
#include <peggy/rule.h>

struct ASTNodeType ASTNode_class = {
    .type_name = ASTNode_NAME,
    .new = &ASTNode_new,
    .init = &ASTNode_init,
    .dest = &ASTNode_dest,
    .del = &ASTNode_del,
    .iter = &ASTNode_iter,
    .reverse = &ASTNode_reverse,
    .get = &ASTNode_get,
    .len = &ASTNode_len,
    .str = &ASTNode_str,
};
ASTNode ASTNode_fail = ASTNode_DEFAULT_INIT;
ASTNode ASTNode_lookahead = {
    ._class = &ASTNode_class,
    .children = NULL,
    .rule = NULL,
    .str_length = 1, /* this indicates the node is skipped/not processed  */
    .nchildren = 0,
    .token_key = 0,
    .ntokens = 0, /* ntokens should be 0 because no tokens are consumed */
};

ASTNode * ASTNode_new(Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode ** children) {
    ASTNode * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    *ret = (ASTNode) ASTNode_DEFAULT_INIT;
    if (ASTNode_init(ret, rule, token_key, ntokens, str_length, nchildren, children)) {
        free(ret);
        ret = NULL;
    }
    return ret;
}
err_type ASTNode_init(ASTNode * self, Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode ** children) {
    self->children = children;
    self->rule = rule;
    self->str_length = str_length;
    self->nchildren = nchildren;
    self->token_key = token_key;
    self->ntokens = ntokens;
    return PEGGY_SUCCESS;
}
void ASTNode_dest(ASTNode * self) { 
    // no-op. ASTNode does not own any contents that need to be destroyed. children are owned externally
    if (self->children) {
        free(self->children);
    }
    self->children = NULL;
    self->nchildren = 0;
}
void ASTNode_del(ASTNode * self) { 
    if (self != &ASTNode_fail) {
        ASTNode_dest(self);
        free(self);
    }
}
err_type ASTNode_iter(ASTNode * self, ASTNodeIterator * node_iter) {
    /* TODO */
    return PEGGY_NOT_IMPLEMENTED;
}
err_type ASTNode_reverse(ASTNode * self, ASTNodeIterator * node_iter) {
    /* TODO */
    return PEGGY_NOT_IMPLEMENTED;
}
err_type ASTNode_get(ASTNode * self, size_t key, ASTNode ** value) {
    /* TODO */
    return PEGGY_NOT_IMPLEMENTED;
}
size_t ASTNode_len(ASTNode * self) {
    return self->nchildren;
}
// should return result of underlying sprintf
int ASTNode_str(ASTNode * self, char * buffer, size_t buf_size) {
    /* TODO */
    return 0;
}

struct ASTNodeIteratorType ASTNodeIterator_class = {
    .type_name = ASTNodeIterator_NAME,
    .new = &ASTNodeIterator_new,
    .init = &ASTNodeIterator_init,
    .del = &ASTNodeIterator_del,
    .next = &ASTNodeIterator_next,
    .iter = &ASTNodeIterator_iter,
    .reverse = &ASTNodeIterator_reverse,
//    .copy = &ASTNode_copy,
};

ASTNodeIterator * ASTNodeIterator_new(ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step) {
    /* TODO */
    return NULL;
}
err_type ASTNodeIterator_init(ASTNodeIterator * self, ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step) {
    /* TODO */
    return PEGGY_NOT_IMPLEMENTED;
}
void ASTNodeIterator_del(ASTNodeIterator * self) {
    free(self);
}
iter_status ASTNodeIterator_next(ASTNodeIterator * self, ASTNode ** val) {
    /* TODO */
    return PEGGY_ITER_GO;
}
err_type ASTNodeIterator_iter(ASTNodeIterator * self, ASTNodeIterator * other) {
    /* TODO */
    return PEGGY_NOT_IMPLEMENTED;
}
err_type ASTNodeIterator_reverse(ASTNodeIterator * self, ASTNodeIterator * other) {
    /* TODO */
    return PEGGY_NOT_IMPLEMENTED;
}

/* helper functions */

ASTNode * make_skip_node(ASTNode * node) {
    node->rule = NULL;
    node->str_length = node->str_length ? node->str_length : 1;
    return node;
}

bool is_skip_node(ASTNode * node) {
    return node->str_length > 0 && !(node->rule);
}