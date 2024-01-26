// randomness.h
#ifndef RANDOM_H
#define RANDOM_H

#include <unistd.h>
#include <fcntl.h>

// Delay
static inline void delay(unsigned int count);

// Generate random bytes
int randombytes(unsigned char* random_array, unsigned long long nbytes);

#endif