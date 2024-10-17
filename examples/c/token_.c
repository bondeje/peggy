#include <string.h>

#include "token_.h"

Token const newline_token = {.string = "\n", .length = 1};

int Token_scomp(Token * a, Token * b) {
	if (a->length != b->length) {
		return 1;
	}
	return strncmp(a->string, b->string, a->length);
}

size_t Token_shash(Token * a, size_t bin_size) {
	unsigned long long hash = 5381;
	size_t i = 0;
	unsigned const char * str = (unsigned const char *)a->string;
	
	while (i < a->length) {
		hash = ((hash << 5) + hash) + *str; /* hash * 33 + c */
		str++;
		i++;
	}
	return hash % bin_size;
}
