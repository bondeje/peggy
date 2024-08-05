#ifndef PEGGY_ASTNODE_H
#define PEGGY_ASTNODE_H

#include <stddef.h>

#include <peggy/utils.h>
#include <peggy/token.h>
//#include <peggy/type.h>

#define ASTNode_DEFAULT_INIT { \
    ._class = &ASTNode_class, \
}

typedef struct ASTNode ASTNode, * pASTNode;

struct ASTNode {
    struct ASTNodeType * _class;
    ASTNode * child; // leftmost child of node
    ASTNode * next; // next sibling left-right
    ASTNode * prev; // prev sibling left-right
    ASTNode * parent; // parent of node
    Rule * rule;
    size_t str_length;
    size_t nchildren;
    Token * token_start;
    Token * token_end;
};

#define ASTNode_NAME "ASTNode"

extern struct ASTNodeType {
    char const * type_name;
} ASTNode_class;

ASTNode * ASTNode_new(Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode * children);
void ASTNode_init(ASTNode * self, Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode * children);
void ASTNode_del(ASTNode * self);

/************************** general node utilities *****************************/

ASTNode * make_skip_node(ASTNode * node);

bool is_skip_node(ASTNode * node);

#endif // PEGGY_ASTNODE_H

