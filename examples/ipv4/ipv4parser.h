// ipv4parser.h
#ifndef IPV4PARSER_H
#define IPV4PARSER_H

#include "peg4c/astnode.h"
#include "peg4c/rule.h"
#include "peg4c/parser.h"
ASTNode * check_ipv4(Production * octet, Parser * parser, ASTNode * node);

#endif 

