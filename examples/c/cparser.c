#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <peggy/astnode.h>
#include <peggy/parser.h>
#include <peggy/rule.h>

#include "cparser.h"
#include "c.h"

typedef struct CString {
    char const * str;
    size_t len;
} CString;

int CString_comp(CString a, CString b) {
    if (a.len < b.len) {
        return 1;
    } else if (a.len > b.len) {
        return -1;
    }
    return strncmp(a.str, b.str, a.len);
}

size_t CString_hash(CString a, size_t bin_size) {
    unsigned long long hash = 5381;
    size_t i = 0;
    unsigned char * str = (unsigned char *) a.str;

    while (i < a.len) {
        hash = ((hash << 5) + hash) + *str; /* hash * 33 + c */
        str++;
        i++;
    }
    return hash % bin_size;
}

#define KEY_TYPE CString
#define VALUE_TYPE pASTNode
#define KEY_COMP CString_comp
#define HASH_FUNC CString_hash
#include <peggy/hash_map.h>

/* Scope definitions and handlers */
typedef struct Scope Scope;
struct Scope {
    Scope * parent;
    HASH_MAP(CString, pASTNode) typedefs; 
};
Scope * Scope_new(Scope * parent) {
    Scope * new = malloc(sizeof(*new));
    if (!new) {
        return NULL;
    }
    new->parent = parent;
    HASH_MAP_INIT(CString, pASTNode)(&new->typedefs, 3);
    return new;
}
void Scope_add_typedef(Scope * self, CString typedef_, ASTNode * node) {
    self->typedefs._class->set(&self->typedefs, typedef_, node);
}
bool Scope_is_typedef(Scope * self, CString id) {
    if (!self) {
        return false;
    }
    ASTNode * node;
    if (!self->typedefs._class->get(&self->typedefs, id, &node)) {
        return true;
    }
    return Scope_is_typedef(self->parent, id);
}
Scope * Scope_dest(Scope * self) {
    if (!self) {
        return NULL;
    }
    Scope * parent = self->parent;
    self->typedefs._class->dest(&self->typedefs);
    free(self);
    return parent;
}

/* Thin wrapper around the default parser that adds scope context for parsing/disambiguating typedefs from other identifiers */
typedef struct CParser {
    Parser parser;
    Scope * scope;
} CParser;

void CParser_init(CParser * self) {
    self->scope = Scope_new(NULL);
    Parser_init((Parser *)self, NULL, 0, (Rule *)&c_token, (Rule *)&c_c, C_NRULES, 0, "cparser.log", LOG_LEVEL_TRACE);
}
void CParser_dest(CParser * self) {
    Parser_dest(&self->parser);
    while (self->scope) {
        self->scope = Scope_dest(self->scope);
    }
}

ASTNode * simplify_binary_op(Production * binary_op, Parser * parser, ASTNode * node) {
    if (!node->children[1]->nchildren) { // there is no binary operator. reduce to left-hand operand type
        LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - simplify rule id %d to rule id %d from line %u, col %u\n", __func__, node->rule->id, node->children[0]->rule->id, node->token_start->coords.line, node->token_start->coords.col);
        return node->children[0];
    }
    return build_action_default(binary_op, parser, node);
}

// open and close scopes in compound statement
ASTNode * _open_scope(Production * rule, Parser * parser, ASTNode * node) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - opening a new block scope at line: %u, col: %u\n", __func__, node->token_start->coords.line, node->token_start->coords.col);
    CParser * self = (CParser *)parser;
    self->scope = Scope_new(self->scope);
    return node; // return node because it's just a punctuator that is unused
}
ASTNode * _close_scope(Production * rule, Parser * parser, ASTNode * node) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - closing a block scope at line: %u, col: %u\n", __func__, node->token_start->coords.line, node->token_start->coords.col);
    CParser * self = (CParser *)parser;
    self->scope = Scope_dest(self->scope);
    return node; // return node because it's just a punctuator that is unused
}

// extracts the identifier from the declarator
CString get_declarator_identifier(ASTNode * declrtr) {
    //ASTNode * possible_id = declrtr->child->next->child->child->next;
    ASTNode * possible_id = declrtr->children[1]->children[0]->children[1];
    if (possible_id->rule->id == IDENTIFIER) {
        return (CString) {.str = possible_id->token_start->string, .len = possible_id->str_length};
    }
    return get_declarator_identifier(possible_id);
}

