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
    ASTNode ** children;
    Rule * rule;
    Token * token_start;
    Token * token_end;
    size_t str_length;
    size_t nchildren;
};

#define ASTNode_NAME "ASTNode"

extern struct ASTNodeType {
    char const * type_name;
} ASTNode_class;

void ASTNode_init(ASTNode * self, Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode ** children);

/************************** general node utilities *****************************/

ASTNode * make_skip_node(ASTNode * node);

_Bool is_skip_node(ASTNode * node);

size_t ASTNode_string_length(ASTNode * node);

#endif // PEGGY_ASTNODE_H

