#include <peggy/parser.h>
#include "testlang.h"
#include "testlangparser.h"

ASTNode * check_triplet(Production * prod, Parser * parser, ASTNode * node) {
    Token * tok = parser->_class->get_tokens(parser, node);
    if (tok->string[0] == tok->string[3] || tok->string[0] != tok->string[1] || tok->string[0] != tok->string[2]) {
        return &ASTNode_fail;
    }
    node->_class->init(node, (Rule *)prod, node->token_key, node->ntokens, node->str_length, node->nchildren, node->children);
    return node;
}

Parser * from_string(char const * string, size_t string_length, char const * name, size_t name_length, char const * log_file, unsigned char log_level) {
    return Parser_new("string", 6, string, string_length, (Rule *)&testlang_token, (Rule *)&testlang_testlang, TESTLANG_NRULES, 0, 0, 0, log_file, log_level);
}