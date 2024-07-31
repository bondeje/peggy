#include <stddef.h>
#include <stdlib.h>

#include <peggy/utils.h>
#include <peggy/stack.h>
#include <peggy/astnode.h>
#include <peggy/packrat_cache.h>
#include <peggy/parser.h>

#define PACKRAT_CACHE_INIT_SIZE 256

#define TOKEN_TO_UINTPTR_T(ptok) ((uintptr_t)(void *)ptok)

struct PackratCacheType PackratCache_class = {
    .get = &PackratCache_get,
    .set = &PackratCache_set,
    //.rebase = &PackratCache_rebase,
    .dest = &PackratCache_dest,
};

err_type PackratCache_init(PackratCache * cache, size_t nrules, unsigned int flags) {
    cache->map = malloc(sizeof(*cache->map) * nrules);
    if (!cache->map) {
        return PEGGY_MALLOC_FAILURE;
    }
    cache->nrules = nrules;
    cache->offset = 0;
    hash_map_err status = HM_SUCCESS;
    size_t i = 0;
    for (; i < nrules; i++) {
        if ((status = HASH_MAP_INIT(uintptr_t, pASTNode)(&cache->map[i], PACKRAT_CACHE_INIT_SIZE))) {
            break;
        }
    }
    if (status) {
        while (i--) {
            cache->map[i]._class->dest(&cache->map[i]);
        }
        free(cache->map);
        return PEGGY_FAILURE;
    }
    cache->_class = &PackratCache_class;
    if (status) {
        return PEGGY_FAILURE;
    }
    return PEGGY_SUCCESS;
}

void PackratCache_dest(PackratCache * cache) {
    //printf("destroying the PackratCache (%zu)\n", cache->nrules);
    for (size_t i = 0; i < cache->nrules; i++) {
        
        HASH_MAP(uintptr_t, pASTNode) * map = cache->map + i;
        //printf("%p (%zu):", (void*)rule_stack, i);
        map->_class->dest(map);
    }
    free(cache->map);
}

ASTNode * PackratCache_get(PackratCache * cache, rule_id_type rule_id, Token * tok) {
    HASH_MAP(uintptr_t, pASTNode) * map = cache->map + rule_id;
    ASTNode * node = NULL;
    if (map->_class->get(map, TOKEN_TO_UINTPTR_T(tok), &node)) {
        return NULL;
    }
    return node;
}

//err_type PackratCache_get_sparse(PackratCache * cache, rule_id_type rule_id, size_t token_key);
err_type PackratCache_set(PackratCache * cache, Parser * parser, rule_id_type rule_id, Token * tok, ASTNode * node) {
    HASH_MAP(uintptr_t, pASTNode) * map = cache->map + rule_id;
    if (!map->_class->set(map, TOKEN_TO_UINTPTR_T(tok), node)) {
        return PEGGY_FAILURE;
    }
    return PEGGY_SUCCESS;
}

/*
err_type PackratCache_rebase(PackratCache * cache, size_t token_key) {
    cache->offset = token_key;
    return PEGGY_SUCCESS;
}
*/

