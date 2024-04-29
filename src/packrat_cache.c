#include <stddef.h>
#include <stdlib.h>

#include <peggy/utils.h>
#include <peggy/stack.h>
#include <peggy/astnode.h>
#include <peggy/packrat_cache.h>

#define PACKRAT_CACHE_INIT_SIZE 256

struct PackratCacheType PackratCache_class = {
    .get = &PackratCache_get,
    .set = &PackratCache_set,
    .rebase = &PackratCache_rebase,
    .dest = &PackratCache_dest,
    .resize = &PackratCache_resize
};

err_type PackratCache_init(PackratCache * cache, size_t nrules, unsigned int flags) {
    cache->cache_ = malloc(sizeof(*cache->cache_) * nrules);
    if (!cache->cache_) {
        return PEGGY_MALLOC_FAILURE;
    }
    err_type status = PEGGY_SUCCESS;
    size_t i = 0;
    for (; i < nrules; i++) {
        if ((status = STACK_INIT(PackratCacheNode)(&cache->cache_[i], PACKRAT_CACHE_INIT_SIZE))) {
            break;
        }
    }
    if (status) {
        while (i--) {
            cache->cache_[i]._class->dest(&cache->cache_[i]);
        }
        free(cache->cache_);
        return status;
    }
    cache->_class = &PackratCache_class;

    /*
    if (flags & PACKRAT_SPARSE) {
        cache->_class = &PackratCache_sparse_class;
    } else {
        
    }
    */
    return status;
}

void PackratCache_dest(PackratCache * cache) {
    for (size_t i = 0; i < cache->nrules; i++) {
        cache->cache_[i]._class->dest(&cache->cache_[i]);
    }
    free(cache->cache_);
}

ASTNode * PackratCache_get(PackratCache * cache, rule_id_type rule_id, size_t token_key) {
    if (token_key < cache->offset) {
        return NULL;
    }
    STACK(PackratCacheNode) * rule_stack = cache->cache_ + rule_id;
    PackratCacheNode node;
    if (rule_stack->_class->get(rule_stack, token_key - cache->offset, &node)) {
        return NULL;
    }
    return node.node;
}

err_type PackratCache_resize(PackratCache * cache, size_t new_min_capacity) {
    //printf("resizing PackratCache\n");
    err_type status = PEGGY_SUCCESS;
    for (size_t i = 0; i < cache->nrules; i++) {
        STACK(PackratCacheNode) * rule_stack = cache->cache_ + i;
        if ((status = rule_stack->_class->resize(rule_stack, new_min_capacity))) {
            return status;
        }
        rule_stack->fill = rule_stack->capacity;
    }
    return PEGGY_SUCCESS;
}

//err_type PackratCache_get_sparse(PackratCache * cache, rule_id_type rule_id, size_t token_key);
err_type PackratCache_set(PackratCache * cache, rule_id_type rule_id, size_t token_key, ASTNode * node) {
    if (token_key < cache->offset) {
        return PEGGY_INDEX_OUT_OF_BOUNDS;
    }
    size_t index = token_key - cache->offset;
    STACK(PackratCacheNode) * rule_stack = cache->cache_ + rule_id;
    if (index >= rule_stack->fill) {
        size_t new_capacity = 2 * (index + 1);
        if (new_capacity > rule_stack->capacity) {
            rule_stack->_class->resize(rule_stack, new_capacity);
        }
        rule_stack->fill = rule_stack->capacity - 1; // can do this because resize will initialize to 0s, which is valid in our case
    }
    PackratCacheNode dummy = {.node = NULL, .loc = rule_stack->fill};
    if (index >= rule_stack->fill) {
        err_type status = PEGGY_SUCCESS;
        err_type (*push)(STACK(PackratCacheNode) *, PackratCacheNode) = rule_stack->_class->push;
        while (index >= rule_stack->fill) {
            if ((status = push(rule_stack, dummy))) {
                return status;
            }
        }
    }
    dummy.node = node;
    dummy.loc = token_key;
    return rule_stack->_class->set(rule_stack, index, dummy);
}

err_type PackratCache_rebase(PackratCache * cache, size_t token_key) {
    cache->offset = token_key;
    return PEGGY_SUCCESS;
}