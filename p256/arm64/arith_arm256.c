#include "../../arith.h"

extern void f_red_asm(f_elm_t a);
extern void f_add_asm(const digit_t* a, const digit_t* b, digit_t* c);
extern void f_neg_asm(const f_elm_t a, f_elm_t b);
extern void f_sub_asm(const f_elm_t a, const f_elm_t b, f_elm_t c);
// extern void mp_mul_asm(const digit_t* a, const digit_t* b, digit_t* c);
// extern void mont_redc_asm(const digit_t* a, digit_t* c);
extern void f_mul_asm(const digit_t* a, const digit_t* b, digit_t* c);


// Reduction modulo p
// a in [0, 2 * p - 1] -> a in [0, p-1]
inline void f_red(f_elm_t a) {
    f_red_asm(a); }


// Addition of two field elements
inline void f_add(const f_elm_t a, const f_elm_t b, f_elm_t c) {
    f_add_asm(a, b, c); }


// Negation of a field element
inline void f_neg(const f_elm_t a, f_elm_t b) {
    f_neg_asm(a, b); }


// Subtraction of two field elements
inline void f_sub(const f_elm_t a, const f_elm_t b, f_elm_t c){    
    f_sub_asm(a, b, c); }


// Multiplication of two multiprecision words (without reduction)
void mp_mul(const digit_t *a, const digit_t *b, digit_t *c)
{ // Schoolbook multiplication
    digit_t carry, th, tl, t = 0, u = 0, v = 0;

    for (int i = 0; i < WORDS_FIELD; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            carry = 0;
            MUL(a[j], b[i - j], th, tl);
            ADDC(carry, tl, v, v);
            ADDC(carry, th, u, u);
            t += carry;
        }
        c[i] = v;
        v = u;
        u = t;
        t = 0;
    }

    for (int i = WORDS_FIELD; i < 2 * WORDS_FIELD - 1; i++)
    {
        for (int j = i - WORDS_FIELD + 1; j < WORDS_FIELD; j++)
        {
            carry = 0;
            MUL(a[j], b[i - j], th, tl);
            ADDC(carry, tl, v, v);
            ADDC(carry, th, u, u);
            t += carry;
        }
        c[i] = v;
        v = u;
        u = t;
        t = 0;
    }
    c[2 * WORDS_FIELD - 1] = v;
}


// Montgomery form reduction after multiplication
void mont_redc(const digit_t *a, digit_t *c)
{
    // c = a*R^-1 mod p, where R = 2^256.
    // If a < 2^256*p, the output c is in the range [0, p).
    // a is assumed to be in Montgomery representation.
    digit_t mask, carry = 0;
    digit_t t0[2 * WORDS_FIELD], t1[WORDS_FIELD];

    mp_mul(a, ip, t0);
    f_copy(t0, t1);
    mp_mul(t1, p, t0);

    for (int i = WORDS_FIELD; i < 2 * WORDS_FIELD; i++)
        SUBC(carry, a[i], t0[i], t0[i]);


    mask = 0 - carry;
    carry = 0;
    for (int i = 0; i < WORDS_FIELD; i++)
        ADDC(carry, t0[WORDS_FIELD + i], p[i] & mask, c[i]);

}


// Multiplication of field elements
inline void f_mul(const f_elm_t a, const f_elm_t b, f_elm_t c){
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
void f_inv(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[6];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* p - 2 =     0b   0 1111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 1 1101011\
    */

    // Compute a^(p-2)
    // bit 255 = 0

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 2) f_copy(t[0], t[2]);  // t[2] = a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // t[3] = a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // t[4] = a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        if(j == 5) f_copy(t[0], t[5]);  // t[5] = a^(2^64 - 1) = a^0b 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // = a^(2^(2^(j+1)) - 1)
    }
    
    /* t[3] = a ^ 0b 11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
    */

    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);

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
    f_mul(t[0], a, t[0]);
    // bit = 5
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // bit = 4
    f_mul(t[0], t[0], t[0]);
    // bit = 3
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // bit = 2
    f_mul(t[0], t[0], t[0]);
    // bit = 1
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // bit = 0
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    f_copy(t[0], b);  

}


// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    f_elm_t t[6];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p - 1)/2 = 0b   00 111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11111111\
                        11111111 11 110110\
    */
    // Compute a^(p-2)
    // bit 255 = 0

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 2) f_copy(t[0], t[2]);  // t[2] = a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // t[3] = a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // t[4] = a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        if(j == 5) f_copy(t[0], t[5]);  // t[5] = a^(2^64 - 1) = a^0b 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // = a^(2^(2^(j+1)) - 1)
    }
    
    /* t[3] = a ^ 0b 11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
    */

    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);

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
    f_mul(t[0], a, t[0]);
    // bit = 1
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // bit = 0
    f_mul(t[0], t[0], t[0]);

    *b = ((*(unsigned char *)t[0]) & 0x01);

}


// Multiplicative inverse of a field element
void f_sqrt(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[7];
    f_elm_t psi = {0x3B5807D4FE2BDB04, 0x03F590FDB51BE9ED, 0x6D6E16BF336202D1, 0x75776B0BD6C71BA8};
    unsigned int i, j;
    digit_t mask = 0;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* d = (p - 1)/4 =0b    000 11111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\

                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\

                            11111111 11111111\
                            11111111 11111111\

                            11111111 11111111\

                            11111111 111 11 011\
    */

    /* (d + 1)/2 = 0b       0000 1111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\

                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\
                            11111111 11111111\

                            11111111 11111111\
                            11111111 11111111\

                            11111111 11111111\

                            11111111 1111 11 10\
    */

    // Compute a^(p-2)
    // bit 255 = 0

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[6]);  // t[6] = a^(2^2  - 1) = a^0b 11
        if(j == 2) f_copy(t[0], t[2]);  // t[2] = a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // t[3] = a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // t[4] = a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        if(j == 5) f_copy(t[0], t[5]);  // t[5] = a^(2^64 - 1) = a^0b 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // = a^(2^(2^(j+1)) - 1)
    }
    
    /* t[3] = a ^ 0b 11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
                     11111111 11111111
    */

    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);

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


    // Next 2 bits
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[6], t[0]);

    // Next 1 bit
    f_mul(t[0], t[0], t[0]);


    // t[1] = a^d
    f_mul(t[0], t[0], t[1]);
    f_mul(t[1], t[1], t[1]);
    f_mul(t[1], t[6], t[1]);

    // t[0] = a^(d+1)/2
    f_mul(t[0], a, t[0]);
    f_mul(t[0], t[0], t[0]);




    // t[0] has to be multiplied with 1 if t[1] is 1, and with psi if t[1] is -1
    mask = 0 - (((digit_t) f_neq(t[1], Mont_one)) & 0x01);
    
    for(i = 0; i < WORDS_FIELD; i++){
        psi[i] = (psi[i] ^ Mont_one[i]) & mask;
        psi[i] ^= Mont_one[i];
    }

    f_mul(t[0], psi, t[0]);

    f_copy(t[0], b);
 

}


#else

