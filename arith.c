#include "arith.h"



// Print a field element
void print_f_elm(const f_elm_t a)
{
    printf("0x");
    for (int i = WORDS_FIELD * sizeof(digit_t) - 1; i >= 0; i--)
        printf("%02X", ((uint8_t *)(a))[i]);
    // printf("\n");
}

// Print a double size mp integer
void print_mp_elm(const digit_t *a, const int nwords)
{
    printf("0x");
    for (int i = nwords * sizeof(digit_t) - 1; i >= 0; i--)
        printf("%02X", ((uint8_t *)(a))[i]);
    // printf("\n");
}

// Print out an array of nbytes bytes as hex
void print_hex(const unsigned char *a, const int nbytes)
{
    printf("0x");
    for (int i = 0; i < nbytes; i++)
        printf("%02X", ((uint8_t *)(a))[i]);
    // printf("\n");
}





// Compare two field elements for equality, 0 if equal, 0xFF otherwise
uint8_t f_eq(const f_elm_t a, const f_elm_t b)
{
    uint8_t t = 0;
    for(unsigned int i = 0; i < NBYTES_FIELD; i++)
        t |= ((uint8_t *)a)[i] ^ ((uint8_t *)b)[i];

    return -!!t;
}

// Copy a field element
void f_copy(const f_elm_t a, f_elm_t b)
{
    for (int i = 0; i < WORDS_FIELD; i++)
        b[i] = a[i];
}



