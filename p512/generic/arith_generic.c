#include "../../arith.h"


// Reduction modulo p
// a in [0, 2 * p - 1] -> a in [0, p-1]
void f_red(f_elm_t a)
{
    digit_t mask, borrow = 0, carry = 0;
    


    #if (PRIMES == ALT)

        mask = 0 - (a[WORDS_FIELD - 1] >> 63);
        a[WORDS_FIELD - 1] &= 0x7FFFFFFFFFFFFFFF;

        for (int i = 0; i < 5; i++)
            ADDC(carry, a[i], mask, a[i]);

        mask = 0;
        for (int i = 5; i < WORDS_FIELD; i++)
            ADDC(carry, a[i], mask, a[i]);

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

// // Montgomery form reduction after multiplication
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

    f_elm_t t[8];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);
 
    /* p - 2 =     0b   11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111110

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111101    
    */


    // Compute a^(p-2)

    // First 256 bits = 2^8 bits
    for(j = 0; j < 8; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[2]);  // a^(2^2  - 1) = a^0b 11
        if(j == 1) f_copy(t[0], t[3]);  // a^(2^4  - 1) = a^0b 1111
        if(j == 2) f_copy(t[0], t[4]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[5]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[6]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        if(j == 5) f_copy(t[0], t[7]);  // a^(2^64 - 1) = a^0b 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }



    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);



    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[6], t[0]);

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
    for (i = 0; i < 1; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Next 1 bit
    for (i = 0; i < 1; i++)
        f_mul(t[0], t[0], t[0]);



    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[7], t[0]);





    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[6], t[0]);

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
    for (i = 0; i < 1; i++)
        f_mul(t[0], t[0], t[0]);

    // Next 1 bit
    for (i = 0; i < 1; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);



    f_copy(t[0], b);  

}


// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    f_elm_t t[8];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p - 1)/2 = 0b   01111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        10000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000

                        01111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        01111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111  
    */


    // Compute a^((p - 1)/2)

    // First 256 bits = 2^8 bits
    for(j = 0; j < 8; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[2]);  // a^(2^2  - 1) = a^0b 11
        if(j == 1) f_copy(t[0], t[3]);  // a^(2^4  - 1) = a^0b 1111
        if(j == 2) f_copy(t[0], t[4]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[5]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[6]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        if(j == 5) f_copy(t[0], t[7]);  // a^(2^64 - 1) = a^0b 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }



    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);



    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[6], t[0]);

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
    for (i = 0; i < 1; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Next 1 bit
    for (i = 0; i < 1; i++)
        f_mul(t[0], t[0], t[0]);



    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[7], t[0]);





    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[6], t[0]);

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
    for (i = 0; i < 1; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    *b = ((*(unsigned char *)t[0]) & 0x02) >> 0x01;

}


void f_sqrt(const f_elm_t a, f_elm_t b){


    f_elm_t t[3];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p + 1)/4 = 0b   00111111111111111111111111111111
                        11111111111111111111111111111111
                        11111111111111111111111111111111
                        11111111111111111111111111111111

                        11111111111111111111111111111111
                        11111111111111111111111111111111
                        11111111111111111111111111111111
                        11111111111111111111111111111111

                        11000000000000000000000000000000
                        00000000000000000000000000000000

                        00111111111111111111111111111111
                        11111111111111111111111111111111

                        11000000000000000000000000000000
                        00000000000000000000000000000000
                        00000000000000000000000000000000
                        00000000000000000000000000000000  
    */


    // Compute a^((p - 1)/2)

    // First 256 bits = 2^8 bits
    for(j = 0; j < 8; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 5) f_copy(t[0], t[2]);  // a^(2^64 - 1) = a^0b 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }


    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);

    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);

    // Next 126 bits
    for (i = 0; i < 126; i++)
        f_mul(t[0], t[0], t[0]);


    f_copy(t[0], b);

}





#elif (PRIMES == ALT)



