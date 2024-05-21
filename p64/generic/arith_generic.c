#include "../../arith.h"


// Reduction modulo p
// a in [0, R) -> a in [0, p-1]
void f_red(f_elm_t a)
{
    digit_t mask, borrow = 0, carry = 0;

    #if (PRIMES == ORIGINAL)
        f_elm_t t0 = {0};
        
        t0[0] = a[0] >> 61;
        a[0] &= 0x1FFFFFFFFFFFFFFF;

        for (int i = 0; i < WORDS_FIELD; i++)
            ADDC(carry, a[i], t0[i], a[i]);

    #endif

    for (int i = 0; i < WORDS_FIELD; i++)
    {
        SUBC(borrow, a[i], p[i], a[i]);
    }

    mask = 0 - borrow;

    for (int i = 0; i < WORDS_FIELD; i++)
        ADDC(carry, a[i], p[i] & mask, a[i]);

    // Alternative reduction
    // digit_t mask, borrow = 0, carry = 0;
    // for (int i = 0; i < WORDS_FIELD; i++)
    // {
    //     ADDC(borrow, a[i], Mont_one[i], a[i]);
    // }

    // mask = 0 - (1-borrow);

    // for (int i = 0; i < WORDS_FIELD; i++)
    //     SUBC(carry, a[i], Mont_one[i] & mask, a[i]);
}

// Addition of two field elements
void f_add(const f_elm_t a, const f_elm_t b, f_elm_t c)
{
    digit_t mask, carry = 0;

    for (int i = 0; i < WORDS_FIELD; i++)
        ADDC(carry, a[i], b[i], c[i]);

    mask = 0 - carry;
    carry = 0;
    for (int i = 0; i < WORDS_FIELD; i++)
        ADDC(carry, c[i], Mont_one[i] & mask, c[i]);

    f_red(c);
}

// Subtraction of two field elements
void f_sub(const f_elm_t a, const f_elm_t b, f_elm_t c)
{
    digit_t mask, borrow = 0, carry = 0;

    for (int i = 0; i < WORDS_FIELD; i++)
        SUBC(borrow, a[i], b[i], c[i]);

    mask = 0 - borrow;

    for (int i = 0; i < WORDS_FIELD; i++)
        SUBC(carry, c[i], Mont_one[i] & mask, c[i])

    f_red(c);
}

// Negation of a field element
void f_neg(const f_elm_t a, f_elm_t b)
{
    digit_t borrow = 0;

    for (int i = 0; i < WORDS_FIELD; i++)
        SUBC(borrow, p[i], a[i], b[i]);

    f_red(b);
}

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

// 2.3 times slower than below !
// void mont_redc(const digit_t *a, digit_t *c)
// {
//     // c = a*R^-1 mod p, where R = 2^256.
//     // If a < 2^256*p, the output c is in the range [0, p).
//     // a is assumed to be in Montgomery representation.
//     digit_t mask, carry = 0;
//     digit_t t0[2 * WORDS_FIELD], t1[WORDS_FIELD];

//     for (int i = 0; i < WORDS_FIELD; i++)
//         c[i] = a[i];

//     mp_mul(c, pp, t0);
//     f_copy(t0, t1);
//     mp_mul(t1, p, t0);

//     for (int i = 0; i < 2 * WORDS_FIELD; i++)
//         ADDC(carry, t0[i], a[i], t0[i]);

//     f_copy((digit_t *)&t0[WORDS_FIELD], c);
//     f_red(c);

//     mask = 0 - carry;
//     carry = 0;
//     for (int i = 0; i < WORDS_FIELD; i++)
//         ADDC(carry, c[i], Mont_one[i] & mask, c[i]);

//     f_red(c);
// }

// Montgomery form reduction after multiplication
void mont_redc(const digit_t *a, digit_t *c)
{
    // c = a*R^-1 mod p
    // If a < *p, the output c is in the range [0, p).
    // a is assumed to be in Montgomery representation.
    digit_t mask, carry = 0;
    digit_t t0[2 * WORDS_FIELD];//, t1[WORDS_FIELD];
    // not sure why this works, but it's faster

    mp_mul(a, ip, t0);
    // f_copy(t0, t1); // works without this step because WORDS_FIELD == 1
    mp_mul(t0, p, t0);

    for (int i = WORDS_FIELD; i < 2 * WORDS_FIELD; i++)
        SUBC(carry, a[i], t0[i], t0[i]);

    mask = 0 - carry;
    carry = 0;
    for (int i = 0; i < WORDS_FIELD; i++)
        ADDC(carry, t0[WORDS_FIELD + i], p[i] & mask, c[i]);

}


