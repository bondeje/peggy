#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <peggy/parser.h>
#include <peggy/packrat_cache.h>

#define Parser_NAME "Parser"

#define PARSER_NONE 0
#define PARSER_LAZY 1
#ifndef PARSER_LOGGER_BUFFER_SIZE
    #define PARSER_LOGGER_BUFFER_SIZE 1024
#endif
#define PARSER_PRINT_BUFFER_SIZE 4096
#ifndef PARSER_PRINT_TAB_SIZE 
    #define PARSER_PRINT_TAB_SIZE 4
#endif


struct ParserType Parser_class = {
    .type_name = Parser_NAME,
    .new = &Parser_new,
    .init = &Parser_init,
    .dest = &Parser_dest,
    .del = &Parser_del,
    .tell = &Parser_tell,
    .seek = &Parser_seek,
    .log = &Parser_log,
    .log_check_fail_ = &Parser_log_check_fail_,
    .get_line_col_end = &Parser_get_line_col_end,
    .gen_final_token_ = &Parser_gen_final_token_,
    .skip_token = &Parser_skip_token,
    .estimate_final_ntokens_ = &Parser_estimate_final_ntokens,
    .add_token = &Parser_add_token,
    .add_node = &Parser_add_node,
//    .move_node = &Parser_move_node,
//    .extend_node = &Parser_extend_node,
    .gen_next_token_ = &Parser_gen_next_token_,
    .get = &Parser_get,
    .get_tokens = &Parser_get_tokens,
    .parse = &Parser_parse,
    .check_cache = &Parser_check_cache,
    .cache_check = &Parser_cache_check,
    .traverse = &Parser_traverse,
    .print_ast = &Parser_print_ast
};

Parser * Parser_new(char const * name, size_t name_length, char const * string, size_t string_length, Rule * token_rule, 
                    Rule * root_rule, size_t nrules, unsigned int line_offset, 
                    unsigned int col_offset, unsigned int flags, 
                    char const * log_file, unsigned char log_level) {
    Parser * parser = malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    if (Parser_init(parser, name, name_length, string, string_length, token_rule, root_rule, nrules, line_offset, 
                    col_offset, flags, log_file, log_level)) {
        free(parser);
        return NULL;
    }
    return parser;
}
err_type Parser_init(Parser * self, char const * name, size_t name_length,
                         char const * string, size_t string_length, Rule * token_rule, 
                         Rule * root_rule, size_t nrules, unsigned int line_offset, 
                         unsigned int col_offset, unsigned int flags, 
                         char const * log_file, unsigned char log_level) {
    self->name = name;
    self->name_length = name_length;
    self->string = string;
    self->string_length = string_length;
    self->token_rule = token_rule;
    self->root_rule = root_rule;
    self->loc_ = 0;

    if (log_file) {
        self->log_file = log_file;
    } else {
        self->log_file = "stdout";
    }

    LOG_INIT(&self->logger, self->log_file, NULL, 0, log_level, 0, NULL, name);
    
    LOG_EVENT(&self->logger, LOG_LEVEL_INFO, "INFO: %s - initialized logger. finalizing initialization of parser\n", __func__);
    
    err_type status = PackratCache_init(&self->cache, nrules, PACKRAT_DEFAULT); // PACKRAT_SPARSE probably not yet implemented
    if (status) {
        return status;
    }

    // initialize token stack
    status = STACK_INIT(Token)(&self->tokens, PARSER_DEFAULT_NTOKENS);
    if (status) {
        self->cache._class->dest(&self->cache);
        return status;
    }

    if (!line_offset) {
        line_offset = 1;
    }
    if (!col_offset) {
        col_offset = 1;
    }

    // initialize first token
    Token final_token = Token_DEFAULT_INIT;
    final_token._class->init(&final_token, string, string_length, line_offset, col_offset);
    self->tokens._class->push(&self->tokens, final_token);

    // initialize garbage collection list of ASTNode *s
    if ((status = STACK_INIT(pASTNode)(&self->node_list, PARSER_DEFAULT_NNODES))) {
        self->cache._class->dest(&self->cache);
        //final_token->_class->del(final_token);
        self->tokens._class->dest(&self->tokens);
        return status;
    }

    self->disable_cache_check = false;
    self->flags = flags;

    if (!(flags & PARSER_LAZY)) {
        LOG_EVENT(&self->logger, LOG_LEVEL_INFO, "INFO: %s - eager parsing initiated\n", __func__);
        self->_class->parse(self);
    } else {
        LOG_EVENT(&self->logger, LOG_LEVEL_INFO, "INFO: %s - lazy parsing. waiting for manual parse command\n", __func__);
    }
    return PEGGY_SUCCESS;
}

