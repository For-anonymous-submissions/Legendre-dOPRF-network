#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include "parameters.h"
#include "random/random.h"


#define MASK(end, start) (((-(1ULL)) >> (64 - (end - start))) << start) // Compute mask from start bit to end-1 bit


// BASIC ARITHMETIC OPERATIONS ON SINGLE WORDS

// Digit multiplication
#define MUL(a, b, hi, lo){                                                                  \
    uint128_t t0 = (uint128_t)(a) * (uint128_t)(b);                                         \
    (hi) = (uint64_t)(t0 >> RADIX);                                                         \
    (lo) = (uint64_t)t0;                                                                    \
}

// Digit addition with carry
#define ADDC(carry, a, b, c){                                                               \
    uint128_t temp = (uint128_t)(a) + (uint128_t)(b) + (uint128_t)(carry);                    \
    (carry) = (uint64_t)(temp >> RADIX);                                                      \
    (c) = (uint64_t)temp;                                                                     \
}

// Digit subtraction with borrow
#define SUBC(borrow, a, b, c){                                                              \
    uint128_t temp2 = (uint128_t)(a) - (uint128_t)(b) - (uint128_t)(borrow);                   \
    (borrow) = (uint64_t)(temp2 >> (sizeof(uint128_t) * 8 - 1));                               \
    (c) = (uint64_t)temp2;                                                                     \
}



// Print a field element
void print_f_elm(const f_elm_t a);

// Print a double size mp integer
void print_mp_elm(const digit_t* a, const int nwords);

// Print out an array of nbytes bytes as hex
void print_hex(const unsigned char* a, const int nbytes);

// Print out a seed which can be copy pasted for re-use
void print_seed(const prng_seed a);


// Compare two field elements for equality, 1 if equal, 0 otherwise
uint8_t f_eq(const f_elm_t a, const f_elm_t b);

// Compare two field elements for non-equality, 0 if equal, 1 otherwise
uint8_t f_neq(const f_elm_t a, const f_elm_t b);

// Conditional select of two field elements
void cond_select(const f_elm_t a, const f_elm_t b, f_elm_t c, uint8_t cond);

// Copy a field element
void f_copy(const f_elm_t a, f_elm_t b);

// Correction, i.e., reduction modulo p
void f_corr(f_elm_t a);

// Generate a random field element
void f_rand(f_elm_t a);




// Reduction modulo p
void f_red(f_elm_t a);


// Generate a random field element
void f_rand_urandom(f_elm_t a);

// Generate a random field element from a seed with a prng
void f_rand_prng(f_elm_t a, prng_seed seed);

// Generate a random field element from either of the above depending on optional third argument
// Not a function but defined as a macro
#define f_rand_1 f_rand_urandom
#define f_rand_2 f_rand_prng
#define f_rand(...) EXPAND(CONCATENATE(f_rand_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Generate an array of random field elements from /dev/urandom
void f_rand_array_urandom(f_elm_t *a, unsigned long long num_elms);

// Generate an array of random field elements from a seed with a prng
void f_rand_array_prng(f_elm_t *a, unsigned long long num_elms, prng_seed seed);

// Generate an array of random field elements from an array of seeds with a prng
void f_rand_array_prng_long(f_elm_t *a, unsigned long long num_elms, prng_seed* seed, unsigned long long nseeds);

// Generate an array of random field elements from either of the above depending on optional third argument
// Not a function but defined as a macro
#define f_rand_array_2 f_rand_array_urandom
#define f_rand_array_3 f_rand_array_prng
#define f_rand_array_4 f_rand_array_prng_long
#define f_rand_array(...) EXPAND(CONCATENATE(f_rand_array_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Addition of two field elements
void f_add(const f_elm_t a, const f_elm_t b, f_elm_t c);

// Subtraction of two field elements
void f_sub(const f_elm_t a, const f_elm_t b, f_elm_t c);

// Negation of a field element
void f_neg(const f_elm_t a, f_elm_t b);

// Multiplication of two multiprecision words (without reduction)
void mp_mul(const digit_t* a, const digit_t* b, digit_t* c);

// Montgomery form reduction after multiplication
void mont_redc(const digit_t* a, digit_t* c);

// Multiplication of field elements
void f_mul(const f_elm_t a, const f_elm_t b, f_elm_t c);

// Convert a number from value to Montgomery form  (a -> aR)
void to_mont(const digit_t* a, f_elm_t b);

// Convert a number from Montgomery form into value (aR -> a)
void from_mont(const f_elm_t a, digit_t* b);

// Multiplicative inverse of a field element
void f_inv(const f_elm_t a, f_elm_t b);

// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b);

// Square root of a field element
void f_sqrt(const f_elm_t a, f_elm_t b);

#endif