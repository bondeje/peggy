// ipv4parser.c
#include <stdio.h>
#include <string.h>

#include <peggy/astnode.h>
#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/token.h>

#include "ipv4.h"
#include "ipv4parser.h"

ASTNode ASTNode_syntax_error = {0}; // used to signal a failure that has already been addressed

int check_octet(Parser * parser, ASTNode * node, unsigned char loc) {
    // only success of the subrule will make it here. Do not have to check node for failure, node == NULL is a more serious failure so assume it is non-null
    Token * toks = node->token_start;
    char const * const str = toks->string;
    size_t len = ASTNode_string_length(node);
    // if the octet is < 3 decimals or numerically <= 255, it is valid
    //printf("octet (%zu)(%zu)(%zu): %.*s\n", len, node->str_length, node->nchildren, (int)len, str);
    if (len && (len < 3 || str[0] < '2' || (str[0] == '2' && (str[1] < '5' || (str[1] == '5' && str[2] <= '5'))))) {
        return 0;
    }
    printf("ipv4 failed. invalid octet at %hu (%.*s). must be in range 0-255\n", loc, (int)len, str);
    return 1;
}
ASTNode * check_ipv4(Production * octet, Parser * parser, ASTNode * node) {
    // if the parser has already failed or resulted in something other than than 3 '.' + 4 octets = 7, fail
    if (node->nchildren != 7 || Parser_is_fail_node(parser, node)) { 
        printf("%s is NOT valid ipv4. %zu octets not found, need 4.\n", parser->token_head->next->string, (node->nchildren + 1) / 2);
        return &ASTNode_syntax_error;
    }
    
    for (size_t i = 0; i < node->nchildren; i++) {
        if (check_octet(parser, node->children[i], i + 1)) {
            return &ASTNode_syntax_error;
        }
    }
    printf("%s is valid ipv4\n", parser->token_head->next->string);
    return node;
}

int main(int narg, char ** args) {
    if (narg > 1) {
        Parser parser = {._class = &Parser_class};
        Parser_init(&parser, (Rule *)&ipv4_token, (Rule *)&ipv4_ipv4, IPV4_NRULES, 0);
        while (--narg > 0) {
            // will print
            Parser_parse(&parser, args[narg], strlen(args[narg]));
            if (Parser_is_fail_node(&parser, parser.ast)) {
                printf("unhandled ipv4 check error\n");
            }
        }
        Parser_dest(&parser);   // clean up the parser
        ipv4_dest();            // clean up the ipv4 module
    } else {
        printf("no candidate ip address provided\n");
    }
    return 0;
}

