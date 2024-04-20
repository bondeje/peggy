#include <stdio.h>
#include "ipv4parser.h"

#define ipv4Parser_NAME "ipv4Parser.Parser"

typedef struct ipv4Parser ipv4Parser;

#define ipv4Parser_DEFAULT_INIT {._class = &ipv4Parser_class, \
								.Parser = {._class = &(ipv4Parser_class.Parser_class), \
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

struct ipv4Parser {
    struct ipv4ParserType const * _class;
	Parser Parser;
} ipv4;

err_type ipv4Parser_init(ipv4Parser * parser, char const * string, size_t string_length) {
	//printf("in ipv4Parser_init\n");
	parser->Parser._class->init(&(parser->Parser), string, string, 
		string_length, &(ipv4_token.AnonymousProduction.DerivedRule.Rule), 
		&(ipv4_ipv4.AnonymousProduction.DerivedRule.Rule), 0, 0, false, NULL);
	return PEGGY_SUCCESS;
}

void ipv4Parser_dest(ipv4Parser * parser) {
	//ipv4_clear_token(parser->Parser.tokens); // this should be unnecessary as I fixed all the clear_cache's in the base Parser
	parser->Parser._class->dest(&(parser->Parser));
}

void ipv4Parser_parse(Parser * self) {
	//printf("in ipv4Parser_parse\n");
    self->ast = self->root_rule->_class->check(self->root_rule, self, false);
    if (self->ast == &ASTNode_fail) {
        printf("parsing failed, no matches found\n\n"); /* TODO: include longest_rule information when available */
    } else if (self->ast->ntokens < self->tokens_length - 1) {
        printf("parsing failed to tokenize input\n\n");
    } else {
		size_t n_list_el = self->ast->_class->len(self->ast);
		if (n_list_el == 7) {
			printf("parsing succeeds, valid IPV4\n\n");
		} else {
			printf("invalid IPV4 address. expecting format [0-255].[0-255].[0-255].[0-255], but found %zu octets\n\n", (n_list_el + 1) / 2);
		}
        
    }
    /* TODO: logging */
}

Type const ipv4Parser_TYPE = {._class = &Type_class,
                        .type_name = ipv4Parser_NAME};

struct ipv4ParserType {
    Type const * _class;
	struct ParserType Parser_class;
    err_type (*init)(ipv4Parser * parser, char const * string, size_t string_length);
    void (*dest)(ipv4Parser * parser);
} ipv4Parser_class = {._class = &ipv4Parser_TYPE,
						.Parser_class = {._class = &ipv4Parser_TYPE,
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
										.parse = &ipv4Parser_parse, 
										.traverse = &Parser_traverse, 
										.print_ast = &Parser_print_ast 
                        },
						.init = &ipv4Parser_init,
						.dest = &ipv4Parser_dest,
						};

int main(int narg, char * args[narg]) {
	ipv4 = (ipv4Parser) ipv4Parser_DEFAULT_INIT;
	
	printf("%s\n", args[0]);
	for (int i = 1; i < narg; i++) {
		printf("processing %s\n", args[i]);
		ipv4._class->init(&ipv4, args[i], strlen(args[i]));
		ipv4._class->dest(&ipv4);
	}
	
	ipv4_dest();
	return 0;
}