// checks and eliminates ambiguities from typedef_name
ASTNode * c_process_declaration_specifiers(Production * decl_specs, Parser * parser, ASTNode * node) {
    _Bool has_type_spec = false;

    assert(node->children[0]->rule->id == DECLARATION_SPECIFIER | !printf("%s, %zu is not a declaration specifier\n"));
    size_t nchildren = 0;
    for (size_t i = 0; i < node->nchildren; i++) {
        ASTNode * decl_spec = node->children[i];
        if (decl_spec->children[0]->rule->id == TYPE_SPECIFIER) {
            ASTNode * type_spec = decl_spec->children[0];
            if (type_spec->children[0]->rule->id == TYPEDEF_NAME && has_type_spec) {
                // strip declaration specifiers at this point
                ASTNode * decl_specs_end = node->children[i - 1];
                LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - removing typedef name at line: %u, col: %u\n", __func__, decl_spec->token_start->coords.line, decl_spec->token_start->coords.col);            
                node->nchildren = nchildren;
                node->token_end = decl_specs_end->token_end;
                node->str_length = (size_t)(decl_specs_end->token_end->string - node->token_start->string) + decl_specs_end->token_end->length;
                return build_action_default(decl_specs, parser, node);            
            }
            has_type_spec = true;
        }
        nchildren++;
    }
    return build_action_default(decl_specs, parser, node);
}

// takes a declaration and if it detects a typedef, registers it in the current scope
ASTNode * c_process_declaration(Production * decl, Parser * parser, ASTNode * node) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - checking for typedef in %.*s\n", __func__, (int)node->str_length, node->token_start->string);
    ASTNode * decl_specs = node->children[0]->children[0];
    ASTNode * init_declarators = node->children[0]->children[1]->nchildren ? node->children[0]->children[1]->children[0] : NULL;
    if (!node->children[0]->children[1]) {
        return build_action_default(decl, parser, node);
    }
    // check declaration specifiers for typedef_kw
    if (decl_specs->rule->id != DECLARATION_SPECIFIERS) {
        decl_specs = node->children[0]->children[1];
        init_declarators = node->children[0]->children[2];
    }
    assert(decl_specs->rule->id == DECLARATION_SPECIFIERS || !printf("c_process_declaration failed to find the declaration_specifiers"));
    assert(init_declarators == NULL || init_declarators->rule->id == INIT_DECLARATOR);
    for (size_t i = 0; i < decl_specs->nchildren; i++) {
        ASTNode * child = decl_specs->children[i];
        ASTNode * gchild = child->children[0];
        if (gchild->rule->id == STORAGE_CLASS_SPECIFIER && gchild->children[0]->rule->id == TYPEDEF_KW) {
            for (size_t i = 0; i < init_declarators->nchildren; i += 2) {
                ASTNode * init_declarator = init_declarators->children[i];
                CString new_typedef = get_declarator_identifier(init_declarator->children[0]);
                LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - found new typedef: %.*s\n", __func__, (int)new_typedef.len, new_typedef.str);
                Scope_add_typedef(((CParser *)parser)->scope, new_typedef, node);
            }
            return build_action_default(decl, parser, node);
        }
    }
    return build_action_default(decl, parser, node);
}

// takes a typedef_name type_specifier and if the identifier is registered as a typedef in any of the open scopes, returns the node as-is else fails
ASTNode * c_check_typedef(Production * decl_specs, Parser * parser, ASTNode * node) {
    ASTNode * identifier = node->children[0];
    if (Scope_is_typedef(((CParser *)parser)->scope, (CString) {.str = identifier->token_start->string, .len = identifier->str_length})) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - typedef accepted %.*s\n", __func__, (int)identifier->str_length, identifier->token_start->string);
        return build_action_default(decl_specs, parser, node); // ignore the negative lookahead from now on
    }
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - typedef of length %zu rejected %.*s\n", __func__, identifier->str_length, (int)identifier->str_length, identifier->token_start->string);
    return Parser_fail_node(parser);
}

int main(int narg, char ** args) {
    CParser parser;
    CParser_init(&parser);
    for (int i = 1; i < narg; i++) {
        //printf("arg %d: %s\n", i, args[i]);
        FILE * input_file = fopen(args[i], "rb");
        if (fseek(input_file, 0, SEEK_END)) {
            printf("failed to seek input file for input file <%s>\n", args[i]);
            continue;
        }
        long file_size = 0;
        if ((file_size = ftell(input_file)) < 0) {
            printf("failed to get file size for input file <%s>\n", args[i]);
            continue;
        }
        fseek(input_file, 0, SEEK_SET);
        char * string = malloc(file_size * sizeof(*string));
        if (!string) {
            printf("malloc failed for input file: <%s>\n", args[i]);
            continue;
        }
        size_t nbytes = fread(string, sizeof(*string), file_size, input_file);
        parser.parser.name = args[i];
        parser.parser.name_length = strlen(args[i]);
        Parser_parse((Parser *)&parser, string, nbytes);
        Parser_print_tokens((Parser *)&parser, stdout);
        if (Parser_is_fail_node((Parser *)&parser, parser.parser.ast)) {
            printf("parser failed for input file: <%s>\n", args[i]);
            continue;
        }
        Parser_print_ast((Parser *)&parser, stdout);
    }
    CParser_dest(&parser);
    return 0;
}

