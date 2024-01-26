#include "../../arith.h"

extern void f_red_asm(f_elm_t a);
extern void f_add_asm(const digit_t* a, const digit_t* b, digit_t* c);
extern void f_neg_asm(const f_elm_t a, f_elm_t b);
extern void f_sub_asm(const f_elm_t a, const f_elm_t b, f_elm_t c);
extern void mp_mul_asm(const digit_t* a, const digit_t* b, digit_t* c);
extern void mont_redc_asm(const digit_t* a, digit_t* c);
extern void f_mul_asm(const digit_t* a, const digit_t* b, digit_t* c);

// Reduction modulo p
inline void f_red(f_elm_t a) {
    f_red_asm(a); }

// Generate a random field element
void f_rand(f_elm_t a)
{
    randombytes((unsigned char *)a, sizeof(digit_t) * WORDS_FIELD);
    f_red(a); // Not uniformly random, can use rejection sampling to fix, thought it's pretty close to uniform
}

// Addition of two field elements
inline void f_add(const f_elm_t a, const f_elm_t b, f_elm_t c) {
    f_add_asm(a, b, c); }

// Negation of a field element
inline void f_neg(const f_elm_t a, f_elm_t b) {
    f_neg_asm(a, b); }

// Subtraction of two field elements
inline void f_sub(const f_elm_t a, const f_elm_t b, f_elm_t c) {
    f_sub_asm(a, b, c); }

// Multiplication of two multiprecision words (without reduction)
inline void mp_mul(const digit_t *a, const digit_t *b, digit_t *c) {
    mp_mul_asm(a, b, c); }

// Montgomery form reduction after multiplication
inline void mont_redc(const digit_t *a, digit_t *c) {
    mont_redc_asm(a, c); }

// Multiplication of field elements
inline void f_mul(const f_elm_t a, const f_elm_t b, f_elm_t c) {
    f_mul_asm(a, b, c); }



// Convert a number from value to Montgomery form  (a -> aR)
void to_mont(const digit_t *a, f_elm_t b)
{
    f_mul(a, R2, b);
}


// Convert a number from Montgomery form into value (aR -> a)
void from_mont(const f_elm_t a, digit_t *b)
{
    digit_t t0[2 * WORDS_FIELD] = {0};
    f_copy(a, t0);
    mont_redc(t0, b);
}




#if ALT_128

// Multiplicative inverse of a field element
void f_inv(const f_elm_t a, f_elm_t b)
{

    f_elm_t t0;
    unsigned int i;
    f_copy(Mont_one, t0);

    /* p - 2 =  1000000000000000\
                0000000000000000\
                0000000000000000\
                0000000000000000\
                0000000000000000\
                0000000000000000\
                0000000000000000\
                0000000000011011\
    */

    // bit = 127
    f_mul(t0, a, t0);

    // bits 126 down to 5
    for (i = 126; i > 4; i--)
    {
        f_mul(t0, t0, t0);
        // f_mul(t0, a, t0);
    }

    // bit = 4
    f_mul(t0, t0, t0);
    f_mul(t0, a, t0);

    // bit = 3
    f_mul(t0, t0, t0);
    f_mul(t0, a, t0);

    // bit = 2
    f_mul(t0, t0, t0);

    // bit = 1
    f_mul(t0, t0, t0);
    f_mul(t0, a, t0);

    // bit = 0
    f_mul(t0, t0, t0);
    f_mul(t0, a, t0);

    f_copy(t0, b);

}


// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    unsigned int i;
    f_elm_t t0;
    f_copy(Mont_one, t0);
    // Compute a^((p-1)/2)

    /* (p - 1)/2 = 0b   01000000 00000000\
                        00000000 00000000\
                        00000000 00000000\
                        00000000 00000000\
                        00000000 00000000\
                        00000000 00000000\
                        00000000 00000000\
                        00000000 00001110\
    */


    // bit = 127 (=0)

    // bit = 126
    f_mul(t0, a, t0);

    // bits 125 down to 4
    for (i = 125; i > 3; i--)
    {
        f_mul(t0, t0, t0);
    }

    // bit = 3
    f_mul(t0, t0, t0);
    f_mul(t0, a, t0);

    // bit = 2
    f_mul(t0, t0, t0);
    f_mul(t0, a, t0);

    // bit = 1
    f_mul(t0, t0, t0);
    f_mul(t0, a, t0);

    // bit = 0
    f_mul(t0, t0, t0);

    *b = ((*(unsigned char *)t0) & 0x08) >> 3;

}

#else


// Multiplicative inverse of a field element
void f_inv(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[6];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* p - 2 = 0b   11111111 11111111\
                    11111111 11111111\
                    11111111 11111111\
                    11111111 11111111\

                    11111111 11111111\
                    11111111 11111111\

                    11111111 11111111\
                    11111111 0101 1111\
    */

    // First 64 bits = 2^6 bits
    for(j = 0; j < 6; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 1) f_copy(t[0], t[2]);  // a^(2^4  - 1) = a^0b 1111
        if(j == 2) f_copy(t[0], t[3]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[4]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[5]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[3] = a ^ 0b    11111111 11111111
                        11111111 11111111
    */

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);

    // Next 16 bits
    for (i = 0; i < 16; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[4], t[0]);

    // Next 8 bits
    for (i = 0; i < 8; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[3], t[0]);

    // bit = 7
    f_mul(t[0], t[0], t[0]);
    // bit = 6
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // bit = 5
    f_mul(t[0], t[0], t[0]);
    // bit = 4
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // Last 4 bits
    for (i = 0; i < 4; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);

    f_copy(t[0], b);

}


// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    f_elm_t t[5];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p - 1)/2 = 0b   01111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\

                        11111111 11111111\
                        11111111 11111111\

                        11111111 11111111\
                        11111111 1 0110000\
    */


    // bit = 128 (=0)

    // First 64 bits = 2^6 bits
    for(j = 0; j < 6; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 2) f_copy(t[0], t[2]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[3] = a ^ 0b    11111111 11111111
                        11111111 11111111
    */

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[4], t[0]);

    // Next 16 bits
    for (i = 0; i < 16; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[3], t[0]);

    // Next 8 bits
    for (i = 0; i < 8; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);

    // bit = 6
    f_mul(t[0], t[0], t[0]);
    // bit = 5
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // bit = 4
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // bit = 3
    f_mul(t[0], t[0], t[0]);
    // bit = 2
    f_mul(t[0], t[0], t[0]);
    // bit = 1
    f_mul(t[0], t[0], t[0]);
    // bit = 0
    f_mul(t[0], t[0], t[0]);

    *b = ((*(unsigned char *)t[0]) & 0x40) >> 6;

}

#endif
