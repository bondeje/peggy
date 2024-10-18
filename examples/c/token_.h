#ifndef CEX_TOKEN__H
#define CEX_TOKEN__H

#include <stddef.h>

#include "common.h"
#include "peg4c/token.h"

BUILD_ALIGNMENT_STRUCT(Token)

extern Token const newline_token;

int Token_scomp(Token * a, Token * b);

size_t Token_shash(Token * a, size_t bin_size);

#endif
