#include "c.h"
#include "cpp.h"
#include "cstring.h"

#define DEFAULT_MACRO_CAPACITY 7

#define MACRO 0
#define MACRO_FUNCTION 1
#define EXPECTS_VA_ARGS 1

typedef struct Macro {
    Token * id;
    Token * rep_start; // start of replacement tokens
    Token * rep_end; // end of replacement tokens (inclusive)
    // not going to distinguish between macros and macro functions
    unsigned short nparam;
    unsigned char type;
    unsigned char va_arg;
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

typedef struct TokenPair {
    Token * start;
    Token * end;
} TokenPair;

BUILD_ALIGNMENT_STRUCT(TokenPair)

// for creating mapping of identifier tokens to token stream of replacements
#define KEY_TYPE pToken
#define VALUE_TYPE TokenPair
#define KEY_COMP Token_scomp
#define HASH_FUNC Token_shash
#include "peg4c/hash_map.h"

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
        .id = define->children[2]->token_start
    };

    if (define->nchildren > 5) {
        macro->type = MACRO_FUNCTION;
        ASTNode * id_list = NULL;
        if (C_ELLIPSIS == define->children[4]->rule) {
            macro->va_arg = EXPECTS_VA_ARGS;
        } else if (C_ELLIPSIS == define->children[5]->rule) {
            macro->va_arg = EXPECTS_VA_ARGS;
            id_list = define->children[4];
        } else if (define->children[4]->nchildren) { // no va_args
            id_list = define->children[4]->children[0];
        }
        if (id_list) {
            macro->nparam = (id_list->nchildren + 1) >> 1;
        }
    } else {
        macro->type = MACRO;
    }

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
    bool tokenizing_ref = parser->tokenizing;
    parser->tokenizing = false;

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

    parser->tokenizing = tokenizing_ref;

    return 0;
}

void CPP_init_macro_arg_map(HASH_MAP(pToken, TokenPair) * arg_map, Macro * macro, Token * start, Token * end) {
    // start should initially be pointing at the '(' start of args list
    start = start->next;
    Token * id = macro->id->next->next; // point to the first parameter
    unsigned char nparams = 0;
    while (nparams < macro->nparam && start != end) {
        start->prev = NULL;
        Token * cur = start->next;
        while (cur != end && ',' != cur->string[0]) { // end should be equivalent to ')'
            cur = cur->next;
        }
        cur->prev->next = NULL;
        // add to argument map. cur current points to either ',' or ')'
        arg_map->_class->set(arg_map, id, (TokenPair){.start = start, .end = cur->prev});
        // advance start of next argument
        if (cur != end) {
            start = cur->next;
        } else {
            start = cur;
        }
        
        // advance to next parameter
        id = id->next->next;
        nparams++;
    }
    if (start != end) {
        fprintf(stderr, "CPP_init_macro_arg_map: error in number of args and number of parameters for macro %.*s. # of parameters: %c\n", 
            (int)macro->id->length, macro->id->string, nparams);
    }
}

void CPP_expand_macro_function(Parser * parser, HASH_MAP(pToken, TokenPair) * arg_map, Macro * macro, Token ** start_, Token ** end_) {
    Token * start = *start_, * end = (*end_)->next;
    Token * head = &(Token){0};
    Token * tail = head;

    while (start != end) {
        TokenPair rep;
        if (arg_map->_class->get(arg_map, start, &rep)) {
            Token_append(tail, Parser_copy_token(parser, start));
            tail = tail->next;
        } else {
            Token * s = rep.start, * e = rep.end;
            Parser_copy_tokens(parser, &s, &e);
            Token_append(tail, s);
            tail = e;
        }
        start = start->next;
    }
    *start_ = head->next;
    *end_ = tail;
}

// TODO: add compatibility with macro functions
// state of parser must be tokenizing  when entering CPP_check
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
    if (macro->rep_start && macro->rep_end) {
        Token * rep_start = macro->rep_start;
        Token * rep_end = macro->rep_end;
        
        // insert before because a successful macro replacement will have the macro skipped
        if (MACRO_FUNCTION == macro->type) {
            size_t length = 0;
            char const * c = start->string + id_re->str_length;
            int nopen = 0;
            while (*c != '(') {
                c++;
                length++;
            }
            c++;
            length++;
            nopen++;

            while (nopen) {
                switch (*c) {
                    case '(': {
                        nopen++;
                        break;
                    }
                    case ')': {
                        nopen--;
                        break;
                    }
                }
                c++;
                length++;
            }
            
            unsigned short nparam = macro->nparam;
            if (nparam) {
                Token * par_start = NULL;
                Token * par_end = NULL;
                // no need to set parser->tokenizing since that is a requirement to entering CPP_check
                int status = parser->_class->tokenize(parser, start->string + id_re->str_length, length, &par_start, &par_end);
                
                // create map
                HASH_MAP(pToken, TokenPair) arg_map;
                HASH_MAP_INIT(pToken, TokenPair)(&arg_map, next_prime(nparam));

                CPP_init_macro_arg_map(&arg_map, macro, par_start, par_end);

                CPP_expand_macro_function(parser, &arg_map, macro, &rep_start, &rep_end);

                // cleanup
                arg_map._class->dest(&arg_map);
            } else {
                Parser_copy_tokens(parser, &rep_start, &rep_end);
            }
            id_re->str_length += length + 1; // so that we skip the whole macro function invocation
        } else {
            Parser_copy_tokens(parser, &rep_start, &rep_end); // non function macros simply copy the replacement list
        }
        Token_insert_before(start, rep_start, rep_end); 
    }    

    return 0;
}
