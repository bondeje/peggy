#include <stddef.h>
#include <stdlib.h>

#include <peggy/utils.h>
#include <peggy/stack.h>
#include <peggy/astnode.h>
#include <peggy/packrat_cache.h>
#include <peggy/parser.h>

#ifdef PACKRAT_HASH
#define PACKRAT_CACHE_INIT_CAP 127
#else
#define PACKRAT_CACHE_INIT_CAP 256
#endif

#define TOKEN_TO_UINTPTR_T(ptok) ((size_t)(void *)ptok)

struct PackratCacheType PackratCache_class = {
    .get = &PackratCache_get,
    .set = &PackratCache_set,
    //.rebase = &PackratCache_rebase,
    .dest = &PackratCache_dest,
};

err_type PackratCache_init(PackratCache * cache, size_t nrules, size_t init_cap, unsigned int flags) {
    cache->nrules = nrules;
    if (!init_cap) {
        init_cap = PACKRAT_CACHE_INIT_CAP;
    }
    //cache->offset = 0;
    cache->_class = &PackratCache_class;
#ifdef PACKRAT_HASH
    cache->map = malloc(sizeof(*cache->map) * nrules);
    if (!cache->map) {
        return PEGGY_MALLOC_FAILURE;
    }
    hash_map_err status = HM_SUCCESS;
    size_t i = 0;
    for (; i < nrules; i++) {
        if ((status = HASH_MAP_INIT(size_t, pASTNode)(&cache->map[i], init_cap))) {
            break;
        }
    }
    if (status) {
        while (i--) {
            cache->map[i]._class->dest(cache->map + i);
        }
        free(cache->map);
        return PEGGY_FAILURE;
    }
    if (status) {
        return PEGGY_FAILURE;
    }
#else
    err_type status = PEGGY_SUCCESS;
    cache->map = calloc(nrules, sizeof(*cache->map));
    if (!cache->map) {
        return PEGGY_MALLOC_FAILURE;
    }
    cache->caps = calloc(nrules, sizeof(*cache->caps));
    if (!cache->caps) {
        free(cache->map);
        return PEGGY_MALLOC_FAILURE;
    }
    size_t i = 0;
    for (; i < nrules; i++) {
        if (!(cache->map[i] = calloc(init_cap, sizeof(*cache->map[i])))) {
            status = PEGGY_MALLOC_FAILURE;
            break;
        }
        cache->caps[i] = init_cap;
    }
    if (status) {
        while (i--) {
            free(cache->map[i]);
        }
        free(cache->map);
        free(cache->caps);
    }
#endif
    
    return PEGGY_SUCCESS;
}

void PackratCache_dest(PackratCache * cache) {
    //printf("destroying the PackratCache (%zu)\n", cache->nrules);
    for (size_t i = 0; i < cache->nrules; i++) {
        
#ifdef PACKRAT_HASH
        HASH_MAP(size_t, pASTNode) * map = cache->map + i;
        map->_class->dest(map);
#else
        free(cache->map[i]);
#endif
        //printf("%p (%zu):", (void*)rule_stack, i);
    }
#ifndef PACKRAT_HASH
    free(cache->caps);
#endif
    free(cache->map);
}

ASTNode * PackratCache_get(PackratCache * cache, rule_id_type rule_id, Token * tok) {
    
    //printf("getting rule_id %d for token id %zu", rule_id, tok->id);
#ifdef PACKRAT_HASH
    ASTNode * node = NULL;
    HASH_MAP(size_t, pASTNode) * map = cache->map + rule_id;
    if (map->_class->get(map, tok->id, &node)) {
        node = NULL;
    }  
#else
    if ((size_t)rule_id >= cache->nrules || tok->id >= cache->caps[rule_id]) {
        return NULL;
    }
    return cache->map[rule_id][tok->id];
#endif
}

//err_type PackratCache_get_sparse(PackratCache * cache, rule_id_type rule_id, size_t token_key);
err_type PackratCache_set(PackratCache * cache, Parser * parser, rule_id_type rule_id, Token * tok, ASTNode * node) {
    //printf("setting rule_id %d for token id %zu", rule_id, tok->id);
#ifdef PACKRAT_HASH
    HASH_MAP(size_t, pASTNode) * map = cache->map + rule_id;
    if (map->_class->set(map, tok->id, node)) {
        printf("packrat for rule id %d starts failing at index %zu\n", rule_id, tok->id);
        return PEGGY_FAILURE;
    }
#else
    if ((size_t)rule_id >= cache->nrules) {
        printf("packrat get invalid rule request for rule id %d (%zu available)\n", rule_id, cache->nrules);
        return PEGGY_FAILURE;
    }
    if (tok->id >= cache->caps[rule_id]) {
        size_t new_cap = 2 * (tok->id > 2 * cache->caps[rule_id] ? tok->id : cache->caps[rule_id]);
        //printf("resizing hash %zu to %zu\n",map->capacity, new_cap);
        ASTNode ** new_row = calloc(new_cap, sizeof(*new_row));
        if (!new_row) {
            printf("packrat get for rule id %d starts failing at index %zu\n", rule_id, tok->id);
            return PEGGY_MALLOC_FAILURE;
        }
        memcpy(new_row, cache->map[rule_id], cache->caps[rule_id] * sizeof(*new_row));
        free(cache->map[rule_id]);
        cache->map[rule_id] = new_row;
        cache->caps[rule_id] = new_cap;
        new_row[tok->id] = node;
    } else {
        cache->map[rule_id][tok->id] = node;
    }
    
#endif
    
    //printf("...done\n");
    return PEGGY_SUCCESS;
}

/*
err_type PackratCache_rebase(PackratCache * cache, size_t token_key) {
    cache->offset = token_key;
    return PEGGY_SUCCESS;
}
*/

