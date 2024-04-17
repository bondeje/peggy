#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <peggy/utils.h>
#include <peggy/type.h>
#include <peggy/token.h>

/* Type system metadata for Token */
#define Token_NAME "Token"

Type const Token_TYPE = {._class = &Type_class,
                        .type_name = Token_NAME};
/* END Type system metadata for Token */

struct TokenType Token_class = TokenType_DEFAULT_INIT;

Token * Token_new(char const * string, size_t start, size_t end, unsigned int line, unsigned int col) {
    /* TODO: not really required since all instances will be initialized */
    return NULL;
}
err_type Token_init(Token * self, char const * string, size_t start, size_t end, unsigned int line, unsigned int col) {
    self->coords.col = col;
    self->coords.line = line;
    memcpy((void *)&(self->string), (void *)&string, sizeof(string));
    memcpy((void *)&(self->start), (void *)&start, sizeof(start));
    return PEGGY_SUCCESS;
}
void Token_del(Token * self) {
    free(self);
}
size_t Token_len(Token * self) {
    return self->end - self->start;
}
struct TokenCoords Token_coords(Token * self) {
    return self->coords;
}
/* forwards output from underlying sprintf call */
int Token_str(Token * self, char * buffer, size_t buf_size) {
    /* TODO */
    return 0;
}
err_type Token_get(Token * self, size_t key, char * chr) {
    if (key < self->_class->len(self)) {
        *chr = self->string[self->start + key];
        return PEGGY_SUCCESS;
    }
    return PEGGY_INDEX_OUT_OF_BOUNDS;
} /* retrieves just a single character so it acts like a string object */
bool Token_equal(Token * self, Token * other) {
    return self == other;
}
bool Token_equal_value(Token * self, Token * other) {
    size_t length = self->_class->len(self);
    if (length != self->_class->len(other)) {
        return false;
    }
    char const * string_s = self->string + self->start;
    char const * string_o = other->string + other->start;
    size_t i = 0;
    while (i < length) {
        if (string_s[i] != string_o[i]) {
            return false;
        }
        i++;
    }
    return true;
}