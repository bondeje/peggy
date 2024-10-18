#ifndef HASH_UTILS_H
#define HASH_UTILS_H

#include <stddef.h>
#include <stdint.h>

typedef unsigned long long hash_type;

/**
 * Some utility functions for implementations of comparison and hash functions 
 * of the following types
 * - uintptr_t
 * - size_t
 * - null-terminated c-string
 * - a void pointer
 */
int uintptr_t_comp(uintptr_t a, uintptr_t b);
size_t uintptr_t_hash(uintptr_t a, size_t hash_size);
int size_t_comp(size_t a, size_t b);
size_t size_t_hash(size_t a, size_t hash_size);
int cstr_comp(char const * a, char const * b);
size_t cstr_hash(char const * key, size_t bin_size);
size_t address_hash(void const * val, size_t bin_size);
int address_comp(void const * a, void const * b);

/*
messages for hash_map_err outputs from the hash_map.h functions
*/
static void hs_print_error_message(int hash_map_status);
static void hm_print_error_message(int hash_set_status);

/**
 * utility functions to calculate next prime values for sizes of underlying 
 * array of hash maps
 */
_Bool is_prime(size_t x);
// inspired by Implementation 5 https://stackoverflow.com/questions/4475996/given-prime-number-n-compute-the-next-prime
// but reconfigured to handle some of the switch cases and reflect more the is_prime function
size_t next_prime(size_t x);


#endif
