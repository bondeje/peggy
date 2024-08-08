#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <peggy/astnode.h>
#include <peggy/rule.h>

struct ASTNodeType ASTNode_class = {
    .type_name = ASTNode_NAME,
};

void ASTNode_init(ASTNode * self, Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode * child) {
    self->_class = &ASTNode_class;
    self->child = child;
    self->nchildren = nchildren;
    if (child) {
        nchildren--;
        child->prev = NULL;
        child->parent = self;
        while (nchildren--) {
            child = child->next;
            child->parent = self;
        }
        child->next = NULL;
    }
    self->parent = NULL;
    self->next = NULL;
    self->prev = NULL;
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

bool is_skip_node(ASTNode * node) {
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

ASTNode * ASTNode_last_child(ASTNode * node) {
    if (!node || !node->nchildren) {
        return NULL;
    }
    ASTNode * child = node->child;
    size_t i = 1;
    while (child->next && i < node->nchildren) {
        child = child->next;
        i++;
    }
    return child;
}