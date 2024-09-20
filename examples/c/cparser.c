#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "peggy/astnode.h"
#include "peggy/parser.h"
#include "peggy/rule.h"

#include "cparser.h"
#include "c.h"

#define BUFFER_SIZE 4096
#define BUFFER_SIZE_SCALE 2

#ifndef DEFAULT_LOG_LEVEL
	#define DEFAULT_LOG_LEVEL LOG_LEVEL_WARN
#endif

typedef struct CString {
    char const * str;
    size_t len;
} CString;

// add builtin types here so that the parsers work. The first two below are needed to parse the stand library with gcc and clang
CString BUILTIN_TYPES[] = {
    {.str = "__builtin_va_list", .len = 17},    // gcc and clang
    {.str = "_Float32", .len = 8},              // gcc
    {.str = "_Float32x", .len = 9},              // gcc
    {.str = "_Float64", .len = 8},              // gcc
    {.str = "_Float64x", .len = 9},              // gcc
    {.str = "_Float128", .len = 9},             // gcc
    {.str = NULL, .len = 0}                     // sentinel that must be last
};

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
    self->scope = Scope_new(NULL); // a file-level scope
    Parser_init((Parser *)self, crules[C_TOKEN], crules[C_C], C_NRULES, 0);
    Parser_set_log_file((Parser *)self, "cparser.log", DEFAULT_LOG_LEVEL);

    // add built-in types to file scope
    ASTNode * builtin_type_node = Parser_add_node((Parser *)self, crules[C_TOKEN], ((Parser *)self)->token_head, NULL, 1, 0, 0);
    size_t i = 0;
    while (BUILTIN_TYPES[i].len) {
        Scope_add_typedef(self->scope, BUILTIN_TYPES[i], builtin_type_node);
        i++;
    }
    //printf("logger set to %d (tried %d)\n", self->parser.logger.max_logging_level, LOG_LEVEL_WARN);
}
void CParser_dest(CParser * self) {
    Parser_dest(&self->parser);
    while (self->scope) {
        self->scope = Scope_dest(self->scope);
    }
}

ASTNode * nc1_pass0(Production * rule, Parser * parser, ASTNode * node) {
    if (node->nchildren == 1) { // there is no binary operator. reduce to left-hand operand type
        LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - simplify rule id %d to rule id %d from line %u, col %u\n", __func__, node->rule->id, node->children[0]->rule->id, node->token_start->coords.line, node->token_start->coords.col);
        return node->children[0];
    }
    return build_action_default(rule, parser, node);
}

ASTNode * c0nc0_pass1(Production * rule, Parser * parser, ASTNode * node) {
    if (!node->children[0]->nchildren) {
        LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - simplify rule id %d to rule id %d from line %u, col %u\n", __func__, node->rule->id, node->children[0]->rule->id, node->token_start->coords.line, node->token_start->coords.col);
        return node->children[1];
    }
    return build_action_default(rule, parser, node);
}

