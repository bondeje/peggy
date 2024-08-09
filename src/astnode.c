#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <peggy/astnode.h>
#include <peggy/rule.h>

struct ASTNodeType ASTNode_class = {
    .type_name = ASTNode_NAME,
};

void ASTNode_init(ASTNode * self, Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode ** children) {
    self->_class = &ASTNode_class;
    self->children = children;
    self->nchildren = nchildren;
    self->rule = rule;
    self->str_length = str_length;
    self->token_start = start;
    self->token_end = end;
}

// helper functions

ASTNode * make_skip_node(ASTNode * node) {
    node->rule = NULL;
    node->str_length = node->str_length ? node->str_length : 1;
    return node;
}

_Bool is_skip_node(ASTNode * node) {
    return node->str_length > 0 && !(node->rule);
}

size_t ASTNode_string_length(ASTNode * node) {
    Token * cur = node->token_start;
    Token * end = node->token_end;
    size_t length = cur->length;
    while (cur != end) {
        cur = cur->next;
        length += cur->length;
    }
    return length;
}