// Multiplication of field elements
void f_mul(const f_elm_t a, const f_elm_t b, f_elm_t c)
{
    digit_t t0[2 * WORDS_FIELD] = {0};

    mp_mul(a, b, t0);
    mont_redc(t0, c);
}



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

    f_elm_t t[4];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* p - 2 =  0b  000 11111 11111111\
                    11111111 11111111\

                    11111111 11111111\

                    111 11111111 11101\
    */


    // First 32 bits = 2^5 bits
    for(j = 0; j < 5; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 2) f_copy(t[0], t[2]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[3] = a ^ 0b 11111111 11111111
    */

    // Next 16 bits
    for (i = 0; i < 16; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[3], t[0]);

    // Next 8 bits
    for (i = 0; i < 8; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);

    // Bit 4
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0],    a, t[0]);

    // Bit 3
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0],    a, t[0]);
    
    // Bit 2
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0],    a, t[0]);
    
    // Bit 1
    f_mul(t[0], t[0], t[0]);

    // Bit 0
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0],    a, t[0]);

    f_copy(t[0], b);

}


// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    f_elm_t t[5];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p - 1)/2 = 0b   00001111 11111111\
                        11111111 11111111\

                        11111111 11111111\

                        1111 1111 1111 1111
    */


    // bit = 64 (=0)

    // First 32 bits = 2^5 bits
    for(j = 0; j < 5; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 1) f_copy(t[0], t[2]);  // a^(2^4  - 1) = a^0b 1111
        if(j == 2) f_copy(t[0], t[3]);  // a^(2^8 - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[4]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[4] = a ^ 0b 11111111 11111111
    */

    // Next 16 bits
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[4], t[0]);

    // Next 8 bits
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[3], t[0]);

    // Next 4 bits
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);


    *b = ((*(unsigned char *)t[0]) & 0x01);

}



// Legendre symbol of a field element
void f_sqrt(const f_elm_t a, f_elm_t b)
{

    /* (p + 1)/4 = 0b   0000100000000000\
                        0000000000000000\
                        0000000000000000\
                        0000000000000000
        =2^59 = (2^61 - 1 + 1)/4
    */

    f_mul(a, a, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);
    f_mul(b, b, b);

}

#elif (PRIMES == ALT)

// Multiplicative inverse of a field element
void f_inv(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[5];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* p - 2 =  0b  11111111 11111111\
                    11111111 11111111\

                    11111111 11111111\

                    11111111 11 0000 11\
    */


    // First 32 bits = 2^5 bits
    for(j = 0; j < 5; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[2]);  // a^(2^2  - 1) = a^0b 11
        if(j == 2) f_copy(t[0], t[3]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[4]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[3] = a ^ 0b 11111111 11111111
    */

    // Next 16 bits
    for (i = 0; i < 16; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[4], t[0]);

    // Next 8 bits
    for (i = 0; i < 8; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[3], t[0]);

    // bit = 7, 6
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);
    // Next 4 bits
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    // bit = 1, 0
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);

    f_copy(t[0], b);

}


// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    f_elm_t t[4];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p - 1)/2 = 0b   0 1111111 11111111\
                        11111111 11111111\

                        11111111 11111111\

                        11111111 1 1100010
    */


    // bit = 64 (=0)

    // First 32 bits = 2^5 bits
    for(j = 0; j < 5; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 2) f_copy(t[0], t[2]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[3] = a ^ 0b 11111111 11111111
    */

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
    // bit = 2
    f_mul(t[0], t[0], t[0]);
    // bit = 1
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);
    // bit = 0
    f_mul(t[0], t[0], t[0]);

    *b = ((*(unsigned char *)t[0]) & 0x80) >> 7;

}




// Square root of a field element
void f_sqrt(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[4];
    f_elm_t psi = {0x57F56382B3D1DEF4}; // psi = 4'th root of 1 = i, psi^2 = -1, psi^4 = 1 (1 being Mont_one)

    unsigned int i, j;
    digit_t mask = 0;

    f_copy(a, t[0]);
    f_copy(a, t[1]);


    /* d = (p - 1)/4 0b 00 111111 11111111\
                        11111111 11111111\

                        11111111 11111111\

                        11111111 11 110001
    */

    /* (d+1)/2 = 0b     000 11111 11111111\
                        11111111 11111111\

                        11111111 11111111\

                        11111111 111 11001
    */

    // First 32 bits = 2^5 bits
    for(j = 0; j < 5; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 2) f_copy(t[0], t[2]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[3]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }
    
    /* t[3] = a ^ 0b 11111111 11111111
    */

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
    f_mul(t[0], a, t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Next 3 bits
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[0], t[0]);


    f_mul(t[0], t[0], t[1]);
    f_mul(t[1], a, t[1]);       // t[1] = a^(d)


    f_mul(t[0], a, t[0]);       // t[0] = a^((d+1)/2)

    // t[0] has to be multiplied with 1 if t[1] is 1, and with psi if t[1] is -1
    mask = 0 - (((digit_t) f_neq(t[1], Mont_one)) & 0x01);
    
    psi[0] = (psi[0] ^ Mont_one[0]) & mask;
    psi[0] ^= Mont_one[0];

    f_mul(t[0], psi, t[0]);

    f_copy(t[0], b);
}


#endif