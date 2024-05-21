#include "../../arith.h"

extern void f_red_asm(f_elm_t a);
extern void f_add_asm(const digit_t* a, const digit_t* b, digit_t* c);
extern void f_neg_asm(const f_elm_t a, f_elm_t b);
extern void f_sub_asm(const f_elm_t a, const f_elm_t b, f_elm_t c);
extern void mp_mul_asm(const digit_t* a, const digit_t* b, digit_t* c);
extern void mont_redc_asm(const digit_t* a, digit_t* c);
extern void f_mul_asm(const digit_t* a, const digit_t* b, digit_t* c);


// Reduction modulo p
// a in [0, 2 * p - 1] -> a in [0, p-1]
inline void f_red(f_elm_t a){
    f_red_asm(a); }

// Addition of two field elements
inline void f_add(const f_elm_t a, const f_elm_t b, f_elm_t c){
    f_add_asm(a, b, c); }

// Subtraction of two field elements
inline void f_sub(const f_elm_t a, const f_elm_t b, f_elm_t c){
    f_sub_asm(a, b, c); }

// Negation of a field element
inline void f_neg(const f_elm_t a, f_elm_t b){
    f_neg_asm(a, b); }

// Multiplication of two multiprecision words (without reduction)
inline void mp_mul(const digit_t *a, const digit_t *b, digit_t *c){
    mp_mul_asm(a, b, c); }

// Montgomery form reduction after multiplication
inline void mont_redc(const digit_t *a, digit_t *c){
    mont_redc_asm(a, c);}

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
    f_elm_t t[5];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* p - 2 =     0b   11111111 11111111\
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

                        11111111 00010001\
    */

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 2) f_copy(t[0], t[2]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
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

    // Next 8 bits
    // bit = 7
    f_mul(t[0], t[0], t[0]);
    // bit = 6
    f_mul(t[0], t[0], t[0]);
    // bit = 5
    f_mul(t[0], t[0], t[0]);
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

    /* (p - 1)/2 = 0b   0 1111111 11111111\
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
                        11111111 1 0001001\
    */   

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 2) f_copy(t[0], t[2]);  // t[2] = a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // t[3] = a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // t[4] = a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
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
    // bit = 4
    f_mul(t[0], t[0], t[0]);
    // bit = 3
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // bit = 2
    f_mul(t[0], t[0], t[0]);
    // bit = 1
    f_mul(t[0], t[0], t[0]);
    // bit = 0
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    *b = ((*(unsigned char *)(t[0])) & 0x02) >> 1;

}

// Multiplicative inverse of a field element
void f_sqrt(const f_elm_t a, f_elm_t b)
{
    f_elm_t t[5];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p + 1)/4 = 0b   00 111111 11111111\
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

                        11111111 11 000101\
    */

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 2) f_copy(t[0], t[2]);  // a^(2^4  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[4] = a ^ 0b 11111111 11111111
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

    // Next 6 bits
    // Bit = 5
    f_mul(t[0], t[0], t[0]);
    // Bit = 4
    f_mul(t[0], t[0], t[0]);
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

#elif (PRIMES == ALT)

// Multiplicative inverse of a field element
void f_inv(const f_elm_t a, f_elm_t b)
{
    f_elm_t t[6];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* p - 2 =     0b   01111111 11111111\
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
                        
                        11111111 1 1101011
    */

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[5]);  // a^(2^2  - 1) = a^0b 11
        if(j == 2) f_copy(t[0], t[2]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[4] = a ^ 0b 11111111 11111111
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

    // Next 7 bits
    // Bit = 6, 5
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);
    // Bit = 4
    f_mul(t[0], t[0], t[0]);
    // Bit = 3
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // Bit = 2
    f_mul(t[0], t[0], t[0]);
    // Bits = 1, 0
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);

    f_copy(t[0], b);

}


// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    f_elm_t t[6];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p - 1)/2 = 0b   00111111 11111111\
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
                        
                        11111111 11 110110
    */

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[5]);  // a^(2^2  - 1) = a^0b 11
        if(j == 2) f_copy(t[0], t[2]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[4] = a ^ 0b 11111111 11111111
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

    // Bits = 5, 4
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);
    // Bit = 3
    f_mul(t[0], t[0], t[0]);
    // Bits = 2, 1
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);
    // Bit = 0
    f_mul(t[0], t[0], t[0]);

    *b = ((*(unsigned char *)(t[0])) & 0x01);

}



// Multiplicative inverse of a field element
void f_sqrt(const f_elm_t a, f_elm_t b)
{
    f_elm_t t[6];
    f_elm_t psi = {0x20CB992113610E18, 0xBFA6E4AC2CD1AFC4, 0x4B68552BFAA9C84A};
    unsigned int i, j;
    digit_t mask = 0;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* d = (p - 1)/4 = 0b   000 11111 11111111\
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

                            11111111 111 11 011
    */

    /* (d + 1)/2     = 0b   0000 1111 11111111\
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

                            11111111 1111 11 10
    */

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[5]);  // a^(2^2  - 1) = a^0b 11
        if(j == 2) f_copy(t[0], t[2]);  // a^(2^4  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[4]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[4] = a ^ 0b 11111111 11111111
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

    // Next 2 bits
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[5], t[0]);

    // Next 1 bit
    f_mul(t[0], t[0], t[0]);


    // t[1] = a^d
    f_mul(t[0], t[0], t[1]);
    f_mul(t[1], t[1], t[1]);
    f_mul(t[1], t[5], t[1]);


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


#endif