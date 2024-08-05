#ifndef PACKRAT_CACHE
#define PACKRAT_CACHE

#include <stddef.h>
#include <stdint.h>

#include <peggy/token.h>
#include <peggy/astnode.h>

typedef struct PackratCacheNode {
    ASTNode * node;
    Token * token;
} PackratCacheNode;

/*
int PackratCacheNode_comp(PackratCacheNode a, PackratCacheNode b) {
    if (a.node != b.node) {
        return 1;
    }
    if (a.token > b.token) {
        return 1;
    } else if (a.token < b.token) {
        return -1;
    }
    return 0;
}
*/

#ifdef PACKRAT_HASH
#define KEY_TYPE size_t
#define KEY_COMP size_t_comp
#define HASH_FUNC size_t_hash
#define VALUE_TYPE pASTNode
#include <peggy/hash_map.h>
#else
//#define ELEMENT_TYPE pASTNode
//#include <peggy/stack.h>
#endif

#define PACKRAT_DEFAULT 0
#define PACKRAT_SPARSE 1

typedef struct PackratCache PackratCache;
typedef struct PackratCacheType PackratCacheType;

struct PackratCache {
    PackratCacheType * _class;
#ifdef PACKRAT_HASH
    HASH_MAP(size_t, pASTNode) * map;
#else
    ASTNode *** map; // 2-dimensional array
    size_t * caps; // one cap for each row of map
#endif
    size_t nrules;
    //size_t offset; // offset for input pointers to convert to array indices
    unsigned int flags; // bit options
};

err_type PackratCache_init(PackratCache * cache, size_t nrules, size_t init_cap, unsigned int flags);
ASTNode * PackratCache_get(PackratCache * cache, rule_id_type rule_id, Token * tok);
//ASTNode * PackratCache_get_sparse(PackratCache * cache, rule_id_type rule_id, size_t token_key);
err_type PackratCache_set(PackratCache * cache, Parser * parser, rule_id_type rule_id, Token * tok, ASTNode * node);
// TODO: need a set parser, too, but that is probably a bad idea.
err_type PackratCache_rebase(PackratCache * cache, size_t token_key);
void PackratCache_dest(PackratCache * cache);
err_type PackratCache_resize(PackratCache * cache, size_t new_min_capacity);

extern struct PackratCacheType {
    ASTNode * (*get)(PackratCache * cache, rule_id_type rule_id, Token * tok);
    err_type (*set)(PackratCache * cache, Parser * parser, rule_id_type rule_id, Token * tok, ASTNode * node);
    err_type (*rebase)(PackratCache * cache, size_t token_key);
    void (*dest)(PackratCache * cache);
    //err_type (*resize)(PackratCache * cache, size_t new_min_capacity);
} PackratCache_class;/*, PackratCache_sparse_class = {
    .get = &PackratCache_get_sparse,
    .set = &PackratCache_set,
    .rebase = &PackratCache_rebase,
    .dest = &PackratCache_dest
};*/


#endif // PACKRAT_CACHE

