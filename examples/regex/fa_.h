#ifndef FA__H
#define FA__H

#include "fa.h"

typedef Symbol * pSymbol; // used for stack and hash_map definitions, not for actual use
#define KEY_TYPE pSymbol
#define VALUE_TYPE pSymbol
#define KEY_COMP pSymbol_comp
#define HASH_FUNC pSymbol_hash
#include "peggy/hash_map.h"

#endif

