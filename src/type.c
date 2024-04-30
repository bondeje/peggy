#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <peggy/type.h>
/*

bool isinstance(char const * type, char const ** types) {
    static char buffer[256] = '\0';
    while (*types != NULL) {
        unsigned char len = (unsigned char)(strchr(*types, '.') - *types);
        memcpy((void *) buffer, *types, len);
        buffer[len] = '\0';
        //printf("checking if %s is instance of %s\n", type->type_name, buffer);
        if (strstr(type, buffer)) {
            return true;
        }
        types++;
    }
    return false;
}

*/