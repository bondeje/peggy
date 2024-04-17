#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <peggy/type.h>

/* Type system metadata */
#define Type_NAME "Type"

/* END Type system metadata */

struct Type Type_class = Type_DEFAULT_INIT;

Type * Type_new(char const * type_name) {
    Type * ret = malloc(sizeof(Type));
    if (!ret) {
        return NULL;
    }
    *ret = (Type) Type_DEFAULT_INIT;
    if (!(Type_init(ret, type_name) == PEGGY_SUCCESS)) {
        Type_del(ret);
        return NULL;
    }
    return ret;
}
err_type Type_init(Type * self, char const * type_name) {
    self->type_name = type_name;
    return PEGGY_SUCCESS;
}
void Type_del(Type * self) {
    free(self);
}

bool isinstance(Type const * type, unsigned short ntypes, char const * types[ntypes]) {
    for (unsigned int i = 0; i < ntypes; i++) {
        if (strstr(type->type_name, types[i])) {
            return true;
        }
    }
    return false;
}