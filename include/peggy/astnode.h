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

extern ASTNode ASTNode_fail;
extern ASTNode ASTNode_lookahead;

#define ASTNodeIterator_DEFAULT_INIT {._class = &ASTNodeIterator_class, \
                                        .node_array = NULL, \
                                        .start = 0, \
                                        .stop = 0, \
                                        .step = 0, \
                                        .cur = 0 \
                                        }

typedef struct ASTNodeIterator ASTNodeIterator;

/**
 * notes for cexpress: An iterator type should be able to take a raw object or 
 * another iterator of its own type. I don't want to have to go down the path 
 * of making an *IteratorIterator that iterates over an *Iterator like I did 
 * with ContLibs. That was pretty stupid and I think there's a more elegant way
 */
struct ASTNodeIterator {
    struct ASTNodeIteratorType * _class;
    ASTNode * parent_node;
    size_t start;
    size_t stop;
    ptrdiff_t step;
    size_t cur;
};

#define ASTNode_NAME "ASTNode"

extern struct ASTNodeType {
    char const * type_name;
    ASTNode * (*new)(Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode * child);
    err_type (*init)(ASTNode * self, Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode * child);
    void (*dest)(ASTNode * self);
    void (*del)(ASTNode * self);
    err_type (*iter)(ASTNode * self, ASTNodeIterator * node_iter);
    err_type (*reverse)(ASTNode * self, ASTNodeIterator * node_iter);
    err_type (*get)(ASTNode * self, size_t key, ASTNode ** value);
    size_t (*len)(ASTNode * self);
    int (*str)(ASTNode * self, char * buffer, size_t buf_size);
} ASTNode_class;

ASTNode * ASTNode_new(Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode * children);
err_type ASTNode_init(ASTNode * self, Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode * children);
void ASTNode_dest(ASTNode * self);
void ASTNode_del(ASTNode * self);
err_type ASTNode_iter(ASTNode * self, ASTNodeIterator * node_iter);
err_type ASTNode_reverse(ASTNode * self, ASTNodeIterator * node_iter);
err_type ASTNode_get(ASTNode * self, size_t key, ASTNode ** value);
size_t ASTNode_len(ASTNode * self);
int ASTNode_str(ASTNode * self, char * buffer, size_t buf_size);

#define ASTNodeIterator_NAME "ASTNodeIterator"

extern struct ASTNodeIteratorType {
    char const * type_name;
    ASTNodeIterator * (*new)(ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step);
    err_type (*init)(ASTNodeIterator * self, ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step);
    void (*del)(ASTNodeIterator * self);
    iter_status (*next)(ASTNodeIterator * self, ASTNode ** val);
    err_type (*iter)(ASTNodeIterator * self, ASTNodeIterator * node_iter);
    err_type (*reverse)(ASTNodeIterator * self, ASTNodeIterator * node_iter);
} ASTNodeIterator_class;

ASTNodeIterator * ASTNodeIterator_new(ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step);
err_type ASTNodeIterator_init(ASTNodeIterator * self, ASTNode * parent_node, size_t start, size_t stop, ptrdiff_t step);
void ASTNodeIterator_del(ASTNodeIterator * self);
iter_status ASTNodeIterator_next(ASTNodeIterator * self, ASTNode ** val);
err_type ASTNodeIterator_iter(ASTNodeIterator * self, ASTNodeIterator * other);
err_type ASTNodeIterator_reverse(ASTNodeIterator * self, ASTNodeIterator * other);

/************************** general node utilities *****************************/

/* helper functions */

ASTNode * make_skip_node(ASTNode * node);

bool is_skip_node(ASTNode * node);

#endif // PEGGY_ASTNODE_H

