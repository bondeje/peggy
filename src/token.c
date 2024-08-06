#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // for Token_print only

#include <peggy/utils.h>
//#include <peggy/type.h>
#include <peggy/token.h>

void Token_init(Token * self, size_t id, char const * string, size_t length, unsigned int line, unsigned int col) {
    self->coords.col = col;
    self->coords.line = line;
    self->string = string;
    self->length = length;
    self->next = NULL;
    self->prev = NULL;
    self->id = id;
}

