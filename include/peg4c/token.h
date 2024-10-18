#ifndef P4C_TOKEN_H
#define P4C_TOKEN_H

#include <stddef.h>

#include "peg4c/utils.h"

typedef struct Token Token, * pToken;

typedef struct TokenCoords {
    unsigned int line;
    unsigned int col;
} TokenCoords;

struct Token {
    struct TokenCoords coords;  //!< coordinates use to locate line/col
    char const * string;    //!< string 
    size_t length;          //!< string length. do not assume null terminated
    Token * prev;           //!< previous node in linked list
    Token * next;           //!< next node in linked list
    size_t id;              //!< an internal identifier for the token for the parsers to use
};

void Token_init(Token * self, size_t id, char const * string, size_t length, 
    unsigned int line, unsigned int col);

void Token_replace_tokens(Token * dstart, Token * dend, Token * sstart, Token * send);

void Token_insert_before(Token * tok, Token * sstart, Token * send);
#define Token_insert_after(tok, sstart, send) Token_insert_before((tok)->next, sstart, send)

void Token_remove_tokens(Token * start, Token * end);

#endif // P4C_TOKEN_H