// Multiplicative inverse of a field element
void f_inv(const f_elm_t a, f_elm_t b)
{

    f_elm_t t[7];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);
 
    /* p - 2 =     0b   01111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111110

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111\
    */

    // Compute a^(p-2)

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 1) f_copy(t[0], t[2]);  // a^(2^4  - 1) = a^0b 1111
        if(j == 2) f_copy(t[0], t[3]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[4]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[5]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        if(j == 5) f_copy(t[0], t[6]);  // a^(2^64 - 1) = a^0b 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }

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

    // Next 4 bits
    for (i = 0; i < 4; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[2], t[0]);

    // Next 2 bits
    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    f_mul(t[0], t[0], t[0]);
    f_mul(t[0], a, t[0]);

    // Next 1 bit
    f_mul(t[0], t[0], t[0]);

    // Next 128 bits
    for (i = 0; i < 128; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[1], t[0]);

    // Next 128 bits
    for (i = 0; i < 128; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[1], t[0]);

    // Next 64 bits
    for (i = 0; i < 64; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[6], t[0]);


    f_copy(t[0], b);  

}




// Legendre symbol of a field element
void f_leg(const f_elm_t a, unsigned char *b)
{

    f_elm_t t[7];
    unsigned int i, j;

    f_copy(a, t[0]);
    f_copy(a, t[1]);

    /* (p - 1)/2 = 0b   00111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        10000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
    */

    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            f_mul(t[0], t[0], t[0]);
        f_mul(t[0], t[1], t[0]);
        if(j == 0) f_copy(t[0], t[2]);  // a^(2^2  - 1) = a^0b 11
        if(j == 1) f_copy(t[0], t[3]);  // a^(2^4  - 1) = a^0b 1111
        if(j == 2) f_copy(t[0], t[4]);  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3) f_copy(t[0], t[5]);  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4) f_copy(t[0], t[6]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
        f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
    }

    // Next 32 bits
    for (i = 0; i < 32; i++)
        f_mul(t[0], t[0], t[0]);
    f_mul(t[0], t[6], t[0]);

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

    // Next 319 bits
    for (i = 0; i < 319; i++)
        f_mul(t[0], t[0], t[0]);

    *b = ((*(unsigned char *)t[0]) & 0x01);

}




// a = a0 + a1*a;   a[0] = a0, a[1] = a1, a[2] = a1^2 * w
// b = a^2 mod x^2 - w
void fp_2_sqr(const f_elm_t* a, f_elm_t* b){

    f_elm_t t0, t1, t2;

    f_mul(a[0], a[0], t0);  // t0 = a0^2
    f_mul(a[0], a[1], t1);  // t1 = a0 * a1
    f_mul(t0, a[2], t2);    // t2 = a0^2 * a1^2 * w
    

    f_add(t0, a[2], b[0]);  // b[0] = a0^2 + a1^2 * w

    f_add(t1, t1, b[1]);    // b[1] = 2 * a0 * a1

    f_add(t2, t2, t2);
    f_add(t2, t2, b[2]);    // b[2] = 4 * a0^2 * a1^2 * w

}


// a = a[0] + a[1] * x
// b = b[0] + b[1] * x
// Modulus = x^2 - w
// c = a * b = (a0b0 + a1b1 w) + (a0b1 + a1b0) * x
void fp_2_mul(const f_elm_t* a, const f_elm_t* b, f_elm_t* c, const f_elm_t w){
    
    f_elm_t t0, t1, t2;

    f_add(a[0], a[1], t0);
    f_add(b[0], b[1], t1);

    f_mul(t0, t1, t2);

    f_mul(a[0], b[0], t0);
    f_mul(a[1], b[1], t1);

    f_sub(t2, t0, t2);
    f_sub(t2, t1, c[1]);
    
    f_mul(t1, w, t1);

    f_add(t0, t1, c[0]);

    f_mul(c[1], c[1], c[2]);
    f_mul(c[2], w, c[2]);

}



// // a = a[0] + a[1] * x
// // b = b[0] + b[1] * x
// // Modulus = x^2 - w
// // c = a * b = (a0b0 + a1b1 w) + (a0b1 + a1b0) * x
// void fp_2_mul(const f_elm_t* a, const f_elm_t* b, f_elm_t* c, const f_elm_t w){
    
//     f_elm_t t0, t1, t2;

//     f_add(a[0], a[1], t0);
//     f_add(b[0], b[1], t1);

//     f_mul(t0, t1, t2);

//     f_mul(a[0], b[0], t0);
//     f_mul(a[1], b[1], t1);

//     f_sub(t2, t0, t2);
//     f_sub(t2, t1, c[1]);
    
//     f_mul(t1, w, t1);

