// ipv4parser.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <peggy/astnode.h>
#include <peggy/rule.h>
#include <peggy/parser.h>
#include <peggy/token.h>

#include "ipv4.h"
#include "ipv4parser.h"

ASTNode ASTNode_syntax_error = {0}; // used to signal a failure that has already been addressed

size_t max_ull_len = 0;

int check_octet(Parser * parser, ASTNode * node, unsigned char loc, size_t max) {
    // only success of the subrule will make it here. Do not have to check node for failure, node == NULL is a more serious failure so assume it is non-null
    char const * const str = node->token_start->string;
    size_t len = ASTNode_string_length(node);
    if (len > max_ull_len) { // if length of string is longer than an unsigned long long can possibly be, fail
        printf("ipv4 failed. invalid octet at %hu (%.*s). too many characters for valid number\n", loc, (int)len, str);
        return 1;
    }
    unsigned long long val = strtoull(str, NULL, 10);
    if (val >= max) {
        printf("ipv4 failed. invalid octet at %hu (%.*s). must be in range [0-%zu)\n", loc, (int)len, str, max);
        return 1;
    }
    return 0;
}
ASTNode * check_ipv4(Production * octet, Parser * parser, ASTNode * node) {
    // if the parser has already failed or resulted in something other than than 3 '.' + 4 octets = 7, fail
    if (node->nchildren < 3 || node->nchildren > 7 || Parser_is_fail_node(parser, node)) { 
        printf("%s is NOT valid ipv4. %zu octets not found, need 2-4.\n", parser->token_head->next->string, (node->nchildren + 1) / 2);
        return &ASTNode_syntax_error;
    }
    
    unsigned char noctets = (node->nchildren >> 1) + 1;
    // maximum value for the 
    size_t max = 1 << (8 * (5 - noctets));
    unsigned char i = 0;
    while (i < noctets - 1) {
        if (check_octet(parser, node->children[2 * i], i, 256)) {
            return &ASTNode_syntax_error;
        }
        i++;
    }
    while (i < noctets) {
        if (check_octet(parser, node->children[2 * i], i, max)) {
            return &ASTNode_syntax_error;
        }
        max >>= 8;
        i++;
    }
    printf("%s is valid ipv4\n", parser->token_head->next->string);
    return node;
}

int main(int narg, char ** args) {
    char buffer[256];
    max_ull_len = snprintf(buffer, 256, "%llu", ULLONG_MAX);
    if (narg > 1) {
        Parser parser = {._class = &Parser_class};
        Parser_init(&parser, ipv4rules[IPV4_TOKEN], ipv4rules[IPV4_IPV4], IPV4_NRULES, 0);
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

