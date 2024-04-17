#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <peggy/parser.h>

#define Parser_NAME "Parser"

Type const Parser_TYPE = {._class = &Type_class,
                        .type_name = Parser_NAME};

struct ParserType Parser_class = ParserType_DEFAULT_INIT;

Parser * Parser_new(char const * name, char const * string, size_t string_length, Rule * token_rule, 
                    Rule * root_rule, unsigned int line_offset, 
                    unsigned int col_offset, bool lazy_parse, 
                    char const * log_file) {
    Parser * parser = malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    if (Parser_init(parser, name, string, string_length, token_rule, root_rule, line_offset, 
                    col_offset, lazy_parse, log_file)) {
        free(parser);
        return NULL;
    }
    return parser;
}
err_type Parser_init(Parser * self, char const * name, 
                         char const * string, size_t string_length, Rule * token_rule, 
                         Rule * root_rule, unsigned int line_offset, 
                         unsigned int col_offset, bool lazy_parse, 
                         char const * log_file) {
    *self = (Parser) Parser_DEFAULT_INIT;
    self->name = name;
    self->token_rule = token_rule;
    self->root_rule = root_rule;
    self->loc_ = 0;
    self->tokens = malloc(sizeof(Token) * PARSER_DEFAULT_NTOKENS);
    if (!self->tokens) {
        return PEGGY_MALLOC_FAILURE;
    }
    if (Token_init(self->tokens, string, 0, string_length, line_offset, col_offset)) {
        free(self->tokens);
        return PEGGY_FAILURE;
    }
    self->tokens_length = 1;
    self->log_file = log_file;
    /* TODO: log_buffer needed if log_file is not NULL */
    return PEGGY_SUCCESS;
}
void Parser_dest(Parser * self) {
    /* clear the rule caches. TODO: clear out all the nodes as well */
    if (self->tokens_length) {
        self->root_rule->_class->clear_cache(self->root_rule, self->tokens);
        self->token_rule->_class->clear_cache(self->token_rule, self->tokens);
    }

    /* clear the token list */
    if (self->tokens_capacity) {
        free(self->tokens);
        self->tokens_length = 0;
        self->tokens_capacity = 0;
    }
    
    if (self->log_buffer) {
        free(self->log_buffer);
        self->log_buffer_length = 0;
    }
    
}
void Parser_del(Parser * self) {
    Parser_dest(self);
    free(self);
}
size_t Parser_tell(Parser * self) {
    return self->loc_;
}
void Parser_seek(Parser * self, long long loc, seek_origin origin) {
    if (origin == P_SEEK_SET) {
        self->loc_ = loc;
        return;
    }
    size_t N = self->tokens[self->tokens_length - 1].end;
    if (origin == P_SEEK_CUR) {
        if (loc < 0 && self->loc_ < (size_t)-loc) {
            self->loc_ = 0;
        } else {
            self->loc_ += loc; /* doesn't prevent overflow */
        }
        
    } else { // P_SEEK_END
        self->loc_ = N + loc;
    }
    if (self->loc_ > N) {
        self->loc_ = N;
    }
}
void Parser_log(Parser * self, size_t loc, Rule * rule, ASTNode * result);
void Parser_log_check_fail_(Parser * self, size_t loc, Rule * rule);
void Parser_get_line_col_end(Parser * self, Token * tok, unsigned int * line_out, unsigned int * col_out) {
    char const * start = tok->string + tok->start;
    char const * end = tok->string + tok->end;
    unsigned int line = tok->coords.line;
    char const * chr = strchr(start, '\n');
    while (chr) {
        line++;
        start = chr + 1;
        chr = strchr(start, '\n');
    }
    unsigned int col = (unsigned int) (end - start);
    if (line == tok->coords.line) { /* no change */
        col += tok->coords.col;
    }
    *col_out = col;
    *line_out = line;
}
void Parser_gen_final_token_(Parser * self, ASTNode * node, Token * tok) {
    /* TODO: when "longest_rule" is implemented, clear it */
    Token * final = self->tokens + self->tokens_length - 1;
    size_t end = final->end;
    final->end = final->start + node->str_length;
    size_t start = final->end;
    unsigned int line;
    unsigned int col;

    /* initialize tok */
    self->_class->get_line_col_end(self, final, &line, &col); // set token coordinates
    /* override const in initializing "tok" */
    Token_class.init(tok, final->string, start, end, line, col);
}
void Parser_skip_token(Parser * self, ASTNode * node) {
    Token new_final;
    self->_class->gen_final_token_(self, node, &new_final);
    /* overwrite final token */
    Token * final = self->tokens + self->tokens_length - 1;
    memcpy(final, &new_final, sizeof(Token));
}
err_type Parser_extend_tokens_(Parser * self) {
    /* how far have we progressed already? */
    Token * final = self->tokens + self->tokens_length - 1;
    /* set a default expansion */
    size_t new_capacity = (self->tokens_capacity == 0) ? PARSER_DEFAULT_NTOKENS :  (self->tokens_capacity * 2);
    /* estimate how many more tokens I need to allocate */
    if (final->start > 0) {
        new_capacity = 1 + (size_t) (((1.0 * final->end) / final->start) * self->tokens_length);
    }
    Token * new_tokens = realloc(self->tokens, sizeof(Token) * new_capacity);
    if (!new_tokens) {
        return PEGGY_MALLOC_FAILURE;
    }
    self->tokens = new_tokens;
    self->tokens_capacity = new_capacity;
    return PEGGY_SUCCESS;
}
err_type Parser_add_token(Parser * self, ASTNode * node) {
    Token new_final;
    self->_class->gen_final_token_(self, node, &new_final);
    /* extend self->tokens if necessary */
    if (self->tokens_length == self->tokens_capacity) {
        err_type status = Parser_extend_tokens_(self);
        if (status) {
            return status;
        }
    }
    /* "append" new_final */
    memcpy(self->tokens + self->tokens_length, &new_final, sizeof(Token));
    self->tokens_length++;
    return PEGGY_SUCCESS;
}
bool Parser_gen_next_token_(Parser * self) {
    self->disable_cache_check = true;
    ASTNode * result = self->token_rule->_class->check(self->token_rule, self, self->disable_cache_check);
    Token * final = self->tokens + self->tokens_length - 1;
    if (final->_class->len(final)) {
        self->disable_cache_check = false;
    }
    return result != &ASTNode_fail;
}
err_type Parser_get(Parser * self, size_t key, Token ** tok) {
    if (!self->disable_cache_check) {
        while (key < self->tokens_length - 1) {
            Token * final = self->tokens + self->tokens_length - 1;
            if (final->_class->len(final) == 0 || !self->_class->gen_next_token_(self)) {
                break;
            }
        }
    }
    if (key < self->tokens_length) {
        *tok = self->tokens + key;
        return PEGGY_SUCCESS;
    }
    return PEGGY_INDEX_OUT_OF_BOUNDS;
}
Token * Parser_get_tokens(Parser * self, ASTNode * node, size_t * ntokens) {
    *ntokens = node->ntokens;
    return self->tokens + node->token_key;
}
void Parser_parse(Parser * self) {
    self->ast = self->root_rule->_class->check(self->root_rule, self, false);
    if (self->ast == &ASTNode_fail || self->ast->ntokens < self->tokens_length - 1) {
        printf("parsing failed\n"); /* TODO: include longest_rule information when available */
    } else {
        /* TODO: for auto_traverse */
    }
    /* TODO: logging */
}
err_type Parser_traverse(Parser * self, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt) {
    /* TODO */
    /* requires a stack implementation */
    return PEGGY_NOT_IMPLEMENTED;
}
void Parser_print_ast(Parser * self, char * buffer, size_t buffer_size) {
    /* requires a deque implementation */
}

// use in e.g. WHITESPACE or comments
ASTNode * skip_token(Parser * parser, ASTNode * node) {
    return make_skip_node(node);
}

ASTNode * token_action(Parser * parser, ASTNode * node) {
    if (node != &ASTNode_fail) {
        if (is_skip_node(node)) {
            parser->_class->skip_token(parser, node);
        } else {
            parser->_class->add_token(parser, node);
        }
        // need to back up one token in order to ensure not erroneously moving the parser location forward
        parser->_class->seek(parser, -1, P_SEEK_CUR);
    }
    return node;
}