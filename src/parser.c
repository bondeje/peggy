#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <peggy/parser.h>
#include <peggy/packrat_cache.h>

#define Parser_NAME "Parser"

#define PARSER_NONE 0
#define PARSER_LAZY 1

Type const Parser_TYPE = {._class = &Type_class,
                        .type_name = Parser_NAME};

struct ParserType Parser_class = ParserType_DEFAULT_INIT;

Parser * Parser_new(char const * name, size_t name_length, char const * string, size_t string_length, Rule * token_rule, 
                    Rule * root_rule, size_t nrules, unsigned int line_offset, 
                    unsigned int col_offset, unsigned int flags, 
                    char const * log_file) {
    Parser * parser = malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    if (Parser_init(parser, name, name_length, string, string_length, token_rule, root_rule, nrules, line_offset, 
                    col_offset, flags, log_file)) {
        free(parser);
        return NULL;
    }
    return parser;
}
err_type Parser_init(Parser * self, char const * name, size_t name_length,
                         char const * string, size_t string_length, Rule * token_rule, 
                         Rule * root_rule, size_t nrules, unsigned int line_offset, 
                         unsigned int col_offset, unsigned int flags, 
                         char const * log_file) {
    //printf("in Parser_init\n");
    self->name = name;
    self->name_length = name_length;
    self->token_rule = token_rule;
    self->root_rule = root_rule;
    self->loc_ = 0;

    err_type status = PackratCache_init(&self->cache, nrules, PACKRAT_DEFAULT); // PACKRAT_SPARSE probably not yet implemented
    if (status) {
        return status;
    }
    self->cache_resize_trigger = self->cache.cache_[0].capacity - 1;

    // initialize token stack
    status = STACK_INIT(pToken)(&self->tokens, PARSER_DEFAULT_NTOKENS);
    if (status) {
        self->cache._class->dest(&self->cache);
        return status;
    }
    // initialize first token
    Token * final_token = Token_new(string, 0, string_length, line_offset, col_offset);
    if (!final_token) {
        self->cache._class->dest(&self->cache);
        self->tokens._class->dest(&self->tokens);
        return PEGGY_MALLOC_FAILURE;
    }
    self->tokens._class->push(&self->tokens, final_token);

    // initialize garbage collection list of ASTNode *s
    if ((status = STACK_INIT(pASTNode)(&self->node_list, PARSER_DEFAULT_NNODES))) {
        self->cache._class->dest(&self->cache);
        final_token->_class->del(final_token);
        self->tokens._class->dest(&self->tokens);
        return status;
    }

    self->log_file = log_file;
    self->disable_cache_check = false;
    self->flags = flags;
    /* TODO: log_buffer needed if log_file is not NULL */
    if (!(flags & PARSER_LAZY)) {
        self->_class->parse(self);
    }
    return PEGGY_SUCCESS;
}

int Parser_dest_pToken(void * data, Token * token) {
    free(token);
    return 0;
}

int Parser_dest_pASTNode(void * data, ASTNode * node) {
    free(node->children);
    node->children = NULL;
    node->nchildren = 0;
    node->_class->del(node);
    return 0;
}

