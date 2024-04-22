#ifndef PEGGY_H
#define PEGGY_H

#include <peggy/parser.h>

typedef struct PeggyParser PeggyParser;

extern Type PeggyParser_TYPE;

err_type PeggyParser_init(PeggyParser * parser, char const * filename, char const * string, size_t string_length);
void PeggyParser_dest(PeggyParser * parser);
void PeggyParser_parse(Parser * self);

extern struct PeggyParserType {
    Type const * _class;
	struct ParserType Parser_class;
    err_type (*init)(PeggyParser * parser, char const * filename, char const * string, size_t string_length);
    void (*dest)(PeggyParser * parser);
} PeggyParser_class;

ASTNode * handle_peggy(Parser * parser, ASTNode * node);

ASTNode * simplify_rule(Parser * parser, ASTNode * node);

#endif // PEGGY_H
