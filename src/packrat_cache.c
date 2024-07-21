#include <stddef.h>
#include <stdlib.h>

#include <peggy/utils.h>
#include <peggy/stack.h>
#include <peggy/astnode.h>
#include <peggy/packrat_cache.h>
#include <peggy/parser.h>

#define PACKRAT_CACHE_INIT_SIZE 256

struct PackratCacheType PackratCache_class = {
    .get = &PackratCache_get,
    .set = &PackratCache_set,
    .rebase = &PackratCache_rebase,
    .dest = &PackratCache_dest,
};

err_type PackratCache_init(PackratCache * cache, size_t nrules, unsigned int flags) {
    cache->cache_ = malloc(sizeof(*cache->cache_) * nrules);
    if (!cache->cache_) {
        return PEGGY_MALLOC_FAILURE;
    }
    cache->nrules = nrules;
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

    return status;
}

void PackratCache_dest(PackratCache * cache) {
    //printf("destroying the PackratCache (%zu)\n", cache->nrules);
    for (size_t i = 0; i < cache->nrules; i++) {
        
        STACK(PackratCacheNode) * rule_stack = cache->cache_ + i;
        //printf("%p (%zu):", (void*)rule_stack, i);
        rule_stack->_class->dest(rule_stack);
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

//err_type PackratCache_get_sparse(PackratCache * cache, rule_id_type rule_id, size_t token_key);
err_type PackratCache_set(PackratCache * cache, Parser * parser, rule_id_type rule_id, size_t token_key, ASTNode * node) {
    if (token_key < cache->offset) {
        return PEGGY_INDEX_OUT_OF_BOUNDS;
    }
    size_t index = token_key - cache->offset;
    STACK(PackratCacheNode) * rule_stack = cache->cache_ + rule_id;
    if (index >= rule_stack->fill) {
        size_t new_cap = parser->_class->estimate_final_ntokens_(parser);
        //printf("\n\nresizing PackratCache stack for rule id %d (%zu / %zu / %zu / %zu)...", rule_id, index, rule_stack->fill, parser->tokens.fill, new_cap);
        rule_stack->_class->resize(rule_stack, new_cap);
        
        rule_stack->fill = rule_stack->capacity; // can do this because resize will initialize to 0s, which is valid in our case
        //printf("to %zu\n\n", rule_stack->fill);
    }
    return rule_stack->_class->set(rule_stack, index, (PackratCacheNode) {.node = node, .loc = token_key});
}

err_type PackratCache_rebase(PackratCache * cache, size_t token_key) {
    cache->offset = token_key;
    return PEGGY_SUCCESS;
}