// Multiplicative inverse of a field element
void f_inv(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[6];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);
    /* p - 2 =  0b  11111111 11111111
                    11111111 11111111

                    00000000 00000000
                    00000000 00000001

                    00000000 00000000
                    00000000 00000000
                    00000000 00000000
                    00000000 00000000
                    00000000 00000000
                    00000000 00000000

                    11111111 11111111
                    11111111 11111111
                    11111111 11111111
                    11111111 11111111

                    11111111 11111111
                    11111111 1111 11 01
    */


    // First 32 bits = 2^5 bits
    for(j = 0; j < 5; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[2]);  // t[2] = a^(2^2  - 1) = a^0b 11
        if(j == 1) f_copy(t[0], t[3]);  // t[3] = a^(2^4  - 1) = a^0b 1111
        if(j == 2) f_copy(t[0], t[4]);  // t[4] = a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[5]);  // t[5] = a^(2^16 - 1) = a^0b 11111111 11111111
        f_copy(t[0], t[1]);             // = a^(2^(2^(j+1)) - 1)
    }
    
    /* t[1] = a ^ 0b 11111111 11111111
                     11111111 11111111
    */

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Next 96 bits
    for (i = 0; i < 96; i++)
        f_mul(t[0], t[0], t[0]);

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[1], t[0]);

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[1], t[0]);

    // Next 16 bits
    for (i = 0; i < 16; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);

    // Next 8 bits
    for (i = 0; i < 8; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[4], t[0]);

    // Next 4 bits
    for (i = 0; i < 4; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[3], t[0]);

    // Next 2 bits
    for (i = 0; i < 2; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);
    
    // Next 1 bit
    f_mul(t[0], t[0], t[0]);

    // Next 1 bit
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    f_copy(t[0], b);  

}


// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    f_elm_t t[6];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);
    /* (p - 1)/2 =  0b  0 11111111 11111111
                        11111111 11111111

                        00000000 00000000
                        00000000 00000001

                        00000000 00000000
                        00000000 00000000
                        00000000 00000000
                        00000000 00000000
                        00000000 00000000
                        00000000 00000000

                        11111111 11111111
                        11111111 11111111
                        11111111 11111111
                        11111111 11111111

                        11111111 11111111
                        11111111 1111111
    */


    // First 32 bits = 2^5 bits
    for(j = 0; j < 5; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[2]);  // t[2] = a^(2^2  - 1) = a^0b 11
        if(j == 1) f_copy(t[0], t[3]);  // t[3] = a^(2^4  - 1) = a^0b 1111
        if(j == 2) f_copy(t[0], t[4]);  // t[4] = a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[5]);  // t[5] = a^(2^16 - 1) = a^0b 11111111 11111111
        f_copy(t[0], t[1]);             // = a^(2^(2^(j+1)) - 1)
    }
    
    /* t[1] = a ^ 0b 11111111 11111111
                     11111111 11111111
    */

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Next 96 bits
    for (i = 0; i < 96; i++)
        f_mul(t[0], t[0], t[0]);

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[1], t[0]);

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[1], t[0]);

    // Next 16 bits
    for (i = 0; i < 16; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);

    // Next 8 bits
    for (i = 0; i < 8; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[4], t[0]);

    // Next 4 bits
    for (i = 0; i < 4; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[3], t[0]);

    // Next 2 bits
    for (i = 0; i < 2; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);
    
    // Next 1 bit
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    *b = ((*(unsigned char *)t[0]) & 0x02) >> 1;

}


// Legendre symbol of a field element
void f_sqrt(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[2];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);
    /* (p + 1)/4 =  0b  00 11111111 11111111
                        11111111 11111111

                        00000000 00000000
                        00000000 00000001

                        00000000 00000000
                        00000000 00000000
                        00000000 00000000
                        00000000 00000000
                        00000000 00000000
                        00000000 00000001

                        00000000 00000000
                        00000000 00000000
                        00000000 00000000
                        00000000 00000000
                        00000000 00000000
                        00000000 000000
    */


    // First 32 bits = 2^5 bits
    for(j = 0; j < 5; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        f_copy(t[0], t[1]);             // = a^(2^(2^(j+1)) - 1)
    }
    
    /* t[1] = a ^ 0b 11111111 11111111
                     11111111 11111111
    */

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Next 96 bits
    for (i = 0; i < 96; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Next 94 bits
    for (i = 0; i < 94; i++)
        f_mul(t[0], t[0], t[0]);

    f_copy(t[0], b);
}



#endif



