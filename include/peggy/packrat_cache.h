/**
 * PackratCache implements the memoization cache for the packrat parser.
 * 
 * The keys into the cache are the integer for the rule being executed and the 
 * pointer to the token at which it began its evaluation. Within a single 
 * "Parser_parse" call, both of these values should be unique. Compiled 
 * grammars from peggy guarantee that the rule values are unique per grammar
 * and any single Parse will ensure that the token metadata is unique enough.
 * This cache is one of the reasons Parser structs are definitely not thread-
 * safe.
 * 
 * WARNING: I tried what I thought would be more a more sensible implementation
 * using fully linked ASTNodes to represent the tree, but because all of those
 * links are saved when caching/retrieving, it wreak's havoc on traversals as
 * many of the links become stale and remain when they are retrieved from the 
 * cache. So I strongly recommend not using an ASTNode that has any direct 
 * linkages to other ASTNodes
 * 
 * There are two implementations separated by the macro PACKRAT_HASH. With it enabled, 
 */

#ifndef PACKRAT_CACHE
#define PACKRAT_CACHE

#include <stddef.h>
#include <stdint.h>

#include <peggy/token.h>
#include <peggy/astnode.h>

/**
 * compile with PACKRAT_HASH defined to switch the underlying representation to
 * a hash-map. NOTE: this is actually slower than the array building despite 
 * its reallocation inefficiences
 */
#ifdef PACKRAT_HASH
#define KEY_TYPE size_t
#define KEY_COMP size_t_comp
#define HASH_FUNC size_t_hash
#define VALUE_TYPE pASTNode
#include <peggy/hash_map.h>
#endif

typedef struct PackratCache PackratCache;
typedef struct PackratCacheType PackratCacheType;

struct PackratCache {
#ifdef PACKRAT_HASH
    HASH_MAP(size_t, pASTNode) * map; // array of HASH_MAPs
#else
    ASTNode *** map;    // 2-dimensional array of ASTNodes
    size_t * caps;      // one cap for each row of map
#endif
    size_t nrules;
};

/**
 * @brief initialize a packrat cache
 * @param[in] cache pointer to the packrat cache
 * @param[in] nrules the number of rules encompassed by the cache
 * @param[in] init_cap initial capacity of the packrat cache. A default value 
 *      is internally provided if set to 0
 * @returns a non-zero value if the initialization fails else 0
 */
err_type PackratCache_init(PackratCache * cache, size_t nrules, size_t init_cap);

/**
 * @brief destroy packrat cache and reclaim memory
 * @param[in] cache pointer to the packrat cache
 */
void PackratCache_dest(PackratCache * cache);

/**
 * @brief attempt to retrieve a saved node for a probe rule-token pair
 * @param[in] cache pointer to the packrat cache
 * @param[in] rule_id the enum value corresponding to the rule being probed
 * @param[in] tok pointer to a token being probed
 * @returns NULL if rule-token pair has not been encountered before else a 
 *      pointer to the ASTNode that resulted from applying rule at specified 
 *      token
 */
ASTNode * PackratCache_get(PackratCache * cache, rule_id_type rule_id, Token * tok);

/**
 * @brief set a value for a rule-token pair in the packrat cache
 * @param[in] cache pointer to the packrat cache
 * @param[in] rule_id the enum value corresponding to the rule being assigned
 * @param[in] tok pointer to a token being assigned
 * @param[in] node pointer to ASTNode that is the result of applying rule at 
 *      token
 * @returns non-zero on error else 0
 */
err_type PackratCache_set(PackratCache * cache, Parser * parser, rule_id_type rule_id, Token * tok, ASTNode * node);
//err_type PackratCache_rebase(PackratCache * cache, size_t token_key); //not used yet

#endif // PACKRAT_CACHE

