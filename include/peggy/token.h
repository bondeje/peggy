#ifndef PEGGY_TOKEN_H
#define PEGGY_TOKEN_H

#include <stddef.h>

#include <peggy/utils.h>
#include <peggy/type.h>

#define Token_DEFAULT_INIT {._class = &Token_class, \
                           }
#define TokenType_DEFAULT_INIT {._class = &Token_TYPE,\
                               .new = &Token_new,\
                               .init = &Token_init,\
                               .del = &Token_del,\
                               .len = &Token_len, \
                               .coords = &Token_coords, \
                               .str = &Token_str, \
                               .get = &Token_get, \
                               .equal = &Token_equal, \
                               .equal_value = &Token_equal, \
                               }

typedef struct Token Token, * pToken;

/* not an object */
typedef struct TokenCoords {
    unsigned int line;
    unsigned int col;
} TokenCoords;

struct Token {
    struct TokenCoords coords;
    struct TokenType * _class;
    char const * const string; /* the string of which Token is a substring. This is not modifiable so that it can be safely hashed */
    size_t const start; /* the start of the token in the string. This is not modifiable so that it can be safely hashed */
    size_t end; /* 1 after the final character in the token */
};

extern struct TokenType {
    Type const * _class;
    Token * (*new)(char const * string, size_t start, size_t end, unsigned int line, unsigned int col);
    err_type (*init)(Token * self, char const * string, size_t start, size_t end, unsigned int line, unsigned int col);
    void (*del)(Token * self);
    size_t (*len)(Token * self);
    struct TokenCoords (*coords)(Token * self);
    int (*str)(Token * self, char * buffer, size_t buf_size);
    err_type (*get)(Token * self, size_t key, char * chr); /* retrieves just a single character so it acts like a string object */
    bool (*equal)(Token * self, Token * other);
    bool (*equal_value)(Token * self, Token * other);
} Token_class;

Token * Token_new(char const * string, size_t start, size_t end, unsigned int line, unsigned int col);
err_type Token_init(Token * self, char const * string, size_t start, size_t end, unsigned int line, unsigned int col);
void Token_del(Token * self);
size_t Token_len(Token * self);
struct TokenCoords Token_coords(Token * self);
int Token_str(Token * self, char * buffer, size_t buf_size);
err_type Token_get(Token * self, size_t key, char * chr); /* retrieves just a single character so it acts like a string object */
bool Token_equal(Token * self, Token * other);
bool Token_equal_value(Token * self, Token * other);
void Token_print(Token * self);

#endif // PEGGY_TOKEN_H