//     f_add(t0, t1, c[0]);

// }


// NOT CONSTANT TIME
// BUT INPUT INDEPENDENT
// Pocklington algorithm
// Choose a random r so that c = r^2 - a is a non-square
// Given that r is random, algorithm doesn't depend on a, but the run time is still not constant
void f_sqrt(const f_elm_t a, f_elm_t b){

    f_elm_t t[8][3] = {0};
    f_elm_t a_temp[2];
    f_elm_t w;

    int i, j;
    unsigned char s[1];


    // IF a = 0 the algorithm doesn't work. 
    // In that case set a = 1, and return 0 only at the end.
    cond_select(a, Mont_one, a_temp[0], f_eq(a, Zero));
    f_copy(Zero, a_temp[1]);


    do {// w = r^2 - a  AND non-square
        f_rand(t[0][0]);                    // r
        f_mul(t[0][0], t[0][0], w);         // r^2
        f_sub(w, a_temp[0], w);             // w = r^2 - a
        f_leg(w, s);
    } while (1 - *s);


    // t[0] = r - x; where w = r^2 - a.
    // t[0] = t0 + t1 * x
    // t[0] is represented as a triple; 
    // t[0] = (t0, t1, t1^2 * w)
    f_neg(Mont_one, t[0][1]);
    f_copy(w, t[0][2]);

    // t[1] = t[0]
    f_copy(t[0][0], t[1][0]);
    f_copy(t[0][1], t[1][1]);

    // t[7] = t[0]
    f_copy(t[0][0], t[7][0]);
    f_copy(t[0][1], t[7][1]);


    /* (p + 1)/2 = 0b   00111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        11111111 11111111 11111111 11111111
                        11111111 11111111 11111111 11111111

                        10000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000

                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000000

                        00000000 00000000 00000000 00000000
                        00000000 00000000 00000000 00000001
    */

    // (r - x)^(p + 1)/2 = sqrt(a) + 0 * x


    // First 128 bits = 2^7 bits
    for(j = 0; j < 7; j++){
        for (i = 0; i < (1 << j); i++)
            fp_2_sqr(t[0], t[0]);
        fp_2_mul(t[0], t[1], t[0], w);
        if(j == 0)  {f_copy(t[0][0], t[2][0]); f_copy(t[0][1], t[2][1]); }  // a^(2^2  - 1) = a^0b 11
        if(j == 1)  {f_copy(t[0][0], t[3][0]); f_copy(t[0][1], t[3][1]); }  // a^(2^4  - 1) = a^0b 1111
        if(j == 2)  {f_copy(t[0][0], t[4][0]); f_copy(t[0][1], t[4][1]); }  // a^(2^8  - 1) = a^0b 11111111
        if(j == 3)  {f_copy(t[0][0], t[5][0]); f_copy(t[0][1], t[5][1]); }  // a^(2^16 - 1) = a^0b 11111111 11111111
        if(j == 4)  {f_copy(t[0][0], t[6][0]); f_copy(t[0][1], t[6][1]); }  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
                    {f_copy(t[0][0], t[1][0]); f_copy(t[0][1], t[1][1]); }  // a^(2^(2^(j+1)) - 1)
    }

    // Next 32 bits
    for (i = 0; i < 32; i++)
        fp_2_sqr(t[0], t[0]);
    fp_2_mul(t[0], t[6], t[0], w);

    // Next 16 bits
    for (i = 0; i < 16; i++)
        fp_2_sqr(t[0], t[0]);
    fp_2_mul(t[0], t[5], t[0], w);

    // Next 8 bits
    for (i = 0; i < 8; i++)
        fp_2_sqr(t[0], t[0]);
    fp_2_mul(t[0], t[4], t[0], w);

    // Next 4 bits
    for (i = 0; i < 4; i++)
        fp_2_sqr(t[0], t[0]);
    fp_2_mul(t[0], t[3], t[0], w);

    // Next 2 bits
    for (i = 0; i < 2; i++)
        fp_2_sqr(t[0], t[0]);
    fp_2_mul(t[0], t[2], t[0], w);

    // Next 1 bit
        fp_2_sqr(t[0], t[0]);
    fp_2_mul(t[0], t[7], t[0], w);

    // Next 319 bits
    for(i = 0; i < 319; i++)
        fp_2_sqr(t[0], t[0]);
    fp_2_mul(t[0], t[7], t[0], w);


    cond_select(t[0][0], a, b, f_eq(a, Zero));

}


