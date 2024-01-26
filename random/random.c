// randomness.c
#include "random.h"
#include <stdio.h>
#include <stdio.h>   // Include for perror()
#include <stdlib.h>  // Include for exit()

static int lock = -1;

static inline void delay(unsigned int count) {
    while (count--) {}
}

int randombytes(unsigned char* random_array, unsigned long long nbytes) {
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