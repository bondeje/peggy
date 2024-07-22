// ipv4parser.c
#include <stdio.h>
#include <peggy/astnode.h>
#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/token.h>
#include "ipv4.h"
#include "ipv4parser.h"

ASTNode ASTNode_syntax_error = {0}; // used to signal a failure that has already been addressed

int check_octet(Parser * parser, ASTNode * node, unsigned char loc) {
    // only success of the subrule will make it here. Do not have to check node for failure, node == NULL is a more serious failure so assume it is non-null
    Token * toks = parser->_class->get_tokens(parser, node);
    char const * str = (char const *)toks[0].string;
    unsigned char len = (unsigned char)(toks[node->ntokens - 1].length + (unsigned char)(toks[node->ntokens - 1].string - toks[0].string));
    // if the octet is < 3 decimals or numerically <= 255, it is valid
    if (len < 3 || str[0] < '2' || (str[0] == '2' && (str[1] < '5' || (str[1] == '5' && str[2] <= '5')))) {
        return 0;
    }
    printf("ipv4 failed. invalid octet at %hu (%.*s). must be in range 0-255\n", loc, len, str);
    return 1;
}
ASTNode * check_ipv4(Production * octet, Parser * parser, ASTNode * node) {
    // if the parser has already failed or resulted in something other than than 3 '.' + 4 octets = 7, fail
    ASTNode * result = node;
    if (node->nchildren != 7) { 
        printf("%s is NOT valid ipv4. %zu octets not found, need 4.\n", parser->string, (node->nchildren + 1) / 2);
        result = &ASTNode_syntax_error;
    }
    size_t i = 0;
    for (ASTNode * child = node->child; child; child = child->next ? child->next->next : NULL) {
        if (check_octet(parser, child, i++ / 2 + 1)) {
            result = &ASTNode_syntax_error;
        }
    }
    if (result == node) {
        printf("%s is valid ipv4\n", parser->string);
    }
    return result;
}

int main(int narg, char ** args) {
    if (narg > 1) {
        Parser parser = Parser_DEFAULT_INIT;
        while (--narg > 0) {
            // will print
            Parser_init(&parser, "ipv4", strlen("ipv4"), args[narg], strlen(args[narg]), (Rule *)&ipv4_token, 
                         (Rule *)&ipv4_ipv4, IPV4_NRULES, 0,0,0,NULL,0);
            // parser.ast should hold
            //      a valid node if passed,
            //      &ASTNode_syntax_error for any failure having been found
            //      &ASTNode_fail for any unhandled failures of the parser
            if (parser.ast == &ASTNode_fail) {
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