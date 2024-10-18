#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "peggy/astnode.h"
#include "peggy/rule.h"

struct ASTNodeType ASTNode_class = {
    .type_name = ASTNode_NAME,
};

/**
 * @brief initialize an ASTnode. This is no different than setting each member 
 * in the struct
 */
void ASTNode_init(ASTNode * self, rule_id_type rule, Token * start, Token * end, 
    size_t str_length, size_t nchildren, ASTNode ** children) {
        
    self->_class = &ASTNode_class;
    self->children = children;
    self->nchildren = nchildren;
#if ASTNODE_ADD_PARENT > 0
    for (size_t i = 0; i < self->nchildren; i++) {
        self->children[i]->parent = self;
    }
#endif
    self->rule = rule;
    self->str_length = str_length;
    self->token_start = start;
    self->token_end = end;
}

/************************* general node utilities ****************************/

/**
 * @brief To facilitate the ability to skip tokens, e.g. whitespace, a node is 
 * marked for skipping during tokenization. This should only be used for rules 
 * evaluated during tokenization and the ASTNode should not be used for 
 * anything else after it is marked for skipping
 */
ASTNode * make_skip_node(ASTNode * node) {
    node->rule = -1;
    //node->str_length = node->str_length ? node->str_length : 1;
    return node;
}

/**
 * @brief check if a node has been marked for skipping
 */
_Bool is_skip_node(ASTNode * node) {
    //return node->str_length > 0 && -1 == node->rule;
    return -1 == node->rule;
}

/**
 * @brief calculate the combined token length encompassed by the node and its 
 * children. This is a more accurate and reliable measure of the length of 
 * the underlying string than using ASTNode.str_length
 */
size_t ASTNode_string_length(ASTNode * node) {
    Token * cur = node->token_start;
    Token * end = node->token_end;
    // empty/invalid nodes may not have a token_end or have end at or before cur 
    if (!end || end->id < cur->id) { 
        return 0;
    }
    size_t length = cur->length;
    while (cur != end) {
        cur = cur->next;
        length += cur->length;
    }
    return length;
}

