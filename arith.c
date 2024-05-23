#include "arith.h"



// Print a field element
void print_f_elm(const f_elm_t a)
{
    printf("0x");
    for (int i = WORDS_FIELD * sizeof(digit_t) - 1; i >= 0; i--)
        printf("%02X", ((uint8_t *)(a))[i]);
    printf("\n");
}

// Print a double size mp integer
void print_mp_elm(const digit_t *a, const int nwords)
{
    printf("0x");
    for (int i = nwords * sizeof(digit_t) - 1; i >= 0; i--)
        printf("%02X", ((uint8_t *)(a))[i]);
    printf("\n");
}

// Print out an array of nbytes bytes as hex
void print_hex(const unsigned char *a, const int nbytes)
{
    printf("0x");
    for (int i = 0; i < nbytes; i++)
        printf("%02X", ((uint8_t *)(a))[i]);
    printf("\n");
}

// Print out a seed which can be copy pasted for re-use
void print_seed(const prng_seed seed)
{
    printf("prng_seed seed = {                               \\\n");
    for (int i = 0; i < NBYTES_SEED; i++) {
        printf("0x%02X", seed[i]);
        if (i != NBYTES_SEED - 1)
            printf(", ");
        if( (i % 8 == 7) && (i != NBYTES_SEED - 1))
            printf(" \\\n");
    }
    printf("};\n");
}



// Compare two field elements for equality, 1 if equal, 0 if not equal
uint8_t f_eq(const f_elm_t a, const f_elm_t b)
{
    uint8_t t = 0;
    for(unsigned int i = 0; i < NBYTES_FIELD; i++)
        t |= ((uint8_t *)a)[i] ^ ((uint8_t *)b)[i];

    return 1 - (((t | (0-t)) >> 7) & 0x01);
}

// Compare two field elements for equality, 1 if not equal, 0 if equal
uint8_t f_neq(const f_elm_t a, const f_elm_t b)
{
    uint8_t t = 0;
    for(unsigned int i = 0; i < NBYTES_FIELD; i++)
        t |= ((uint8_t *)a)[i] ^ ((uint8_t *)b)[i];
    
     return ((t | (unsigned char)(0-t)) >> 7);
}

// if cond == 0, c = a; if cond == 1, c = b
void cond_select(const f_elm_t a, const f_elm_t b, f_elm_t c, uint8_t cond)
{
    digit_t mask = 0 - (digit_t)cond;

    for(int i = 0; i < WORDS_FIELD; i++){
            c[i] = (a[i] ^ b[i]) & mask;
            c[i] ^= a[i];
        }
}


// Copy a field element
void f_copy(const f_elm_t a, f_elm_t b)
{
    // printf("start f_copy \n");
    for (int i = 0; i < WORDS_FIELD; i++)
        b[i] = a[i];
}

// Generate a random field element from /dev/urandom
void f_rand_urandom(f_elm_t a)
{
    randombytes((unsigned char *)a, sizeof(f_elm_t));
    f_red(a); // Not uniformly random, can use rejection sampling to fix, thought it's pretty close to uniform
}

// Generate a random field element from a seed with a prng
void f_rand_prng(f_elm_t a, unsigned char* seed)
{
    randombytes((unsigned char *)a, sizeof(f_elm_t), seed);
    f_red(a); // Not uniformly random, can use rejection sampling to fix, thought it's pretty close to uniform
}


// Fill array with random field elements generated from /dev/urandom
void f_rand_array_urandom(f_elm_t *a, const unsigned long long num_elms)
{
    randombytes((unsigned char *)a, num_elms * sizeof(f_elm_t));
    for(unsigned long long i = 0; i < num_elms; i++)
        f_red(a[i]);
}

// Fill array with random field elements generated with a prng
void f_rand_array_prng(f_elm_t *a, const unsigned long long num_elms, prng_seed seed)
{
    randombytes((unsigned char *)a, num_elms * sizeof(f_elm_t), seed);
    for(unsigned long long i = 0; i < num_elms; i++)
        f_red(a[i]);
}

// Fill array with random field elements generated with a prng from an array of seeds
void f_rand_array_prng_long(f_elm_t *a, const unsigned long long num_elms, prng_seed* seed, unsigned long long nseeds)
{
    randombytes((unsigned char *)a, num_elms * sizeof(f_elm_t), seed, nseeds);
    for(unsigned long long i = 0; i < num_elms; i++)
        f_red(a[i]);
}
