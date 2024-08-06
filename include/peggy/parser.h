#ifndef PEGGY_PARSER_H
#define PEGGY_PARSER_H

/* C std lib includes */
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/* lib includes */
#include "logger.h"
#include "mempool.h"

/* peggy include */
#include <peggy/utils.h>
#include <peggy/token.h>
#include <peggy/rule.h>
#include <peggy/packrat_cache.h>

#define PARSER_DEFAULT_NTOKENS 256
#define PARSER_DEFAULT_NNODES 4096

/* Parser definitions and declarations */

#ifdef PACKRAT_HASH
#define ELEMENT_TYPE pASTNode
#include <peggy/stack.h>
#endif

#define Parser_tell(pparser) (((Parser *)pparser)->token_cur)
#define Parser_seek(pparser, ptoken) (((Parser *)pparser)->token_cur = ptoken)
#define Parser_fail_node(pparser) (((Parser *)pparser)->fail_node)
#define Parser_fail(pparser, prule) do { \
    if (Parser_tell(pparser) && (Parser_tell(pparser)->id >= Parser_fail_node(pparser)->token_start->id)) {\
        Parser_fail_node(pparser)->rule = prule;\
        Parser_fail_node(pparser)->token_start = Parser_tell(pparser);\
    } } while (0)
//#define Parser_fail(pparser, prule) Parser_fail_node(pparser)
#define Parser_is_fail_node(pparser, node) (node == Parser_fail_node(pparser))

struct Parser {
    struct ParserType * _class;
    PackratCache cache;
    Logger logger;
    MemPoolManager * node_mgr;
    MemPoolManager * token_mgr;
    Token * token_head;
    Token * token_cur;
    size_t ntokens;
    Token * token_tail;
    Rule * token_rule;
    Rule * root_rule;
    char const * name; /* usually file name */
    char const * log_file;
    size_t name_length;
    bool tokenizing;
    ASTNode * ast;
    ASTNode * fail_node;
    ASTNode * lookahead_node;
    unsigned int flags;
};

typedef enum ParserFlags {
    PARSER_NONE = 0,
    PARSER_LAZY = 1,
    PARSER_SPARSE_ALLOC = 2
} ParserFlags;

typedef struct ParserType ParserType;

extern struct ParserType {
    char const * type_name;
    err_type (*add_token)(Parser * parser, ASTNode * node);
    ASTNode * (*add_node)(Parser * self, Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode * child, size_t size);
    void (*parse)(Parser * parser, char const * string, size_t string_length);
    size_t (*tokenize)(Parser * self, char const * string, size_t string_length, Token ** start, Token ** end);
} Parser_class;

Parser * Parser_new(char const * name, size_t name_length, Rule * token_rule, 
                    Rule * root_rule, size_t nrules, 
                    unsigned int flags, char const * log_file, unsigned char log_level);
err_type Parser_init(Parser * parser, char const * name, size_t name_length,
                         Rule * token_rule, Rule * root_rule, size_t nrules, unsigned int flags, 
                         char const * log_file, unsigned char log_level);
void Parser_dest(Parser * parser);
void Parser_del(Parser * parser);
size_t Parser_tokenize(Parser * self, char const * string, size_t string_length, Token ** start, Token ** end);
void Parser_get_line_col_end(Parser * self, Token * tok, unsigned int * line_out, unsigned int * col_out);
void Parser_generate_new_token(Parser * self, size_t token_length, Token * cur);
size_t Parser_get_ntokens(Parser * self);
err_type Parser_skip_token(Parser * parser, ASTNode * node);
err_type Parser_add_token(Parser * parser, ASTNode * node);
ASTNode * Parser_add_node(Parser * self, Rule * rule, Token * start, Token * end, size_t str_length, size_t nchildren, ASTNode * child, size_t size);
void Parser_parse(Parser * parser, char const * string, size_t string_length);
ASTNode * Parser_check_cache(Parser * self, rule_id_type rule_id, Token * tok);
void Parser_cache_check(Parser * self, rule_id_type rule_id, Token * tok, ASTNode * node);
err_type Parser_traverse(Parser * parser, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt);
void Parser_print_tokens(Parser * self, FILE * stream);
size_t Parser_get_ntokens(Parser * self);
err_type Parser_print_ast(Parser * parser, FILE * stream);

// use in e.g. WHITESPACE or comments
ASTNode * skip_token(Production * production, Parser * parser, ASTNode * node);
ASTNode * token_action(Production * production, Parser * parser, ASTNode * node);

#endif // PEGGY_PARSER_H