// identical to nc1_pass0 but want a little more clarity
ASTNode * simplify_binary_op(Production * binary_op, Parser * parser, ASTNode * node) {
    if (node->nchildren == 1) { // there is no binary operator. reduce to left-hand operand type
        LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - simplify rule id %d to rule id %d from line %u, col %u\n", __func__, node->rule->id, node->children[0]->rule->id, node->token_start->coords.line, node->token_start->coords.col);
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
    ASTNode * possible_id = declrtr->children[1]->children[0]->children[0];
    if (possible_id->rule->id == C_IDENTIFIER) {
        return (CString) {.str = possible_id->token_start->string, .len = possible_id->str_length};
    }
    return get_declarator_identifier(declrtr->children[1]->children[0]->children[1]);
}

// checks and eliminates ambiguities from typedef_name
ASTNode * c_process_declaration_specifiers(Production * decl_specs, Parser * parser, ASTNode * node) {
    _Bool has_type_spec = false;

    assert((node->children[0]->rule->id == C_DECLARATION_SPECIFIER) || !printf("%.*s is not a declaration specifier\n", (int)node->children[0]->token_start->length, node->children[0]->token_start->string));
    size_t nchildren = 0;
    for (size_t i = 0; i < node->nchildren; i++) {
        ASTNode * decl_spec = node->children[i];
        if (decl_spec->children[0]->rule->id == C_TYPE_SPECIFIER) {
            ASTNode * type_spec = decl_spec->children[0];
            if (type_spec->children[0]->rule->id == C_TYPEDEF_NAME && has_type_spec) {
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
    if (node->children[0]->rule->id == C_STATIC_ASSERT_DECLARATION || node->children[0]->children[0]->rule->id == C_ATTRIBUTE_SPECIFIER) {
        return build_action_default(decl, parser, node);
    }
    ASTNode * decl_specs = node->children[0]->children[0];
    ASTNode * init_declarators = node->children[0]->children[1]->nchildren ? node->children[0]->children[1]->children[0] : NULL;
    // check declaration specifiers for typedef_kw
    if (decl_specs->rule->id != C_DECLARATION_SPECIFIERS) {
        decl_specs = node->children[0]->children[1];
        init_declarators = node->children[0]->children[2];
    }
    assert((decl_specs->rule->id == C_DECLARATION_SPECIFIERS) || !printf("c_process_declaration failed to find the declaration_specifiers: %s\n", decl_specs->rule->name));
    assert((init_declarators == NULL) || (init_declarators->children[0]->rule->id == C_INIT_DECLARATOR));
    for (size_t i = 0; i < decl_specs->nchildren; i++) {
        ASTNode * child = decl_specs->children[i];
        ASTNode * gchild = child->children[0];
        if (gchild->rule->id == C_STORAGE_CLASS_SPECIFIER && gchild->children[0]->rule->id == C_TYPEDEF_KW) {
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

/* // this will be used when a preprocessor is added
ASTNode * b_to_end_of_line(Production * rule, Parser * parser, ASTNode * node) {
    /// *
    node at this point represents a single token that is on a given line. consume the tokens until a new line is found.
    Since tokens already track line coordinates, consume tokens until a new line is identified or token_tail is found
    This is a very hacky way to do it that I don't otherwise recommend, but it is very difficult to track preprocessor
    lines during lexing stage or diagnostic/pragma lines during parsing a preprocessed file without it
    // * /
    Token * tok = node->token_start;
    unsigned int start_line = tok->coords.line;
    size_t str_length = node->str_length;
    tok = tok->next;
    while (tok && tok != parser->token_tail && tok->coords.line == start_line) {
        str_length += tok->length;
        tok = tok->next;
    }
    node->str_length = str_length;
    node->token_end = tok->prev;
    Parser_seek(parser, tok);
    node->rule = (Rule *)rule;
    return node;
}
*/

int main(int narg, char ** args) {
    char * string = NULL;
    size_t nbytes = 0;
    if (narg == 2) {
        //printf("arg %d: %s\n", i, args[i]);
        FILE * input_file = fopen(args[1], "rb");
        if (fseek(input_file, 0, SEEK_END)) {
            printf("failed to seek input file for input file <%s>\n", args[1]);
            return 1;
        }
        long file_size = 0;
        if ((file_size = ftell(input_file)) < 0) {
            printf("failed to get file size for input file <%s>\n", args[1]);
            return 2;
        }
        fseek(input_file, 0, SEEK_SET);
        string = malloc(file_size * sizeof(*string));
        if (!string) {
            printf("malloc failed for input file: <%s>\n", args[1]);
            return 3;
        }
        nbytes = fread(string, sizeof(*string), file_size, input_file);
    } else {
        int c = getc(stdin);
        if (c != EOF) {
            ungetc(c, stdin);
            size_t nbuffered = 0;
            while ((c = getc(stdin)) != EOF) {
                ungetc(c, stdin);
                char * new_string = realloc(string, nbuffered + BUFFER_SIZE);
                if (!new_string) {
                    free(string);
                    return 4;
                }
                string = new_string;
                nbuffered += BUFFER_SIZE;
                nbytes += fread(string + nbytes, sizeof(*string), nbuffered - nbytes, stdin);
            }
            printf("string from stdin (length %zu): %.*s\n", nbytes, (int)nbytes, string);
        } else {
            printf("parse string not found found in file or stdin\n");
            return 5;
        }
    }
    if (string) {
        CParser parser;
        CParser_init(&parser);
        Parser_parse((Parser *)&parser, string, nbytes);
        Parser_print_tokens((Parser *)&parser, stdout);
        if (!parser.parser.ast || Parser_is_fail_node((Parser *)&parser, parser.parser.ast) || parser.parser.token_cur->length) {
            err_type status = Parser_print_parse_status((Parser *)&parser, stdout);
            if (status) {
                LOG_EVENT(&((Parser *)&parser)->logger, LOG_LEVEL_ERROR, "ERROR: %s - %s failed for input file: <%s>, at token: %.*s\n", __func__, status == PEGGY_PARSE_FAILURE ? "parser" : "lexer", args[1], (int)parser.parser.token_cur->length, parser.parser.token_cur->string);
            }
        }
        Parser_print_ast((Parser *)&parser, stdout);
        CParser_dest(&parser);
        free(string);
    }
    return 0;
}