int Parser_dest_pToken(void * data, Token * token) {
    free(token);
    return 0;
}

int Parser_dest_pASTNode(void * data, ASTNode * node) {
    LOG_EVENT(&((Parser *)data)->logger, LOG_LEVEL_TRACE, "TRACE: %s - freeing node at %p, children %p\n", __func__, (void *)node, (void*)node->children);
    node->_class->del(node);
    return 0;
}

void Parser_dest(Parser * self) {
    LOG_EVENT(&self->logger, LOG_LEVEL_INFO, "INFO: %s - destroying parser with %zu nodes and %zu tokens\n", __func__, self->node_list.fill, self->tokens.fill);
    
    /* clear out the ASTNodes */
    self->node_list._class->for_each(&self->node_list, &Parser_dest_pASTNode, self);
    self->node_list._class->dest(&self->node_list);
    
    self->ast = NULL;
    
    LOG_DEST(&self->logger);

    // clear the cache
    self->cache._class->dest(&self->cache);

    /* clear the token list */
    self->tokens._class->dest(&self->tokens);
    self->loc_ = 0;
}
void Parser_del(Parser * self) {
    Parser_dest(self);
    free(self);
}
size_t Parser_tell(Parser * self) {
    if (self->loc_ == self->tokens.fill - 1 && !self->disable_cache_check) {
        Token final;
        self->tokens._class->get(&self->tokens, self->loc_, &final);
        if (final._class->len(&final)) {
            LOG_EVENT(&self->logger, LOG_LEVEL_TRACE, "TRACE: %s - Parser_tell at location %zu (%zu tokens) triggered new token generation\n", __func__, self->loc_, self->tokens.fill);
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
    char const * start = tok->string;
    char const * end = tok->string + tok->length;
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
    if (!col) {
        col = 1;
    }
    *col_out = col;
    *line_out = line;
}
Token Parser_gen_final_token_(Parser * self, ASTNode * node) {
    Token * final = self->tokens.bins + self->tokens.fill - 1;
    size_t length = final->length - node->str_length;
    final->length = node->str_length;
    char const * string = final->string + final->length;
    unsigned int line;
    unsigned int col;

    /* initialize tok */
    self->_class->get_line_col_end(self, final, &line, &col); // get token coordinates
    /* override const in initializing "tok" */
    //return final->_class->new(final->string, start, end, line, col);
    Token new_tok = Token_DEFAULT_INIT;
    new_tok._class->init(&new_tok, string, length, line, col);
    return new_tok;
}
err_type Parser_skip_token(Parser * self, ASTNode * node) {
    Token new_final = self->_class->gen_final_token_(self, node);
    /* swap new final token for old final token and delete the old one */
    Token final;
    err_type status = self->tokens._class->pop(&self->tokens, &final);
    if (status) {
        return status;
    }
    return self->tokens._class->push(&self->tokens, new_final);
}

size_t Parser_estimate_final_ntokens(Parser * self) {
    double remaining_frac = ((self->tokens.bins[self->tokens.fill - 1].length) / (1.0 * ((size_t)(self->tokens.bins[self->tokens.fill - 1].string - self->string))));
    if (remaining_frac < 2.0) {
        remaining_frac = 2.0;
    }
    return (size_t)(self->tokens.fill * remaining_frac);
}

err_type Parser_add_token(Parser * self, ASTNode * node) {
    LOG_EVENT(&self->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - adding new token at line %u, col %u of length %zu: %.*s\n", __func__, self->tokens.bins[node->token_key].coords.line, self->tokens.bins[node->token_key].coords.col, node->str_length, (int)node->str_length, self->tokens.bins[node->token_key].string);
    
    if (self->tokens.fill >= self->tokens.capacity - 1) {
        size_t new_cap = Parser_estimate_final_ntokens(self);
        LOG_EVENT(&self->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - resizing token list from %zu to %zu\n", __func__, self->tokens.fill, new_cap);
        self->tokens._class->resize(&self->tokens, new_cap);
    }
    
    Token new_final = self->_class->gen_final_token_(self, node);
    Token final;
    self->tokens._class->peek(&self->tokens, &final);
    return self->tokens._class->push(&self->tokens, new_final);
}

ASTNode * Parser_add_node(Parser * self, Rule * rule, size_t token_key, size_t ntokens, size_t str_length, size_t nchildren, ASTNode ** children, size_t size) {
    LOG_EVENT(&self->logger, LOG_LEVEL_TRACE, "TRACE: %s - adding node with ntokens %zu, str_length %zu, nchildren %zu (%p) at %zu", __func__, ntokens, str_length, nchildren, (void*)children, token_key);
    if (!size) {
        size = sizeof(ASTNode);
    }
    ASTNode * new_node = malloc(size); // need to check for failure
    if (!new_node) {
        LOG_EVENT(&self->logger, LOG_LEVEL_ERROR, "ERROR: %s - failed to malloc new node\n", __func__);
        return NULL;
    }
    //printf("allocated node at %p (%zu)\n", (void*)new_node, size);
    *new_node = (ASTNode) ASTNode_DEFAULT_INIT;
    new_node->_class->init(new_node, rule, token_key, ntokens, str_length, nchildren, children);
    if (self->node_list.fill >= self->node_list.capacity - 1) {
        size_t new_cap = (size_t) (Parser_estimate_final_ntokens(self) * 1.0 / self->tokens.capacity * self->node_list.capacity);
        new_cap = new_cap > self->node_list.capacity ? new_cap : 2 * self->node_list.capacity;
        LOG_EVENT(&self->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - resizing node_list from %zu to %zu\n", __func__, self->node_list.capacity, new_cap);
        self->node_list._class->resize(&self->node_list, new_cap);
    }
    if (self->node_list._class->push(&self->node_list, new_node)) {
        LOG_EVENT(&self->logger, LOG_LEVEL_ERROR, "ERROR: %s - failed to add node to parser's node_list\n", __func__);
        return NULL;
    }
    return new_node;
}

//ASTNode * Parser_extend_node(Parser * self, ASTNode * node)
//void Parser_move_node(Parser * self, ASTNode * node)


bool Parser_gen_next_token_(Parser * self) {
    LOG_EVENT(&self->logger, LOG_LEVEL_TRACE, "TRACE: %s - generating next token at %zu\n", __func__, self->tokens.fill);

    self->disable_cache_check = true;
    ASTNode * result = self->token_rule->_class->check(self->token_rule, self);
    self->disable_cache_check = false;
    
    return result != &ASTNode_fail;
}
err_type Parser_get(Parser * self, size_t key, Token * tok) {
    if (key < self->tokens.fill) {
        return self->tokens._class->get(&self->tokens, key, tok);
    }
    LOG_EVENT(&self->logger, LOG_LEVEL_ERROR, "ERROR: %s - index %zu out of bounds %zu\n", __func__, key, self->tokens.fill);
    return PEGGY_INDEX_OUT_OF_BOUNDS;
}
Token * Parser_get_tokens(Parser * self, ASTNode * node) {
    return self->tokens.bins + node->token_key;
}
void Parser_parse(Parser * self) {
    LOG_EVENT(&self->logger, LOG_LEVEL_INFO, "INFO: %s - initiating parse\n", __func__);
    self->ast = self->root_rule->_class->check(self->root_rule, self);
}
// the token_key isn't strictly necessary, but should be used for safety
ASTNode * Parser_check_cache(Parser * self, rule_id_type rule_id, size_t token_key) {
    LOG_EVENT(&self->logger, LOG_LEVEL_TRACE, "TRACE: %s - retrieving cache result of rule id %d at location %zu: %s\n", __func__, rule_id, token_key, self->disable_cache_check ? "cache check disabled" : "");
    if (self->disable_cache_check) {
        return NULL;
    }
    return self->cache._class->get(&self->cache, rule_id, token_key);
}
void Parser_cache_check(Parser * self, rule_id_type rule_id, size_t token_key, ASTNode * node) {
    LOG_EVENT(&self->logger, LOG_LEVEL_TRACE, "TRACE: %s - caching result of rule id %d at location %zu: %p\n", __func__, rule_id, token_key, (void*)node);
    self->cache._class->set(&self->cache, self, rule_id, token_key, node);
}
err_type Parser_traverse(Parser * self, void (*traverse_action)(void * ctxt, ASTNode * node), void * ctxt) {
    /* TODO */
    /* requires a stack implementation */
    return PEGGY_NOT_IMPLEMENTED;
}
typedef struct ASTNodeSize {
    ASTNode * node;
    size_t size;
} ASTNodeSize;
#define ELEMENT_TYPE ASTNodeSize
#include <peggy/stack.h>

// this is unused and probably should be unused. delete
size_t ast_depth(ASTNode * root) {
    if (!root) {
        return 0;
    }
    size_t depth = 1;
    STACK(ASTNodeSize) st;
    STACK_INIT(ASTNodeSize)(&st, 0);
    st._class->push(&st, (ASTNodeSize) {.node = root, .size = 1});
    while (st.fill) {
        ASTNodeSize nodesize;
        st._class->pop(&st, &nodesize);
        ASTNode * node = nodesize.node;        
        size_t nchildren = node->nchildren;
        size_t new_depth = nodesize.size + 1;
        depth = depth * (new_depth <= depth) + new_depth * (new_depth > depth);
        while (nchildren--) {
            st._class->push(&st, (ASTNodeSize) {.node = node->children[nchildren], .size = new_depth});
        }
    }
    return depth;
}

err_type Parser_print_ast(Parser * self, FILE * stream) {
    LOG_EVENT(&self->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - starting ast print %p\n", __func__, (void*)self->ast);
    if (self->ast == &ASTNode_fail) {
        LOG_EVENT(&self->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - invalid AST for printing\n", __func__);
        return PEGGY_SUCCESS;
    }
    if (!stream) {
        stream = stdout;
    }
    char print_buffer[PARSER_PRINT_BUFFER_SIZE];
    char * buffer = &print_buffer[0];
    int buffer_size = PARSER_PRINT_BUFFER_SIZE;
    STACK(ASTNodeSize) st;
    STACK_INIT(ASTNodeSize)(&st, 0); // this is huge. should just get depth of ast

    ASTNodeSize pair;
    Token * toks = NULL;
    st._class->push(&st, (ASTNodeSize){.node = self->ast, .size = 0});
    err_type status = PEGGY_SUCCESS;
    int snp_size = 0;
    while (st.fill) {
        //printf("starting ast traversal loop with %zu elements in stack\n", st.fill);
        if ((status = st._class->pop(&st, &pair))) {
            fprintf(stream, "%.*s\nERROR: status %d retrieving node...aborting Parser_print_ast\n", PARSER_PRINT_BUFFER_SIZE - buffer_size, print_buffer, status);
            fflush(stream);
            goto print_ast_fail;
        }
        // print the information to the buffer
        if (!pair.node->nchildren) { // the node is a token leaf in the AST tree. Print the node and token into the buffer
            toks = self->_class->get_tokens(self, pair.node);
            snp_size = snprintf(buffer, buffer_size, "%*s%s: rule id: %d, ntokens: %zu, nchildren: %zu, token: %.*s\n", (int)pair.size * PARSER_PRINT_TAB_SIZE, "", pair.node->_class->type_name, pair.node->rule->id, pair.node->ntokens, pair.node->nchildren, (int)toks->length, toks->string);
        } else { // the node is not a leaf. Print the node into the buffer
            //printf("printing branch to buffer\n");
            snp_size = snprintf(buffer, buffer_size, "%*s%s: rule id: %d, ntokens: %zu, nchildren: %zu\n", (int)pair.size * PARSER_PRINT_TAB_SIZE, "", pair.node->_class->type_name, pair.node->rule->id, pair.node->ntokens, pair.node->nchildren);
            // increment the number of tabs and add the children in reverse order (pre-order traversal)
            pair.size++;
            size_t i = pair.node->nchildren;
            while (i--) {
                pair.node = pair.node->children[i];
                st._class->push(&st, pair);
            }
        }

        // update the buffer data based on how many characters were printed to the buffer
        if (snp_size >= 0) {
            if (snp_size < buffer_size) {
                buffer_size -= snp_size;
                buffer += snp_size;
                //printf("successfully updated buffer");
            } else if (snp_size < PARSER_PRINT_BUFFER_SIZE) {
                fprintf(stream, "%.*s", PARSER_PRINT_BUFFER_SIZE - buffer_size, print_buffer);
                fflush(stream);
                buffer = &print_buffer[0];
                buffer_size = PARSER_PRINT_BUFFER_SIZE;
            } else {
                fprintf(stream, "%.*sERROR: static buffer of insufficient size %d, need %d\n", PARSER_PRINT_BUFFER_SIZE - buffer_size, print_buffer, PARSER_PRINT_BUFFER_SIZE, snp_size);
                fflush(stream);
                buffer = &print_buffer[0];
                buffer_size = PARSER_PRINT_BUFFER_SIZE;
            }
        } else {
            fprintf(stream, "%.*s\nERROR: status %d retrieving node...aborting Parser_print_ast\n", PARSER_PRINT_BUFFER_SIZE - buffer_size, print_buffer, status);
            fflush(stream);
            goto print_ast_fail;
        }
        //printf("buffer contents: %.*s\n", PARSER_PRINT_BUFFER_SIZE - buffer_size, print_buffer);
    }

    // print remainder of buffer to stream
    fprintf(stream, "%.*s", PARSER_PRINT_BUFFER_SIZE - buffer_size, print_buffer);
    fflush(stream);

print_ast_fail:
    st._class->dest(&st);
    return status;
}

// use in e.g. WHITESPACE or comments
ASTNode * skip_token(Production * production, Parser * parser, ASTNode * node) {
    LOG_EVENT(&parser->logger, LOG_LEVEL_DEBUG, "DEBUG: %s - skipping node at line %u, col %u of length %zu\n", __func__, parser->tokens.bins[node->token_key].coords.line, parser->tokens.bins[node->token_key].coords.col, node->str_length);
    node = make_skip_node(node);
    DEBUG_ASSERT(is_skip_node(node), "ASSERT FAILURE: %s - node made skip node not registering as skip node\n", __func__);
    //printf("is_skip_node: %s\n", is_skip_node(node) ? "True" : "False");
    return node;
}

ASTNode * token_action(Production * token, Parser * parser, ASTNode * node) {
    if (node != &ASTNode_fail) {
        if (is_skip_node(node)) {
            LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - skipping token generated at line %u, col %u of length %zu\n", __func__, parser->tokens.bins[node->token_key].coords.line, parser->tokens.bins[node->token_key].coords.col, node->str_length);
            parser->_class->skip_token(parser, node);
        } else {
            LOG_EVENT(&parser->logger, LOG_LEVEL_TRACE, "TRACE: %s - adding token generated at line %u, col %u of length %zu\n", __func__, parser->tokens.bins[node->token_key].coords.line, parser->tokens.bins[node->token_key].coords.col, node->str_length);
            parser->_class->add_token(parser, node);
        }
        // need to back up one token in order to ensure not erroneously moving the parser location forward
        parser->_class->seek(parser, -1, P_SEEK_CUR);
    }
    return node;
}