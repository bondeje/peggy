#ifndef PEGGY_TYPE_H
#define PEGGY_TYPE_H

#include <stddef.h>
#include <stdbool.h>

#include <peggy/utils.h>

//bool isinstance(Type const * type, unsigned short ntypes, char const * types[ntypes]);
bool isinstance(char const * type, char const ** types);

#endif // PEGGY_TYPE_H