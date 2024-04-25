#ifndef PEGGY_PARSER_H
#define PEGGY_PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <peggy/utils.h>
#include <peggy/type.h>
#include <peggy/token.h>
#include <peggy/rule.h>

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

#define PARSER_DEFAULT_NTOKENS (4096 / sizeof(Token))
#define PARSER_DEFAULT_NNODES (4096 / sizeof(ASTNode *))

/* Parser definitions and declarations */

#define Parser_DEFAULT_INIT {._class = &Parser_class, \
                             .token_rule = NULL, \
                             .root_rule = NULL, \
                             .tokens = NULL, \
                             .node_list = NULL, \
                             .name = "", \
                             .log_buffer = NULL, \
                             .log_buffer_length = 0, \
                             .log_file = NULL, \
                             .loc_ = 0, \
                             .tokens_length = 0, \
                             .tokens_capacity = 0, \
                             .node_list_length = 0, \
                             .node_list_capacity = 0, \
                             .disable_cache_check = false, \
                             .ast = NULL \
                             }
#define ParserType_DEFAULT_INIT {._class = &Parser_TYPE,\
                                .new = &Parser_new,\
                                .init = &Parser_init,\
                                .dest = &Parser_dest, \
                                .del = &Parser_del,\
                                .tell = &Parser_tell, \
                                .seek = &Parser_seek, \
                                .log = &Parser_log, \
                                .log_check_fail_ = &Parser_log_check_fail_, \
                                .get_line_col_end = &Parser_get_line_col_end, \
                                .gen_final_token_ = &Parser_gen_final_token_, \
                                .skip_token = &Parser_skip_token, \
                                .add_token = &Parser_add_token, \
                                .add_node = &Parser_add_node, \
                                .gen_next_token_ = &Parser_gen_next_token_, \
                                .get = &Parser_get, \
                                .get_tokens = &Parser_get_tokens, \
                                .parse = &Parser_parse, \
                                .traverse = &Parser_traverse, \
                                .print_ast = &Parser_print_ast \
                                }

//typedef struct Parser Parser; // handled in <peggy/utils.h>

struct Parser {
    struct ParserType const * _class;
    Rule * token_rule;
    Rule * root_rule;
    Token ** tokens; /* only going to have a simple array */
    Token ** tokens_gc;
    ASTNode ** node_list; /* fucking stupid...need an arena allocator in an AST object */
    char const * name; /* usually file name */
    char * log_buffer; 
    size_t log_buffer_length; 
    char const * log_file;
    size_t loc_; /* location in token stream */
    size_t name_length;
    size_t tokens_length;
    size_t tokens_capacity;
    size_t tokens_gc_length;
    size_t tokens_gc_capacity;
    size_t node_list_length;
    size_t node_list_capacity;
    bool disable_cache_check;
    ASTNode * ast;
};

extern struct ParserType {
    Type const * _class;
    Parser * (*new)(char const * name, size_t name_length, char const * string, size_t string_length, Rule * token_rule, 
                    Rule * root_rule, unsigned int line_offset, 
                    unsigned int col_offset, bool lazy_parse, 
                    char const * log_file);
    err_type (*init)(Parser * parser, char const * name, size_t name_length, 
                         char const * string, size_t string_length, Rule * token_rule, 
                         Rule * root_rule, unsigned int line_offset, 
                         unsigned int col_offset, bool lazy_parse, 
                         char const * log_file);
    void (*dest)(Parser * parser);
    void (*del)(Parser * parser);
    size_t (*tell)(Parser * parser);
    void (*seek)(Parser * parser, long long loc, seek_origin origin);
    void (*log)(Parser * parser, size_t loc, Rule * rule, ASTNode * result);
    void (*log_check_fail_)(Parser * parser, size_t loc, Rule * rule);
    void (*get_line_col_end)(Parser * parser, Token * tok, unsigned int * line_out, unsigned int * col_out);
    Token * (*gen_final_token_)(Parser * parser, ASTNode * node);
    err_type (*skip_token)(Parser * parser, ASTNode * node);
    err_type (*add_token)(Parser * parser, ASTNode * node);
    err_type (*add_node)(Parser * parser, ASTNode * node);
    bool (*gen_next_token_)(Parser * parser);
    err_type (*get)(Parser * parser, size_t key, Token ** tok);
    Token ** (*get_tokens)(Parser * parser, ASTNode * node, size_t * ntokens);
    void (*parse)(Parser * parser);
    err_type (*traverse)(Parser * parser, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt);
    void (*print_ast)(Parser * parser, char * buffer, size_t buffer_size);
} Parser_class;

extern Type const Parser_TYPE;

Parser * Parser_new(char const * name, size_t name_length, char const * string, size_t string_length, Rule * token_rule, 
                    Rule * root_rule, unsigned int line_offset, unsigned intcol_offset, 
                    bool lazy_parse, char const * log_file);
err_type Parser_init(Parser * parser, char const * name, size_t name_length,
                         char const * string, size_t string_length, Rule * token_rule, 
                         Rule * root_rule, unsigned int line_offset, 
                         unsigned int col_offset, bool lazy_parse, 
                         char const * log_file);
void Parser_dest(Parser * parser);
void Parser_del(Parser * parser);
size_t Parser_tell(Parser * parser);
void Parser_seek(Parser * parser, long long loc, seek_origin origin);
void Parser_log(Parser * parser, size_t loc, Rule * rule, ASTNode * result);
void Parser_log_check_fail_(Parser * parser, size_t loc, Rule * rule);
void Parser_get_line_col_end(Parser * parser, Token * tok, unsigned int * line_out, unsigned int * col_out);
Token * Parser_gen_final_token_(Parser * parser, ASTNode * node);
err_type Parser_skip_token(Parser * parser, ASTNode * node);
err_type Parser_add_token(Parser * parser, ASTNode * node);
err_type Parser_add_node(Parser * parser, ASTNode * node);
bool Parser_gen_next_token_(Parser * parser);
err_type Parser_get(Parser * parser, size_t key, Token ** tok);
Token ** Parser_get_tokens(Parser * parser, ASTNode * node, size_t * ntokens);
void Parser_parse(Parser * parser);
err_type Parser_traverse(Parser * parser, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt);
void Parser_print_ast(Parser * parser, char * buffer, size_t buffer_size);

// use in e.g. WHITESPACE or comments
ASTNode * skip_token(Parser * parser, ASTNode * node);
ASTNode * token_action(Parser * parser, ASTNode * node);

#endif // PEGGY_PARSER_H
