#ifndef PEGGY_TOKEN_H
#define PEGGY_TOKEN_H

#include <stddef.h>

#include <peggy/utils.h>

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

#endif // PEGGY_TOKEN_H

