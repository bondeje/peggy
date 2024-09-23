#ifndef PEGGY_ASTNODE_H
#define PEGGY_ASTNODE_H

#include <stddef.h>

#include <peggy/utils.h>
#include <peggy/token.h>

// default initializer for ASTNode to ensure the vtable is available
#define ASTNode_DEFAULT_INIT { \
    ._class = &ASTNode_class, \
}

typedef struct ASTNode ASTNode, * pASTNode;

/*
The base structure for the ASTNode
*/
struct ASTNode {
    struct ASTNodeType * _class;//!< vtable
#if ASTNODE_ADD_PARENT > 0
    ASTNode * parent;           //!< parent reference in ASTNode
#endif
    ASTNode ** children;        //!< Child node references. Note the array's memory is NOT owned by the node
    //Rule * rule;                //!< Pointer to the rule that succeeded in creation of the ASTNode
    Token * token_start;        //!< Initial token encompassed by the ASTNode and its children
    Token * token_end;          //!< Final token encompassed by the ASTNode and its children
    size_t nchildren;           //!< Number of child nodes
    size_t str_length;          //!< This is not what you think it is. Internal use only. DO NOT USE
    rule_id_type rule;                   //!< integer for the rule id enum
};

#define ASTNode_NAME "ASTNode"

// vtable current only has one element tracking the node type.
extern struct ASTNodeType {
    char const * type_name;
} ASTNode_class;

/**
 * @brief initialize an ASTnode. This is no different than setting each member 
 * in the struct
 */
void ASTNode_init(ASTNode * self, rule_id_type rule, Token * start, Token * end, 
    size_t str_length, size_t nchildren, ASTNode ** children);

/************************* general node utilities ****************************/

/**
 * @brief To facilitate the ability to skip tokens, e.g. whitespace, a node is 
 * marked for skipping during tokenization. This should only be used for rules 
 * evaluated during tokenization and the ASTNode should not be used for 
 * anything else after it is marked for skipping
 */
ASTNode * make_skip_node(ASTNode * node);

/**
 * @brief check if a node has been marked for skipping
 */
_Bool is_skip_node(ASTNode * node);

/**
 * @brief calculate the combined token length encompassed by the node and its 
 * children. This is a more accurate and reliable measure of the length of 
 * the underlying string than using ASTNode.str_length
 */
size_t ASTNode_string_length(ASTNode * node);

#endif // PEGGY_ASTNODE_H

