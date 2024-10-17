#include "cpp.h"
#include "cstring.h"

typedef struct Macro {
    Token * id;
    CString * params;
    size_t nparams;
    char const * string;
} Macro, *pMacro;

#define KEY_TYPE pToken
#define VALUE_TYPE pMacro
#define KEY_COMP Token_scomp
#define HASH_FUNC Token_shash
#include <peggy/hash_map.h>

struct CPreProcessor {
    HASH_MAP(pToken, pMacro) defines;
};

int CPP_directive(Parser * parser, CPP * cpp, ASTNode * directive) {
    return 0;
}

int CPP_check(Parser * parser, CPP * cpp, ASTNode * identifier) {
    return 0;
}