void Parser_dest(Parser * self) {
    //printf("\ncleaning up Parser\n");
    /* clear out the ASTNodes */
    // oh boy, do I hate this, but it eliminates memory leaks. at least until I can make a nice arena allocator
    self->node_list._class->for_each(&self->node_list, &Parser_dest_pASTNode, NULL);
    self->node_list._class->dest(&self->node_list);
    
    self->ast = NULL;
    
    //printf("clearing log_buffer\n");
    if (self->log_buffer) {
        free(self->log_buffer);
        self->log_buffer = NULL;
        self->log_buffer_length = 0;
    }

    // clear the cache
    self->cache._class->dest(&self->cache);

    /* clear the token list */
    //printf("cleaning up tokens\n");
    self->tokens._class->for_each(&self->tokens, &Parser_dest_pToken, NULL);
    self->tokens._class->dest(&self->tokens);
    self->loc_ = 0;
}
void Parser_del(Parser * self) {
    Parser_dest(self);
    free(self);
}
size_t Parser_tell(Parser * self) {
    // at the end of the tokens list...someone is going to call check(). attempt to generate the next token
    if (self->loc_ == self->tokens.fill - 1 && !self->disable_cache_check) {
        Token * final;
        self->tokens._class->get(&self->tokens, self->loc_, &final);
        if (final->_class->len(final)) {
            while (self->loc_ == self->tokens.fill - 1 && self->_class->gen_next_token_(self)) {
                // do nothing. either of the conditions above will fail
            }
        }
        
        // this sould be a good opportunity to set a flag for failure if gen_next_token_ fails
    }
    return self->loc_;
}
void Parser_seek(Parser * self, long long loc, seek_origin origin) {
    //printf("seeking from %zu, offset %llu (origin %d) to ", self->loc_, loc, origin);
    if (origin == P_SEEK_SET) {
        self->loc_ = loc;
        //printf("\treset seek to %zu\n", self->loc_);
        return;
    }
    size_t N = self->tokens.fill;
    if (origin == P_SEEK_CUR) {
        //printf("\trelative seek of %lld from %zu\n", loc, self->loc_);
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
    Token * final;
    err_type status = self->tokens._class->peek(&self->tokens, &final);
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
err_type Parser_skip_token(Parser * self, ASTNode * node) {
    //sprintf("Parser_skip_token\n");
    Token * new_final = self->_class->gen_final_token_(self, node);
    if (!new_final) {
        return PEGGY_MALLOC_FAILURE;
    }
    /* swap new final token for old final token and delete the old one */
    Token * final;
    err_type status = self->tokens._class->pop(&self->tokens, &final);
    if (status) {
        return status;
    }
    final->_class->del(final);
    
    return self->tokens._class->push(&self->tokens, new_final);
}

err_type Parser_add_token(Parser * self, ASTNode * node) {
    //printf("Parser_add_token");
    if (self->tokens.fill == self->cache_resize_trigger) {
        double remaining_frac = ((self->tokens.bins[self->tokens.fill - 1]->end - self->tokens.bins[self->tokens.fill - 1]->start) / (1.0 * self->tokens.bins[self->tokens.fill - 1]->start));
        if (remaining_frac < 2) {
            remaining_frac = 2;
        }
        size_t new_cap = (size_t)((self->cache_resize_trigger + 1) * remaining_frac);
        //printf("resizing cache from %zu to %zu\n", self->cache_resize_trigger + 1, new_cap);
        self->cache_resize_trigger = new_cap - 1;
        self->cache._class->resize(&self->cache, new_cap);
        self->tokens._class->resize(&self->tokens, new_cap);
    }
    Token * new_final = self->_class->gen_final_token_(self, node);
    if (!new_final) {
        return PEGGY_MALLOC_FAILURE;
    }
    /*
    Token * final;
    self->tokens._class->peek(&self->tokens, &final);
    printf("\nnew token added(%zu/%zu): ", self->tokens.fill, self->tokens.capacity);
    Token_print(final);
    printf("\n\n");
    */
    return self->tokens._class->push(&self->tokens, new_final);
}

ASTNode * Parser_add_node(Parser * self, Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode * children[nchildren]) {
    ASTNode * new_node = ASTNode_class.new(rule, token_key, ntokens, str_length, nchildren, children);
    if (self->node_list.fill >= self->node_list.capacity - 1) {
        
        double remaining_frac = ((self->tokens.bins[self->tokens.fill - 1]->end - self->tokens.bins[self->tokens.fill - 1]->start) / (1.0 * self->tokens.bins[self->tokens.fill - 1]->start));
        if (remaining_frac < 2) {
            remaining_frac = 2;
        }
        size_t new_cap = (size_t)(self->node_list.capacity * remaining_frac);
        //printf("resizing nodes from %zu to %zu\n", self->node_list.capacity, new_cap);
        self->node_list._class->resize(&self->node_list, new_cap);
    }
    if (!new_node || self->node_list._class->push(&self->node_list, new_node)) {
        return NULL;
    }
    return new_node;
}
bool Parser_gen_next_token_(Parser * self) {
    //printf("generating next token\n");
    self->disable_cache_check = true;
    ASTNode * result = self->token_rule->_class->check(self->token_rule, self);
    
    Token * final;
    err_type status = self->tokens._class->peek(&self->tokens, &final);
    //if (final->_class->len(final)) {
        //printf("disabling cache check\n");
        self->disable_cache_check = false;
    //}
    return result != &ASTNode_fail;
}
err_type Parser_get(Parser * self, size_t key, Token ** tok) {
    if (!self->disable_cache_check) {
        //printf("getting token %zu / %zu from Parser\n", key, self->tokens.fill);
        while (key >= self->tokens.fill - 1) {
            Token * final;
            err_type status = self->tokens._class->peek(&self->tokens, &final);
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
    if (key < self->tokens.fill) {
        return self->tokens._class->get(&self->tokens, key, tok);
    }
    return PEGGY_INDEX_OUT_OF_BOUNDS;
}
Token ** Parser_get_tokens(Parser * self, ASTNode * node, size_t * ntokens) {
    *ntokens = node->ntokens;
    return self->tokens.bins + node->token_key;
}
void Parser_parse(Parser * self) {
    //printf("in Parser_parse\n");
    self->ast = self->root_rule->_class->check(self->root_rule, self);
    if (self->ast == &ASTNode_fail) {
        printf("parsing failed\n"); /* TODO: include longest_rule information when available */
    } else if (self->ast->ntokens < self->tokens.fill - 1) {
        printf("tokens remaining...failed?\n");
    } else {
        /* TODO: for auto_traverse */
    }
    /* TODO: logging */
}
// the token_key isn't strictly necessary, but should be used for safety
ASTNode * Parser_check_cache(Parser * self, rule_id_type rule_id, size_t token_key) {
    //return NULL; // disable packrat
    if (self->disable_cache_check) {
        return NULL;
    }
    return self->cache._class->get(&self->cache, rule_id, token_key);
}
void Parser_cache_check(Parser * self, rule_id_type rule_id, size_t token_key, ASTNode * node) {
    //printf("caching result of rule id %d at location %zu: %p\n", rule_id, token_key, (void*)node);
    self->cache._class->set(&self->cache, rule_id, token_key, node);
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