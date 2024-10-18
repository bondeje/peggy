#include "c.h"
#include "cpp.h"
#include "cstring.h"

#define DEFAULT_MACRO_CAPACITY 7

enum MacroType {
    MACRO,
    MACRO_FUNCTION
};

typedef struct Macro {
    Token * id;
    Token * rep_start; // start of replacement tokens
    Token * rep_end; // end of replacement tokens (inclusive)
    enum MacroType type;
} Macro, *pMacro;

BUILD_ALIGNMENT_STRUCT(Macro)

#define KEY_TYPE pToken
#define VALUE_TYPE pMacro
#define KEY_COMP Token_scomp
#define HASH_FUNC Token_shash
#include "peg4c/hash_map.h"

struct CPreProcessor {
    HASH_MAP(pToken, pMacro) defines;
    MemPoolManager * macro_mgr;
};

BUILD_ALIGNMENT_STRUCT(CPP)

// allocate CPP from a pool
CPP * CPP_new(void) {
    CPP * out = malloc(sizeof(CPP));
    if (!out) {
        return out;
    }
    *out = (CPP){0};
    out->macro_mgr = MemPoolManager_new(128, sizeof(Macro), _Alignof(Macro));
    return out;
}

void CPP_del(CPP * self) {
    MemPoolManager_del(self->macro_mgr);
    if (self->defines.capacity) {
        self->defines._class->dest(&self->defines);
    }
    free(self);
}

void CPP_define(CPP * cpp, ASTNode * define) {
    if (!cpp->defines.capacity) {
        HASH_MAP_INIT(pToken, pMacro)(&cpp->defines, DEFAULT_MACRO_CAPACITY);
    }
    Macro * macro = MemPoolManager_next(cpp->macro_mgr);
    *macro = (Macro){
        .id = define->children[2]->token_start,
        .type = define->nchildren > 5 ? MACRO_FUNCTION : MACRO
    };
    ASTNode * rep_list = define->children[define->nchildren - 2];
    if (rep_list->nchildren) {
        macro->rep_start = rep_list->token_start;
        macro->rep_end = rep_list->token_end;    
    }
    cpp->defines._class->set(&cpp->defines, macro->id, macro);
}

void CPP_undef(CPP * cpp, ASTNode * undef) {
    cpp->defines._class->remove(&cpp->defines, undef->children[2]->token_start);
}

void CPP_get_directive_line(ASTNode * line, Token ** start, Token ** end) {
    *start = line->token_start;
    Token * cur = (*start);
    while (cur->string[0] != '\n') {
        cur = cur->next;
    }
    *end = cur;
}

int CPP_directive(Parser * parser, CPP * cpp) {
    ASTNode * pp_key = Rule_check(crules[C_PP_CHECK], parser);
    if (!pp_key) {
        return 1;
    }
    ASTNode * key = pp_key->children[1]->children[0];

    Parser_seek(parser, pp_key->token_start);
    switch (key->rule) {
        case C_DEFINE_KW: {
            // parse
            ASTNode * define = Rule_check(crules[C_CONTROL_LINE], parser);

            // register define
            CPP_define(cpp, define);

            // remove directive line
            Token_remove_tokens(define->token_start, define->token_end);
            break;
        }
        case C_UNDEF_KW: {
            CPP_undef(cpp, Rule_check(crules[C_CONTROL_LINE], parser));
            break;
        }
        case C_IF_KW:
        case C_IFDEF_KW:
        case C_IFNDEF_KW: {
            break;
        }
        case C_ELIF_KW: {
            break;
        }
        case C_ELIFDEF_KW: {
            break;
        }
        case C_ELIFNDEF_KW: {
            break;
        }
        case C_ELSE_KW: {
            break;
        }
        case C_ENDIF_KW: {
            break;
        }
        case C_INCLUDE_KW: {
            break;
        }
        case C_EMBED_KW: {
            break;
        }
        case C_LINE_KW: {
            break;
        }
        case C_ERROR_KW: {
            break;
        }
        case C_WARNING_KW: {
            break;
        }
        case C_PRAGMA_KW: {
            break;
        }
        case C__PRAGMA_KW: {
            break;
        }
        default: {
            return 1;
        }
    }

    return 0;
}

// TODO: add compatibility with macro functions
int CPP_check(Parser * parser, CPP * cpp, ASTNode * id_re) {
    Macro * macro = NULL;
    Token * start = id_re->token_start;
    // since this only occurs during tokenizing, the token->length is not reliable.
    // need to overwrite with node->str_length
    Token * key = &(Token){.string = start->string, .length = id_re->str_length};
    // retrieve the macro if present
    if (!cpp->defines.capacity || cpp->defines._class->get(&cpp->defines, key, &macro)) {
        return 1;
    }
    if (MACRO_FUNCTION == macro->type) {
        // replace the tokens in the stream with the tokens in the macro.
        // it should be OK
        fprintf(stderr, "CPP_check: macro function %.*s found but macro functions not implement\n", (int)start->length, start->string);
        exit(1);
    }
    if (macro->rep_start && macro->rep_end) {
        // insert before because a successful macro replacement will have the macro skipped

        Token * rep_start = macro->rep_start;
        Token * rep_end = macro->rep_end;
        Parser_copy_tokens(parser, &rep_start, &rep_end);
        Token_insert_before(start, rep_start, rep_end);
        
        if (MACRO_FUNCTION == macro->type) {
            // also have to remove remainder of macro function arguments

            // since we are actually removing tokens, make sure only going to skip the write number of characters
            // TODO: see above
        }
    }    

    return 0;
}
