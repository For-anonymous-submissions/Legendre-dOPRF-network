// random.c
#include "random.h"
#include "../parameters.h"
#include <stdio.h>      // Include for perror()
#include <stdlib.h>     // Include for exit()
#include <stdarg.h>     // Include for seeded randombytes
#include <string.h>     // Include for memcpy
#include <blake3.h>



static int lock = -1;

static inline void delay(unsigned int count) {
    while (count--) {}
}

int randombytes_urandom(unsigned char* random_array, const size_t nbytes) {
        int r, n = (int)nbytes, count = 0;

        if (lock == -1) {
            do {
                lock = open("/dev/urandom", O_RDONLY);
                if (lock == -1) {
                    delay(0xFFFFF);
                }
            } while (lock == -1);
        }
        while (n > 0) {
            do {
                r = read(lock, random_array + count, n);
                if (r == -1) {
                    perror("Error reading from /dev/urandom");
                    exit(EXIT_FAILURE);
                    delay(0xFFFF);
                }
            } while (r == -1);
            count += r;
            n -= r;
        }
        return 0;
}

int randombytes_prng(unsigned char* random_array, const size_t nbytes, prng_seed seed) {
        unsigned char (*t) = malloc(nbytes + NBYTES_SEED);
        blake3_hasher hasher;

        blake3_hasher_init(&hasher);
        blake3_hasher_update(&hasher, seed, NBYTES_SEED);
        blake3_hasher_update(&hasher, (unsigned char*)&(nbytes), sizeof(size_t));
        blake3_hasher_finalize(&hasher, t, nbytes + NBYTES_SEED);

        memcpy(seed, t, NBYTES_SEED);
        memcpy(random_array, t + NBYTES_SEED, nbytes);
        free(t);
        return 0;
}

int randombytes_prng_long(unsigned char* random_array, const size_t nbytes, prng_seed* seed, const size_t nseeds) {
        unsigned char (*t) = malloc(nbytes + NBYTES_SEED);
        blake3_hasher hasher;

        blake3_hasher_init(&hasher);
        for(size_t i = 0; i < nseeds; i++)
            blake3_hasher_update(&hasher, *(seed + i), NBYTES_SEED);
        blake3_hasher_update(&hasher, (unsigned char*)&(nbytes), sizeof(size_t));
        blake3_hasher_finalize(&hasher, t, nbytes + NBYTES_SEED);

        memcpy(seed, t, NBYTES_SEED);
        memcpy(random_array, t + NBYTES_SEED, nbytes);
        free(t);
        return 0;
}


// Hash a field element into a hash digest
int hash(const f_elm_t input, hash_digest digest){
        blake3_hasher hasher;
        blake3_hasher_init(&hasher);
        blake3_hasher_update(&hasher, (unsigned char *)input, NBYTES_FIELD);
        blake3_hasher_finalize(&hasher, digest, NBYTES_DIGEST);

        return 0;
}

// Hash an array of field elements into a hash digest
int hash_array(const f_elm_t* input, const size_t nelms, hash_digest digest){
        blake3_hasher hasher;
        blake3_hasher_init(&hasher);
        blake3_hasher_update(&hasher, (unsigned char *)input, nelms * NBYTES_FIELD);
        blake3_hasher_finalize(&hasher, digest, NBYTES_DIGEST);

        return 0;
}