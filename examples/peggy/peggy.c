#include <stdio.h>

#include "peggyparser.h"
#include "peggy.h"

ASTNode * handle_peggy(Parser * parser, ASTNode * node) {
    printf("handling peggy\n");
    parser = NULL;
    return node;
}

ASTNode * simplify_rule(Parser * parser, ASTNode * node) {
    //printf("simplifying node\n");
    parser = NULL;
    return node;
}

#define PeggyParser_NAME "PeggyParser.Parser"

#define PeggyParser_DEFAULT_INIT {._class = &PeggyParser_class, \
								.Parser = {._class = &(PeggyParser_class.Parser_class), \
									.token_rule = NULL, \
									.root_rule = NULL, \
									.tokens = NULL, \
									.name = "", \
									.log_buffer = NULL, \
									.log_buffer_length = 0, \
									.log_file = NULL, \
									.loc_ = 0, \
									.tokens_length = 0, \
									.tokens_capacity = 0, \
									.node_list = NULL, \
									.node_list_length = 0, \
									.node_list_capacity = 0, \
									.disable_cache_check = false, \
									.ast = NULL \
								} \
								}

Type PeggyParser_TYPE = {._class = &Type_class,
                        .type_name = PeggyParser_NAME};

struct PeggyParserType PeggyParser_class = {._class = &PeggyParser_TYPE,
						.Parser_class = {._class = &PeggyParser_TYPE,
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
										.add_token = &Parser_add_token, 
										.add_node = &Parser_add_node,
										.gen_next_token_ = &Parser_gen_next_token_, 
										.get = &Parser_get, 
										.get_tokens = &Parser_get_tokens, 
										.parse = &PeggyParser_parse, 
										.traverse = &Parser_traverse, 
										.print_ast = &Parser_print_ast 
                        },
						.init = &PeggyParser_init,
						.dest = &PeggyParser_dest,
						};

struct PeggyParser {
    struct PeggyParserType const * _class;
	Parser Parser;
} peggy = {._class = &PeggyParser_class, \
            .Parser = {._class = &(PeggyParser_class.Parser_class), \
                .token_rule = NULL, \
                .root_rule = NULL, \
                .tokens = NULL, \
                .name = "", \
                .log_buffer = NULL, \
                .log_buffer_length = 0, \
                .log_file = NULL, \
                .loc_ = 0, \
                .tokens_length = 0, \
                .tokens_capacity = 0, \
                .node_list = NULL, \
                .node_list_length = 0, \
                .node_list_capacity = 0, \
                .disable_cache_check = false, \
                .ast = NULL \
            } \
            };

err_type PeggyParser_init(PeggyParser * parser, char const * filename, char const * string, size_t string_length) {
	//printf("in ipv4Parser_init\n");
	parser->Parser._class->init(&(parser->Parser), filename, string, 
		string_length, &peggy_token.AnonymousProduction.DerivedRule.Rule, 
		&peggy_peggy.AnonymousProduction.DerivedRule.Rule, 0, 0, false, NULL);
	return PEGGY_SUCCESS;
}

void PeggyParser_dest(PeggyParser * parser) {
	parser->Parser._class->dest(&(parser->Parser));
}

void PeggyParser_parse(Parser * self) {
	//printf("in PeggyParser_parse\n");
    self->ast = self->root_rule->_class->check(self->root_rule, self, false);
    if (self->ast == &ASTNode_fail) {
        printf("parsing failed, no matches found\n\n"); /* TODO: include longest_rule information when available */
    } else if (self->ast->ntokens < self->tokens_length - 1) {
        printf("parsing failed to tokenize input\n\n");
    } else {
        printf("parsing succeeded\n");        
    }
    /* TODO: logging */
}

err_type from_string(char const * string, size_t string_length, char const * name) {
    printf("processing string:\n%s", string);
    peggy._class->init(&peggy, name, string, string_length);
    return PEGGY_SUCCESS;
}

err_type from_file(char const * filename) {
    printf("processing from file %s\n", filename);
    FILE * pfile = fopen(filename, "rb");
    if (!pfile) {
        printf("failed to open file: %s\n", filename);
    }
    fseek(pfile, 0, SEEK_END);
    long file_size = ftell(pfile);
    fseek(pfile, 0, SEEK_SET);

    char * string = malloc(file_size + 1);
    if (!string) {
        return PEGGY_MALLOC_FAILURE;
    }
    fread(string, 1, file_size, pfile);
    if (ferror(pfile)) {
        printf("error occurred in reading file: %s\n", filename);
        free(string);
        return PEGGY_FILE_IO_ERROR;
    }
    string[file_size] = '\0';

    fclose(pfile);

    err_type status = from_string(string, (size_t) file_size, filename);
    if (status) {
        return status;
    }

    free(string);
    return PEGGY_SUCCESS;
}


int main(int narg, char ** args) {
    printf("built!\n");
    if (narg > 1) {
        from_file(args[1]);
    }
    peggy._class->dest(&peggy);   
    peggy_dest();
    return 0;
}