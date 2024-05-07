#ifndef PEGGY_PARSER_H
#define PEGGY_PARSER_H

/* C std lib includes */
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

/* lib includes */
#include <logger.h>

/* peggy include */
#include <peggy/utils.h>
//#include <peggy/type.h>
#include <peggy/token.h>
#include <peggy/rule.h>
#include <peggy/packrat_cache.h>

/**
 * Consider:
 *      changing the parser->tokens to a stack
 *
 */

typedef enum seek_origin {
    P_SEEK_SET,
    P_SEEK_CUR,
    P_SEEK_END,
} seek_origin;

//#define PARSER_DEFAULT_NTOKENS (4096 / sizeof(Token))
//#define PARSER_DEFAULT_NNODES (4096 / sizeof(ASTNode *))

#define PARSER_DEFAULT_NTOKENS 256
#define PARSER_DEFAULT_NNODES 256

/* Parser definitions and declarations */

#define Parser_DEFAULT_INIT {._class = &Parser_class, \
                             .token_rule = NULL, \
                             .root_rule = NULL, \
                             .name = "", \
                             .logger = DEFAULT_LOGGER_INIT, \
                             .log_file = NULL, \
                             .loc_ = 0, \
                             .disable_cache_check = false, \
                             .ast = NULL \
                             }

//typedef struct Parser Parser; // handled in <peggy/utils.h>

#define ELEMENT_TYPE pASTNode
#include <peggy/stack.h>

#define ELEMENT_TYPE Token
#include <peggy/stack.h>

struct Parser {
    struct ParserType * _class;
    PackratCache cache;
    Logger logger;
    STACK(pASTNode) node_list; /* fucking stupid...need an arena allocator in an AST object */
    STACK(Token) tokens; /* only going to have a simple array */
    Rule * token_rule;
    Rule * root_rule;
    char const * name; /* usually file name */
    char const * string;
    char const * log_file;
    size_t loc_; /* location in token stream */
    size_t name_length;
    size_t string_length;
    bool disable_cache_check;
    ASTNode * ast;
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
    Parser * (*new)(char const * name, size_t name_length, char const * string, size_t string_length, Rule * token_rule, 
                    Rule * root_rule, size_t nrules, unsigned int line_offset, 
                    unsigned int col_offset, unsigned int flags, 
                    char const * log_file, unsigned char log_level);
    err_type (*init)(Parser * parser, char const * name, size_t name_length, 
                         char const * string, size_t string_length, Rule * token_rule, 
                         Rule * root_rule, size_t nrules, unsigned int line_offset, 
                         unsigned int col_offset, unsigned int flags, 
                         char const * log_file, unsigned char log_level);
    void (*dest)(Parser * parser);
    void (*del)(Parser * parser);
    size_t (*tell)(Parser * parser);
    void (*seek)(Parser * parser, long long loc, seek_origin origin);
    void (*log)(Parser * parser, size_t loc, Rule * rule, ASTNode * result);
    void (*log_check_fail_)(Parser * parser, size_t loc, Rule * rule);
    void (*get_line_col_end)(Parser * parser, Token * tok, unsigned int * line_out, unsigned int * col_out);
    Token (*gen_final_token_)(Parser * parser, ASTNode * node);
    err_type (*skip_token)(Parser * parser, ASTNode * node);
    size_t (*estimate_final_ntokens_)(Parser * self);
    err_type (*add_token)(Parser * parser, ASTNode * node);
    ASTNode * (*add_node)(Parser * self, Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode ** children, size_t size);
    bool (*gen_next_token_)(Parser * parser);
    err_type (*get)(Parser * parser, size_t key, Token * tok);
    Token * (*get_tokens)(Parser * parser, ASTNode * node);
    void (*parse)(Parser * parser);
    ASTNode * (*check_cache)(Parser * self, rule_id_type rule_id, size_t token_key);
    void (*cache_check)(Parser * self, rule_id_type rule_id, size_t token_key, ASTNode * node);
    err_type (*traverse)(Parser * parser, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt);
    void (*print_ast)(Parser * parser, char * buffer, size_t buffer_size);
} Parser_class;

Parser * Parser_new(char const * name, size_t name_length, char const * string, size_t string_length, Rule * token_rule, 
                    Rule * root_rule, size_t nrules, unsigned int line_offset, unsigned intcol_offset, 
                    unsigned int flags, char const * log_file, unsigned char log_level);
err_type Parser_init(Parser * parser, char const * name, size_t name_length,
                         char const * string, size_t string_length, Rule * token_rule, 
                         Rule * root_rule, size_t nrules, unsigned int line_offset, 
                         unsigned int col_offset, unsigned int flags, 
                         char const * log_file, unsigned char log_level);
void Parser_dest(Parser * parser);
void Parser_del(Parser * parser);
size_t Parser_tell(Parser * parser);
void Parser_seek(Parser * parser, long long loc, seek_origin origin);
void Parser_log(Parser * parser, size_t loc, Rule * rule, ASTNode * result);
void Parser_log_check_fail_(Parser * parser, size_t loc, Rule * rule);
void Parser_get_line_col_end(Parser * parser, Token * tok, unsigned int * line_out, unsigned int * col_out);
Token Parser_gen_final_token_(Parser * parser, ASTNode * node);
err_type Parser_skip_token(Parser * parser, ASTNode * node);
size_t Parser_estimate_final_ntokens(Parser * self);
err_type Parser_add_token(Parser * parser, ASTNode * node);
ASTNode * Parser_add_node(Parser * self, Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode ** children, size_t size);
bool Parser_gen_next_token_(Parser * parser);
err_type Parser_get(Parser * parser, size_t key, Token * tok);
Token * Parser_get_tokens(Parser * parser, ASTNode * node);
void Parser_parse(Parser * parser);
ASTNode * Parser_check_cache(Parser * self, rule_id_type rule_id, size_t token_key);
void Parser_cache_check(Parser * self, rule_id_type rule_id, size_t token_key, ASTNode * node);
err_type Parser_traverse(Parser * parser, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt);
void Parser_print_ast(Parser * parser, char * buffer, size_t buffer_size);

// use in e.g. WHITESPACE or comments
ASTNode * skip_token(Production * production, Parser * parser, ASTNode * node);
ASTNode * token_action(Production * production, Parser * parser, ASTNode * node);

#endif // PEGGY_PARSER_H
