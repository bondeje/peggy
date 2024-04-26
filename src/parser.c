#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <peggy/parser.h>

#define Parser_NAME "Parser"

Type const Parser_TYPE = {._class = &Type_class,
                        .type_name = Parser_NAME};

struct ParserType Parser_class = ParserType_DEFAULT_INIT;

Parser * Parser_new(char const * name, size_t name_length, char const * string, size_t string_length, Rule * token_rule, 
                    Rule * root_rule, unsigned int line_offset, 
                    unsigned int col_offset, bool lazy_parse, 
                    char const * log_file) {
    Parser * parser = malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    if (Parser_init(parser, name, name_length, string, string_length, token_rule, root_rule, line_offset, 
                    col_offset, lazy_parse, log_file)) {
        free(parser);
        return NULL;
    }
    return parser;
}
err_type Parser_init(Parser * self, char const * name, size_t name_length,
                         char const * string, size_t string_length, Rule * token_rule, 
                         Rule * root_rule, unsigned int line_offset, 
                         unsigned int col_offset, bool lazy_parse, 
                         char const * log_file) {
    //printf("in Parser_init\n");
    self->name = name;
    self->name_length = name_length;
    self->token_rule = token_rule;
    self->root_rule = root_rule;
    self->loc_ = 0;
    self->tokens = malloc(sizeof(*self->tokens) * PARSER_DEFAULT_NTOKENS);
    if (!self->tokens) {
        return PEGGY_MALLOC_FAILURE;
    }
    self->tokens_capacity = PARSER_DEFAULT_NTOKENS;
    self->tokens_length = 0;
    // tokens_gc is always of length/capacity 1 less than self->tokens
    self->tokens_gc = NULL;
    self->tokens_gc_capacity = 0;
    self->tokens_gc_length = 0;

    // initialize first token
    Token * final_token = Token_new(string, 0, string_length, line_offset, col_offset);
    if (!final_token) {
        free(self->tokens_gc);
        free(self->tokens);
        return PEGGY_MALLOC_FAILURE;
    }
    self->tokens[self->tokens_length++] = final_token;

    /* allocate node_list */
    // oh boy, do I hate this, but it eliminates memory leaks. at least until I can make a nice arena allocator
    self->node_list = malloc(sizeof(*self->node_list) * PARSER_DEFAULT_NNODES);
    if (!self->node_list) {
        free(self->tokens_gc);
        free(self->tokens);
        free(final_token);
        return PEGGY_MALLOC_FAILURE;
    }
    self->node_list_length = 0;
    self->node_list_capacity = PARSER_DEFAULT_NNODES;

    self->log_file = log_file;
    self->disable_cache_check = false;
    /* TODO: log_buffer needed if log_file is not NULL */
    if (!lazy_parse) {
        self->_class->parse(self);
    }
    return PEGGY_SUCCESS;
}
void Parser_dest(Parser * self) {
    //printf("\ncleaning up Parser\n");
    /* clear out the ASTNodes */
    // oh boy, do I hate this, but it eliminates memory leaks. at least until I can make a nice arena allocator
    for (size_t i = 0; i < self->node_list_length; i++) {
        if (self->node_list[i]->children) {
            free(self->node_list[i]->children);
            self->node_list[i]->children = NULL;
            self->node_list[i]->nchildren = 0;
        }
        self->node_list[i]->_class->del(self->node_list[i]);
    }
    free(self->node_list);
    self->node_list = NULL;  
    self->node_list_length = 0;
    self->node_list_capacity = 0;  
    self->ast = NULL;
    
    //printf("clearing log_buffer\n");
    if (self->log_buffer) {
        free(self->log_buffer);
        self->log_buffer = NULL;
        self->log_buffer_length = 0;
    }

    // clear up the PackratCache's maintained for this set of tokens
    // do not necessarily have to nullify them. don't free them.
    self->root_rule->_class->clear_cache(self->root_rule, self->tokens[0]);
    self->token_rule->_class->clear_cache(self->token_rule, self->tokens[0]);

    /* clear the token list */
    //printf("cleaning up tokens\n");
    if (self->tokens_capacity) {
        for (size_t i = 0; i < self->tokens_length; i++) {
            self->tokens[i]->_class->del(self->tokens[i]);
        }
        free(self->tokens);
        self->tokens = NULL;
        self->tokens_length = 0;
        self->tokens_capacity = 0;
    }
    if (self->tokens_gc_capacity) {
        for (size_t i = 0; i < self->tokens_gc_length; i++) {
            self->tokens_gc[i]->_class->del(self->tokens_gc[i]);
        }
        free(self->tokens_gc);
        self->tokens_gc = NULL;
        self->tokens_gc_length = 0;
        self->tokens_gc_capacity = 0;
    }
    self->loc_ = 0;
}
void Parser_del(Parser * self) {
    Parser_dest(self);
    free(self);
}
size_t Parser_tell(Parser * self) {
    // at the end of the tokens list...someone is going to call check(). attempt to generate the next token
    if (self->loc_ == self->tokens_length - 1 && !self->disable_cache_check) {
        self->_class->gen_next_token_(self);
        // this sould be a good opportunity to set a flag for failure if gen_next_token_ fails
    }
    return self->loc_;
}
void Parser_seek(Parser * self, long long loc, seek_origin origin) {
    //printf("seeking from %zu, offset %llu (origin %d) to ", self->loc_, loc, origin);
    if (origin == P_SEEK_SET) {
        self->loc_ = loc;
        //printf("%zu\n", self->loc_);
        return;
    }
    size_t N = self->tokens[self->tokens_length - 1]->end;
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
    //printf("%zu\n", self->loc_);
}
void Parser_log(Parser * self, size_t loc, Rule * rule, ASTNode * result) {
    /* TODO */
}
void Parser_log_check_fail_(Parser * self, size_t loc, Rule * rule) {
    /* TODO */
}
void Parser_get_line_col_end(Parser * self, Token * tok, unsigned int * line_out, unsigned int * col_out) {
    char const * start = tok->string + tok->start;
    char const * end = tok->string + tok->end;
    unsigned int line = tok->coords.line;
    char const * chr = strchr(start, '\n');
    while (chr && chr < end) {
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
Token * Parser_gen_final_token_(Parser * self, ASTNode * node) {
    //printf("gen final token\n");
    /* TODO: when "longest_rule" is implemented, clear it */
    // tok and final may refer to the same token (in the case of Parser_skip_token)
    Token * final = self->tokens[self->tokens_length - 1];
    size_t end = final->end;
    final->end = final->start + node->str_length;
    size_t start = final->end;
    unsigned int line;
    unsigned int col;

    /* initialize tok */
    self->_class->get_line_col_end(self, final, &line, &col); // get token coordinates
    /* override const in initializing "tok" */
    return final->_class->new(final->string, start, end, line, col);
}
err_type Parser_extend_tokens_gc_(Parser * self) {
    /* set a default expansion */
    size_t new_capacity = (self->tokens_gc_capacity == 0) ? PARSER_DEFAULT_NTOKENS :  (self->tokens_gc_capacity * 2);
    Token ** new_tokens_gc = realloc(self->tokens_gc, sizeof(Token *) * new_capacity);
    if (!new_tokens_gc) {
        return PEGGY_MALLOC_FAILURE;
    }
    self->tokens_gc = new_tokens_gc;
    self->tokens_gc_capacity = new_capacity;
    return PEGGY_SUCCESS;
}
err_type Parser_skip_token(Parser * self, ASTNode * node) {
    //sprintf("Parser_skip_token\n");
    Token * new_final = self->_class->gen_final_token_(self, node);
    if (!new_final) {
        return PEGGY_MALLOC_FAILURE;
    }
    /* swap new final token for old final token and delete the old one */
    size_t index = self->tokens_length - 1;
    if (self->tokens_gc_length == self->tokens_gc_capacity) {
        err_type status = Parser_extend_tokens_gc_(self);
        if (status) {
            return status;
        }
    }
    self->tokens_gc[self->tokens_gc_length++] = self->tokens[index]; // move the current final token to garbage collection
    self->tokens[index] = new_final; // append new final token to the active tokens list
    return PEGGY_SUCCESS;
}
err_type Parser_extend_tokens_(Parser * self) {
    /* how far have we progressed already? */
    //Token * final = self->tokens + self->tokens_length - 1;
    /* set a default expansion */
    size_t new_capacity = (self->tokens_capacity == 0) ? PARSER_DEFAULT_NTOKENS :  (self->tokens_capacity * 2);
    /* estimate how many more tokens I need to allocate */
//    if (final->start > 0) {
//        new_capacity = 1 + (size_t) (((1.0 * final->end) / final->start) * self->tokens_length);
//    }
    Token ** new_tokens = realloc(self->tokens, sizeof(Token *) * new_capacity);
    if (!new_tokens) {
        return PEGGY_MALLOC_FAILURE;
    }
    self->tokens = new_tokens;
    self->tokens_capacity = new_capacity;
    return PEGGY_SUCCESS;
}
err_type Parser_add_token(Parser * self, ASTNode * node) {
    //printf("Parser_add_token");
    Token * new_final = self->_class->gen_final_token_(self, node);
    if (!new_final) {
        return PEGGY_MALLOC_FAILURE;
    }
    /* extend self->tokens if necessary */
    if (self->tokens_length == self->tokens_capacity) {
        err_type status = Parser_extend_tokens_(self);
        if (status) {
            //printf("failed to extend tokens list\n");
            return status;
        }
    }
    // append new token
    self->tokens[self->tokens_length++] = new_final;
    //printf("new token added(%zu/%zu): ", self->tokens_length, self->tokens_capacity);
    //Token_print(self->tokens[self->tokens_length - 2]);
    //printf("\n");
    return PEGGY_SUCCESS;
}
err_type Parser_extend_nodes_(Parser * self) {
    /* set a default expansion */
    size_t new_capacity = (self->node_list_capacity == 0) ? PARSER_DEFAULT_NNODES :  (self->node_list_capacity * 2);
    /* estimate how many more tokens I need to allocate */
    ASTNode ** new_node_list = realloc(self->node_list, sizeof(ASTNode *) * new_capacity);
    if (!new_node_list) {
        return PEGGY_MALLOC_FAILURE;
    }
    self->node_list = new_node_list;
    self->node_list_capacity = new_capacity;
    return PEGGY_SUCCESS;
}
err_type Parser_add_node(Parser * self, ASTNode * node) {
    //printf("registering a new node with Parser\n");
    if (self->node_list_length >= self->node_list_capacity) {
        err_type status = Parser_extend_nodes_(self);
        if (status) {
            return status;
        }
    }
    self->node_list[self->node_list_length++] = node;
    return PEGGY_SUCCESS;
}
bool Parser_gen_next_token_(Parser * self) {
    //printf("generating next token\n");
    self->disable_cache_check = true;
    ASTNode * result = self->token_rule->_class->check(self->token_rule, self, self->disable_cache_check);
    
    Token * final = self->tokens[self->tokens_length - 1];
    if (final->_class->len(final)) {
        //printf("disabling cache check\n");
        self->disable_cache_check = false;
    }
    return result != &ASTNode_fail;
}
err_type Parser_get(Parser * self, size_t key, Token ** tok) {
    if (!self->disable_cache_check) {
        //printf("getting token %zu / %zu from Parser\n", key, self->tokens_length);
        while (key >= self->tokens_length - 1) {
            Token * final = self->tokens[self->tokens_length - 1];
            //printf("final start-end: %zu - %zu\n", final->start, final->end);
            //printf("final: %p\n", (void *)final);
            //printf("final->_class: %p\n", (void *)final->_class);
            //printf("final->_class->len(final): %zu\n", final->_class->len(final));
            //printf("final token: %s (len = %zu)\n", final->string + final->start, final->_class->len(final));
            if (final->_class->len(final) == 0 || !self->_class->gen_next_token_(self)) {
                break;
            }
        }
    }
    if (key < self->tokens_length) {
        *tok = self->tokens[key];
        //printf("got token %zu of %zu!\n", key, self->tokens_length);
        return PEGGY_SUCCESS;
    }
    return PEGGY_INDEX_OUT_OF_BOUNDS;
}
Token ** Parser_get_tokens(Parser * self, ASTNode * node, size_t * ntokens) {
    *ntokens = node->ntokens;
    return self->tokens + node->token_key;
}
void Parser_parse(Parser * self) {
    //printf("in Parser_parse\n");
    self->ast = self->root_rule->_class->check(self->root_rule, self, false);
    if (self->ast == &ASTNode_fail) {
        printf("parsing failed\n"); /* TODO: include longest_rule information when available */
    } else if (self->ast->ntokens < self->tokens_length - 1) {
        printf("tokens remaining...failed?\n");
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
    //printf("executing skip_token action\n");
    node = make_skip_node(node);
    //printf("is_skip_node: %s\n", is_skip_node(node) ? "True" : "False");
    return node;
}

ASTNode * token_action(Parser * parser, ASTNode * node) {
    //printf("token action\n");
    if (node != &ASTNode_fail) {
        if (is_skip_node(node)) {
            //printf("node is skip_node\n");
            parser->_class->skip_token(parser, node);
        } else {
            //printf("node is not skip_node\n");
            parser->_class->add_token(parser, node);
        }
        // need to back up one token in order to ensure not erroneously moving the parser location forward
        parser->_class->seek(parser, -1, P_SEEK_CUR);
    }
    return node;
}