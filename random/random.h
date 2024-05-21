// random.h
#ifndef RANDOM_H
#define RANDOM_H

#include "../arith.h"
#include <unistd.h>
#include <fcntl.h>


// Delay
static inline void delay(unsigned int count);

// Generate random bytes from /dev/urandom
int randombytes_urandom(unsigned char* random_array, const size_t nbytes);

// Generate random bytes from seed
int randombytes_prng(unsigned char* random_array, const size_t nbytes, prng_seed seed);

// Generate random bytes from multiple seeds
int randombytes_prng_long(unsigned char* random_array, const size_t nbytes, prng_seed* seed, const size_t nseeds);

// Generate random bytes from either depending on optional third argument
// Not a function but defined as a macro
#define randombytes_2 randombytes_urandom
#define randombytes_3 randombytes_prng
#define randombytes_4 randombytes_prng_long
#define randombytes(...) EXPAND(CONCATENATE(randombytes_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Hash a field element into a hash digest
int hash(const f_elm_t input, hash_digest digest);

// Hash an array of field elements into a hash digest
int hash_array(const f_elm_t* input, const size_t nelms, hash_digest digest);

#endif