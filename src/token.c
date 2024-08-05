#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // for Token_print only

#include <peggy/utils.h>
//#include <peggy/type.h>
#include <peggy/token.h>

/*
#define Token_NAME "Token"

struct TokenType Token_class = {
    .type_name = Token_NAME,
    .new = &Token_new,
    .init = &Token_init,
    .del = &Token_del,
    .len = &Token_len,
    .coords = &Token_coords,
    .str = &Token_str,
    .get = &Token_get,
    .equal = &Token_equal,
    .equal_value = &Token_equal,
};

Token * Token_new(size_t id, char const * string, size_t length, unsigned int line, unsigned int col) {
    Token * ret = malloc(sizeof(*ret));
    if (!ret) {
        return NULL;
    }
    // initialize class structure
    //memcpy((void*)ret, (void *)&((Token)Token_DEFAULT_INIT), sizeof(Token));
    Token_init(ret, id, string, length, line, col);
    return ret;
}
*/
void Token_init(Token * self, size_t id, char const * string, size_t length, unsigned int line, unsigned int col) {
    self->coords.col = col;
    self->coords.line = line;
    self->string = string;
    self->length = length;
    self->next = NULL;
    self->prev = NULL;
    self->id = id;
}
/*
void Token_del(Token * self) {
    free(self);
}
size_t Token_len(Token * self) {
    return self->length;
}
struct TokenCoords Token_coords(Token * self) {
    return self->coords;
}
// forwards output from underlying sprintf call
int Token_str(Token * self, char * buffer, size_t buf_size) {
    // TODO
    return 0;
}
err_type Token_get(Token * self, size_t key, char * chr) {
    
    if (key < Token_len(self)) {
        *chr = self->string[key];
        return PEGGY_SUCCESS;
    }
    return PEGGY_INDEX_OUT_OF_BOUNDS;
} // retrieves just a single character so it acts like a string object
bool Token_equal(Token * self, Token * other) {
    return self == other;
}
bool Token_equal_value(Token * self, Token * other) {
    size_t length = Token_len(self);
    if (length != Token_len(other)) {
        return false;
    }
    char const * string_s = self->string;
    char const * string_o = other->string;
    size_t i = 0;
    while (i < length) {
        if (string_s[i] != string_o[i]) {
            return false;
        }
        i++;
    }
    return true;
}

void Token_print(Token * self) {
    printf("%.*s", (int)self->length, self->string);
}

*/

