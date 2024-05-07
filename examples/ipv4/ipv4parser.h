// ipv4parser.h
#ifndef IPV4PARSER_H
#define IPV4PARSER_H

#include <peggy/astnode.h>
#include <peggy/rule.h>
#include <peggy/parser.h>
ASTNode * check_ipv4(Production * octet, Parser * parser, ASTNode * node);

#endif 

