#include "../../arith.h"

extern void f_red_asm(f_elm_t a);
extern void f_add_asm(const digit_t* a, const digit_t* b, digit_t* c);
extern void f_neg_asm(const f_elm_t a, f_elm_t b);
extern void f_sub_asm(const f_elm_t a, const f_elm_t b, f_elm_t c);
extern void mp_mul_asm(const digit_t* a, const digit_t* b, digit_t* c);
extern void mont_redc_asm(const digit_t* a, digit_t* c);
extern void f_mul_asm(const digit_t* a, const digit_t* b, digit_t* c);
extern void f_leg_asm(const digit_t* a, unsigned char* b);
extern void f_inv_asm(const digit_t* a, digit_t* b);
extern void f_sqrt_asm(const digit_t* a, digit_t* b);

// Reduction modulo p
inline void f_red(f_elm_t a) {
    f_red_asm(a); }

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





#if (PRIMES == ORIGINAL)

// Multiplicative inverse of a field element
void f_inv(const f_elm_t a, f_elm_t b){
    f_inv_asm(a, b); }

// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b){
    f_leg_asm(a, b); }

// Square root of a field element
void f_sqrt(const f_elm_t a, f_elm_t b){
    f_sqrt_asm(a, b); }




#elif (PRIMES == ALT)



// Multiplicative inverse of a field element
void f_inv(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[5];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* p - 2 = 0b   11111111 11111111
                    11111111 11111111
                    11111111 11111111
                    11111111 11111111

                    11111111 11111111
                    11111111 11111111

                    11111111 11111111

                    11111111 01010001
    */

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
    
    /* t[4] = a ^ 0b    11111111 11111111
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
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    f_copy(t[0], b);

}


// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    f_elm_t t[5];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p - 1)/2 = 0b   0 1111111 11111111
                        11111111 11111111
                        11111111 11111111
                        11111111 11111111

                        11111111 11111111
                        11111111 11111111

                        11111111 11111111

                        11111111 1 0101001\
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
    
    /* t[4] = a ^ 0b    11111111 11111111
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

    // Bit = 6
    f_mul(t[0], t[0], t[0]);
    // Bit = 5
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // Bit = 4
    f_mul(t[0], t[0], t[0]);
    // Bit = 3
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // Bit = 2
    f_mul(t[0], t[0], t[0]);
    // Bit = 1
    f_mul(t[0], t[0], t[0]);
    // Bit = 0
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);


    *b = ((*(unsigned char *)t[0]) & 0x02) >> 1;

}


void f_sqrt(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[5];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* p - 2 = 0b   00 111111 11111111
                    11111111 11111111
                    11111111 11111111
                    11111111 11111111

                    11111111 11111111
                    11111111 11111111

                    11111111 11111111

                    11111111 11 010101
    */

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
    
    /* t[4] = a ^ 0b    11111111 11111111
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

    // Bit = 5
    f_mul(t[0], t[0], t[0]);
    // Bit = 4
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Bit = 3
    f_mul(t[0], t[0], t[0]);
    // Bit = 2
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Bit = 1
    f_mul(t[0], t[0], t[0]);
    // Bit = 0
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    f_copy(t[0], b);

}


#endif