// Constant time but slow
// Can be sped up with more memory, but stays much slower than above algorithm
// This is Tonelli-Shanks
// void f_sqrt(const f_elm_t a, f_elm_t b){

//     // psi = 2^320'th root of unity. psi^(2^320) = 1, psi^(2^319) = -1
//     f_elm_t psi = {0x866F1CC18DC46260, 0xBC6920F07BAAE79F, 0x5AFFB5FF5381606B, 0xBD8EE7B3BE6830E1, 0xA728C86D8ADE7CA2, 0x35903E99C2D6B5D4, 0x4D6556226F7CBA34, 0x001B66972B030BDF};
//     f_elm_t psi_old;

//     f_elm_t t[7];
//     int i, j;

//     f_copy(a, t[0]);
//     f_copy(a, t[1]);

//     /* d = (p - 1)/(2^320) 0b   01111111111111111111111111111111
//                                 11111111111111111111111111111111
//                                 11111111111111111111111111111111
//                                 11111111111111111111111111111111

//                                 11111111111111111111111111111111
//                                 11111111111111111111111111111111
//     */

//     /* (d + 1)/2 0b     01000000000000000000000000000000
//                         00000000000000000000000000000000
//                         00000000000000000000000000000000
//                         00000000000000000000000000000000

//                         00000000000000000000000000000000
//                         00000000000000000000000000000000
//     */



//     // First 128 bits = 2^7 bits
//     for(j = 0; j < 7; j++){
//         for (i = 0; i < (1 << j); i++)
//             f_mul(t[0], t[0], t[0]);
//         f_mul(t[0], t[1], t[0]);
//         if(j == 0) f_copy(t[0], t[2]);  // a^(2^2  - 1) = a^0b 11
//         if(j == 1) f_copy(t[0], t[3]);  // a^(2^4  - 1) = a^0b 1111
//         if(j == 2) f_copy(t[0], t[4]);  // a^(2^8  - 1) = a^0b 11111111
//         if(j == 3) f_copy(t[0], t[5]);  // a^(2^16 - 1) = a^0b 11111111 11111111
//         if(j == 4) f_copy(t[0], t[6]);  // a^(2^32 - 1) = a^0b 11111111 11111111 11111111 11111111
//         f_copy(t[0], t[1]);             // a^(2^(2^(j+1)) - 1)
//     }

//     // Next 32 bits
//     for (i = 0; i < 32; i++)
//         f_mul(t[0], t[0], t[0]);
//     f_mul(t[0], t[6], t[0]);

//     // Next 16 bits
//     for (i = 0; i < 16; i++)
//         f_mul(t[0], t[0], t[0]);
//     f_mul(t[0], t[5], t[0]);

//     // Next 8 bits
//     for (i = 0; i < 8; i++)
//         f_mul(t[0], t[0], t[0]);
//     f_mul(t[0], t[4], t[0]);

//     // Next 4 bits
//     for (i = 0; i < 4; i++)
//         f_mul(t[0], t[0], t[0]);
//     f_mul(t[0], t[3], t[0]);

//     // Next 2 bits
//     for (i = 0; i < 2; i++)
//         f_mul(t[0], t[0], t[0]);
//     f_mul(t[0], t[2], t[0]);

//     // Next 1 bit
//     f_mul(t[0], t[0], t[0]);
//     f_mul(t[0], a, t[0]);

//     // t[0] = a^d

//     // t[1] = a^((d+1)/2)
//     f_copy(a, t[1]);
//     for(i = 0; i < 190; i++)
//         f_mul(t[1], t[1], t[1]);


//     for(i = 318; i >= 0; i--){

//         f_copy(psi, psi_old);
//         f_mul(psi, psi, psi);


//         f_copy(t[0], t[2]);
//         for(j = 0; j < i; j++)
//             f_mul(t[2], t[2], t[2]);

//         cond_select(Mont_one, psi_old, t[3], f_neq(t[2], Mont_one));
//         cond_select(Mont_one, psi,     t[4], f_neq(t[2], Mont_one));

//         f_mul(t[1], t[3], t[1]);
//         f_mul(t[0], t[4], t[0]);

//     }

//     f_copy(t[1], b);

// }




#endif