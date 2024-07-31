#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#include <stddef.h>

int uintptr_comp(size_t a, size_t b);
size_t uintptr_hash(size_t a, size_t hash_size);
size_t cstr_hash(char const * key, size_t bin_size);
int cstr_comp(char const * a, char const * b);
size_t address_hash(void const * val, size_t bin_size);
int address_comp(void const * a, void const * b);
static void hs_print_error_message(int hash_map_status);
static void hm_print_error_message(int hash_set_status);

#endif
