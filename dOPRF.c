#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "bench.h"
#include "arith.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BIT_TO_STR(bit)     ((bit) ? '1' : '0')
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 



#define PASS(x)                 ((x) ? "\033[31m✗\033[0m" : "\033[0;32m✔\033[0m")

#define TESTS_PAD 5

const char* pass_check(unsigned char arr[], int n) {
    static char buffer[256] = "";
    buffer[0] = '\0';
    for(int i = 0; i < TESTS_PAD - n; i++)
        strcat(buffer, " ");
    for(int i = 0; i < n; i++)
        strcat(buffer, PASS(arr[i]));
    return buffer;
}


#define REMOVE_BIT(x, i)        (((x) & MASK(0, i)) | (((x) & MASK(i+1, 63)) >> 1))
#define INSERT_BIT(x, b, i)     (((x) & MASK(0, i)) | (((x) & MASK(i, 63)) << 1) | ((b) << (i)))
#define i_hold_T(i, T)          (!((T) & (1 << (i))))


#define FACTORIAL_TABLE(i)   (int64_t)( (i ==  0) ? 1 : \
                                        (i ==  1) ? 1 : \
                                        (i ==  2) ? 2 : \
                                        (i ==  3) ? 6 : \
                                        (i ==  4) ? 24 : \
                                        (i ==  5) ? 120 : \
                                        (i ==  6) ? 720 : \
                                        (i ==  7) ? 5040 : \
                                        (i ==  8) ? 40320 : \
                                        (i ==  9) ? 362880 : \
                                        (i == 10) ? 3628800 : \
                                        (i == 11) ? 39916800 : \
                                        (i == 12) ? 479001600 : \
                                        (i == 13) ? 6227020800 : \
                                        (i == 14) ? 87178291200 : \
                                        (i == 15) ? 1307674368000 : \
                                        (i == 16) ? 20922789888000 : \
                                        (i == 17) ? 355687428096000 : \
                                        (i == 18) ? 6402373705728000 : \
                                        (i == 19) ? 121645100408832000 : \
                                        (i == 20) ? 2432902008176640000 : -1)

int factorial(unsigned int n){
    return FACTORIAL_TABLE(n);
}




#define FIRST_COMB_0    {}
#define FIRST_COMB_1    {0}
#define FIRST_COMB_2    {0, 1}
#define FIRST_COMB_3    {0, 1, 2}
#define FIRST_COMB_4    {0, 1, 2, 3}
#define FIRST_COMB_5    {0, 1, 2, 3, 4}
#define FIRST_COMB_6    {0, 1, 2, 3, 4, 5}
#define FIRST_COMB_7    {0, 1, 2, 3, 4, 5, 6}
#define FIRST_COMB_8    {0, 1, 2, 3, 4, 5, 6, 7}
#define FIRST_COMB_9    {0, 1, 2, 3, 4, 5, 6, 7, 8}
#define FIRST_COMB_10   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
#define FIRST_COMB_11   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
#define FIRST_COMB_12   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
#define FIRST_COMB_13   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}
#define FIRST_COMB_14   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}
#define FIRST_COMB_15   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}
#define FIRST_COMB_16   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
#define FIRST_COMB_17   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}
#define FIRST_COMB_18   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}
#define FIRST_COMB_19   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18}
#define FIRST_COMB_20   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}


#define CONCAT_NAMES(A, B)      A ## B
#define CONCAT(A, B)            CONCAT_NAMES(A, B)
#define FIRST_COMB(i)           CONCAT(FIRST_COMB_,i)


#define BINOMIAL_TABLE(i, j)    (\
                                (i ==  0) ? ((j == 0) ? 1 : (j == 1) ?  0 : (j == 2) ?   0 : (j == 3) ?   0 : (j == 4) ?    0 : (j == 5) ?     0 : (j == 6) ?     0 : (j == 7) ?     0 : (j == 8) ?     0 : (j == 9) ?     0 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i ==  1) ? ((j == 0) ? 1 : (j == 1) ?  1 : (j == 2) ?   0 : (j == 3) ?   0 : (j == 4) ?    0 : (j == 5) ?     0 : (j == 6) ?     0 : (j == 7) ?     0 : (j == 8) ?     0 : (j == 9) ?     0 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i ==  2) ? ((j == 0) ? 1 : (j == 1) ?  2 : (j == 2) ?   1 : (j == 3) ?   0 : (j == 4) ?    0 : (j == 5) ?     0 : (j == 6) ?     0 : (j == 7) ?     0 : (j == 8) ?     0 : (j == 9) ?     0 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i ==  3) ? ((j == 0) ? 1 : (j == 1) ?  3 : (j == 2) ?   3 : (j == 3) ?   1 : (j == 4) ?    0 : (j == 5) ?     0 : (j == 6) ?     0 : (j == 7) ?     0 : (j == 8) ?     0 : (j == 9) ?     0 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i ==  4) ? ((j == 0) ? 1 : (j == 1) ?  4 : (j == 2) ?   6 : (j == 3) ?   4 : (j == 4) ?    1 : (j == 5) ?     0 : (j == 6) ?     0 : (j == 7) ?     0 : (j == 8) ?     0 : (j == 9) ?     0 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i ==  5) ? ((j == 0) ? 1 : (j == 1) ?  5 : (j == 2) ?  10 : (j == 3) ?  10 : (j == 4) ?    5 : (j == 5) ?     1 : (j == 6) ?     0 : (j == 7) ?     0 : (j == 8) ?     0 : (j == 9) ?     0 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i ==  6) ? ((j == 0) ? 1 : (j == 1) ?  6 : (j == 2) ?  15 : (j == 3) ?  20 : (j == 4) ?   15 : (j == 5) ?     6 : (j == 6) ?     1 : (j == 7) ?     0 : (j == 8) ?     0 : (j == 9) ?     0 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i ==  7) ? ((j == 0) ? 1 : (j == 1) ?  7 : (j == 2) ?  21 : (j == 3) ?  35 : (j == 4) ?   35 : (j == 5) ?    21 : (j == 6) ?     7 : (j == 7) ?     1 : (j == 8) ?     0 : (j == 9) ?     0 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i ==  8) ? ((j == 0) ? 1 : (j == 1) ?  8 : (j == 2) ?  28 : (j == 3) ?  56 : (j == 4) ?   70 : (j == 5) ?    56 : (j == 6) ?    28 : (j == 7) ?     8 : (j == 8) ?     1 : (j == 9) ?     0 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i ==  9) ? ((j == 0) ? 1 : (j == 1) ?  9 : (j == 2) ?  36 : (j == 3) ?  84 : (j == 4) ?  126 : (j == 5) ?   126 : (j == 6) ?    84 : (j == 7) ?    36 : (j == 8) ?     9 : (j == 9) ?     1 : (j == 10) ?     0 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i == 10) ? ((j == 0) ? 1 : (j == 1) ? 10 : (j == 2) ?  45 : (j == 3) ? 120 : (j == 4) ?  210 : (j == 5) ?   252 : (j == 6) ?   210 : (j == 7) ?   120 : (j == 8) ?    45 : (j == 9) ?    10 : (j == 10) ?     1 : (j == 11) ?     0 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i == 11) ? ((j == 0) ? 1 : (j == 1) ? 11 : (j == 2) ?  55 : (j == 3) ? 165 : (j == 4) ?  330 : (j == 5) ?   462 : (j == 6) ?   462 : (j == 7) ?   330 : (j == 8) ?   165 : (j == 9) ?    55 : (j == 10) ?    11 : (j == 11) ?     1 : (j == 12) ?     0 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i == 12) ? ((j == 0) ? 1 : (j == 1) ? 12 : (j == 2) ?  66 : (j == 3) ? 220 : (j == 4) ?  495 : (j == 5) ?   792 : (j == 6) ?   924 : (j == 7) ?   792 : (j == 8) ?   495 : (j == 9) ?   220 : (j == 10) ?    66 : (j == 11) ?    12 : (j == 12) ?     1 : (j == 13) ?     0 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i == 13) ? ((j == 0) ? 1 : (j == 1) ? 13 : (j == 2) ?  78 : (j == 3) ? 286 : (j == 4) ?  715 : (j == 5) ?  1287 : (j == 6) ?  1716 : (j == 7) ?  1716 : (j == 8) ?  1287 : (j == 9) ?   715 : (j == 10) ?   286 : (j == 11) ?    78 : (j == 12) ?    13 : (j == 13) ?     1 : (j == 14) ?     0 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i == 14) ? ((j == 0) ? 1 : (j == 1) ? 14 : (j == 2) ?  91 : (j == 3) ? 364 : (j == 4) ? 1001 : (j == 5) ?  2002 : (j == 6) ?  3003 : (j == 7) ?  3432 : (j == 8) ?  3003 : (j == 9) ?  2002 : (j == 10) ?  1001 : (j == 11) ?   364 : (j == 12) ?    91 : (j == 13) ?    14 : (j == 14) ?     1 : (j == 15) ?    0 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i == 15) ? ((j == 0) ? 1 : (j == 1) ? 15 : (j == 2) ? 105 : (j == 3) ? 455 : (j == 4) ? 1365 : (j == 5) ?  3003 : (j == 6) ?  5005 : (j == 7) ?  6435 : (j == 8) ?  6435 : (j == 9) ?  5005 : (j == 10) ?  3003 : (j == 11) ?  1365 : (j == 12) ?   455 : (j == 13) ?   105 : (j == 14) ?    15 : (j == 15) ?    1 : (j == 16) ?   0 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i == 16) ? ((j == 0) ? 1 : (j == 1) ? 16 : (j == 2) ? 120 : (j == 3) ? 560 : (j == 4) ? 1820 : (j == 5) ?  4368 : (j == 6) ?  8008 : (j == 7) ? 11440 : (j == 8) ? 12870 : (j == 9) ? 11440 : (j == 10) ?  8008 : (j == 11) ?  4368 : (j == 12) ?  1820 : (j == 13) ?   560 : (j == 14) ?   120 : (j == 15) ?   16 : (j == 16) ?   1 : (j == 17) ?   0 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i == 17) ? ((j == 0) ? 1 : (j == 1) ? 17 : (j == 2) ? 136 : (j == 3) ? 680 : (j == 4) ? 2380 : (j == 5) ?  6188 : (j == 6) ? 12376 : (j == 7) ? 19448 : (j == 8) ? 24310 : (j == 9) ? 24310 : (j == 10) ? 19448 : (j == 11) ? 12376 : (j == 12) ?  6188 : (j == 13) ?  2380 : (j == 14) ?   680 : (j == 15) ?  136 : (j == 16) ?  17 : (j == 17) ?   1 : (j == 18) ?  0 : (j == 19) ? 0 : -1) :    \
                                (i == 18) ? ((j == 0) ? 1 : (j == 1) ? 18 : (j == 2) ? 153 : (j == 3) ? 816 : (j == 4) ? 3060 : (j == 5) ?  8568 : (j == 6) ? 18564 : (j == 7) ? 31824 : (j == 8) ? 43758 : (j == 9) ? 48620 : (j == 10) ? 43758 : (j == 11) ? 31824 : (j == 12) ? 18564 : (j == 13) ?  8568 : (j == 14) ?  3060 : (j == 15) ?  816 : (j == 16) ? 153 : (j == 17) ?  18 : (j == 18) ?  1 : (j == 19) ? 0 : -1) :    \
                                (i == 19) ? ((j == 0) ? 1 : (j == 1) ? 19 : (j == 2) ? 171 : (j == 3) ? 969 : (j == 4) ? 3876 : (j == 5) ? 11628 : (j == 6) ? 27132 : (j == 7) ? 50388 : (j == 8) ? 75582 : (j == 9) ? 92378 : (j == 10) ? 92378 : (j == 11) ? 75582 : (j == 12) ? 50388 : (j == 13) ? 27132 : (j == 14) ? 11628 : (j == 15) ? 3876 : (j == 16) ? 969 : (j == 17) ? 171 : (j == 18) ? 19 : (j == 19) ? 1 : -1) : -1 \
                                )


#define HWT_4BIT(x)      ((((x) >> 0) & 0x01) + (((x) >> 1) & 0x01) + (((x) >> 2) & 0x01) + (((x) >> 3) & 0x01))
#define HWT_8BIT(x)      (HWT_4BIT(x) + HWT_4BIT((x) >> 4))
#define HWT_16BIT(x)     (HWT_8BIT(x) + HWT_8BIT((x) >> 8))
#define HWT_32BIT(x)     (HWT_16BIT(x) + HWT_16BIT((x) >> 16))
#define HWT_64BIT(x)     (HWT_32BIT(x) + HWT_32BIT((x) >> 32))


#define SEMIHONEST  0
#define MALICIOUS   1

#define ADVERSARY   MALICIOUS

#define CONST_N     4
#define CONST_T     1

#define TAU         (BINOMIAL_TABLE(CONST_N, CONST_T))
#define TAU_i       (BINOMIAL_TABLE(CONST_N - 1, CONST_T))

#define MUL_FACTOR  NBITS_FIELD
#define LAMBDA      (NBITS_FIELD/MUL_FACTOR)
#define PRF_BYTES   ((LAMBDA+7)/8)

uint32_t binom_table[CONST_N + 1][CONST_N + 1] = {0};

void generate_table(){
    for(int n = 0; n < CONST_N + 1; n++)
        for(int t = 0; t <= n; t++)
            binom_table[n][t] = BINOMIAL_TABLE(n,t);
}

uint32_t binomial(unsigned int n, unsigned int t){
    // return BINOMIAL_TABLE(n, t);
    return binom_table[n][t];
}



typedef uint32_t    ind_T;
typedef uint8_t     com_T[CONST_T];
typedef uint32_t    bin_T;


typedef struct {
    ind_T ind;      // share represented as a natural numbers in [0,infty) but actually bounded by (CONST_N choose CONST_T)
    com_T com;      // share represented as a combinations i.e. subsets of CONST_T elements from the set {0,1,...,CONST_N-1}
    bin_T bin;      // share represented as a binary string of length CONST_N with exactly CONST_T bits equal to 1 (bit i = 0 iff index i is selected)
} share_T;

#define T_0   (share_T) {\
                            0, \
                            FIRST_COMB(CONST_T),   \
                            (1ULL << CONST_T) - 1    \
                        }









f_elm_t f_inverses[CONST_N + 1] = {0};

void init_inverses() {
    f_elm_t t0 = {0};

    for(int i = 0; i < CONST_N + 1; i++){
        f_inv(t0, f_inverses[i]);
        f_add(t0, Mont_one, t0);
    }
}







void print_ind_T(const ind_T T_ind){
    #if   (TAU < 10)
        printf("%01d", T_ind);
    #elif (TAU < 100)
        printf("%02d", T_ind);
    #elif (TAU < 1000)
        printf("%03d", T_ind);
    #elif (TAU < 10000)
        printf("%04d", T_ind);
    #else
        printf("%05d", T_ind);
    #endif
}

void print_com_T(const com_T T_com){
    printf("{");
    for(int i = CONST_T - 1; i > 0; i--)
        printf("%01d, ", T_com[i]);
    printf("%01d", T_com[0]);
    printf("}");
}

void print_bin_T(const bin_T T_bin){
    for(int i = CONST_N - 1; i >= 0; i--)
        printf("%c", BIT_TO_STR((T_bin) & (1 << i)));
}

void print_T(const share_T T){
    print_ind_T(T.ind);printf("   ");
    print_com_T(T.com);printf("   ");
    print_bin_T(T.bin);printf("   ");
}



// Convert share from binary format to integer format
void ind_to_com_T(const ind_T* T_ind, com_T* T_com){
    unsigned int j = CONST_T - 1;
    ind_T T_ind_t = *T_ind;

    for(int i = CONST_N - 1; i >= 0; i--){
        if(binomial(i, j + 1) <= T_ind_t){
            (*T_com)[j] = i;
            T_ind_t -= binomial(i, j+1);
            j--;
        }
    }
}

// Convert share from index format to integer format
void com_to_ind_T(const com_T* T_com, ind_T* T_ind){
    *T_ind = 0;
    for(int i = 0; i < CONST_T; i++)
        *T_ind += binomial((*T_com)[i], i + 1);
}


// Convert share from binary format to index format
void bin_to_com_T(const bin_T* T_bin, com_T* T_com){
    unsigned int j = 0;
    bin_T T_bin_t = *T_bin;
    for(int i = 0; i < CONST_N; i++){
        if(T_bin_t & 0x01){
            (*T_com)[j] = i;
            j++;
        }
        T_bin_t >>= 1;
    }
}

// Convert share from index format to binary format
void com_to_bin_T(const com_T* T_com, bin_T* T_bin){
    *T_bin = 0;
    for(int i = 0; i < CONST_T; i++)
        *T_bin |= (1 << (*T_com)[i]);

}


// Convert share from binary format to integer format
void bin_to_ind_T(const bin_T* T_bin, ind_T* T_ind){
    com_T T_com;
    bin_to_com_T(T_bin, &T_com);
    com_to_ind_T(&T_com, T_ind);
}

// Convert share from int format to binary format
void ind_to_bin_T(const ind_T* T_ind, bin_T* T_bin)
{
    com_T T_com[1] = {0};
    ind_to_com_T(T_ind, T_com);
    com_to_bin_T(T_com, T_bin);
}






void next_T(share_T* T)
{
    (T->ind)++;
    if((T->ind) != TAU){
        ind_to_com_T(&(T->ind), &(T->com));
        com_to_bin_T(&(T->com), &(T->bin));
    }
}


void update_T(share_T* T, char* type){
    if     (strcmp(type, "ind") == 0){
        ind_to_com_T(&(T->ind), &(T->com));
        com_to_bin_T(&(T->com), &(T->bin));
        }
    else if(strcmp(type, "com") == 0){
        ind_to_com_T(&(T->ind), &(T->com));
        com_to_bin_T(&(T->com), &(T->bin));
        }
    else if(strcmp(type, "bin") == 0){        
        bin_to_com_T(&(T->bin), &(T->com));
        com_to_ind_T(&(T->com), &(T->ind));
        }
    else
        printf("Invalid share representation selector. Please choose ind, com or bin.\n");
}





typedef f_elm_t     ASS[CONST_N];
typedef f_elm_t     ASS_i[1];
typedef f_elm_t     RSS[TAU];
typedef f_elm_t     RSS_i[TAU_i];
typedef f_elm_t     DRSS[TAU * TAU];
typedef f_elm_t     DRSS_i[TAU_i * TAU_i];




void add_RSS(const RSS a, const RSS b, RSS c){
    for(ind_T T_ind = 0; T_ind < TAU; T_ind ++)
        f_add(a[T_ind], b[T_ind], c[T_ind]);
}

void add_RSS_i(const RSS_i a, const RSS_i b, RSS_i c){
    for(ind_T T_ind = 0; T_ind < TAU_i; T_ind ++)
        f_add(a[T_ind], b[T_ind], c[T_ind]);
}

void sub_RSS(const RSS a, const RSS b, RSS c){
    for(ind_T T_ind = 0; T_ind < TAU; T_ind ++)
        f_sub(a[T_ind], b[T_ind], c[T_ind]);
}

void sub_RSS_i(const RSS_i a, const RSS_i b, RSS_i c){
    for(ind_T T_ind = 0; T_ind < TAU_i; T_ind ++)
        f_sub(a[T_ind], b[T_ind], c[T_ind]);
}

void add_DRSS(const DRSS a, const DRSS b, DRSS c){
    for(ind_T T_ind = 0; T_ind < TAU*TAU; T_ind ++)
        f_add(a[T_ind], b[T_ind], c[T_ind]);
}

void add_DRSS_i(const DRSS_i a, const DRSS_i b, DRSS_i c){
    for(ind_T T_ind = 0; T_ind < TAU_i*TAU_i; T_ind ++)
        f_add(a[T_ind], b[T_ind], c[T_ind]);
}

void sub_DRSS(const DRSS a, const DRSS b, DRSS c){
    for(ind_T T_ind = 0; T_ind < TAU*TAU; T_ind ++)
        f_sub(a[T_ind], b[T_ind], c[T_ind]);
}

void sub_DRSS_i(const DRSS_i a, const DRSS_i b, DRSS_i c){
    for(ind_T T_ind = 0; T_ind < TAU_i*TAU_i; T_ind ++)
        f_sub(a[T_ind], b[T_ind], c[T_ind]);
}


void to_ASS(const f_elm_t a, const unsigned int I, ASS A){
    f_elm_t t0 = {0};

    for(ind_T T_ind = 0; T_ind < I - 1; T_ind++){
        f_rand(A[T_ind]);
        f_add(t0, A[T_ind], t0);
    }

    f_sub(a, t0, A[I - 1]);
}

void to_RSS(const f_elm_t a, RSS A){
    f_elm_t t0 = {0};

    for(ind_T T_ind = 0; T_ind < TAU - 1; T_ind++){
        f_rand(A[T_ind]);
        f_add(t0, A[T_ind], t0);
    }

    f_sub(a, t0, A[TAU - 1]);
}

void to_DRSS(const f_elm_t a, DRSS A){
    f_elm_t t0 = {0};

    for(ind_T T_ind = 0; T_ind < TAU * TAU - 1; T_ind++){
        f_rand(A[T_ind]);
        f_add(t0, A[T_ind], t0);
    }

    f_sub(a, t0, A[TAU * TAU - 1]);
}


void ASS_rand(ASS A, const unsigned int I){
    f_elm_t t0;
    f_rand(t0);
    to_ASS(t0, I, A);
}

void RSS_rand(RSS A){
    f_elm_t t0;
    f_rand(t0);
    to_RSS(t0, A);
}

void DRSS_rand(DRSS A){
    f_elm_t t0;
    f_rand(t0);
    to_DRSS(t0, A);
}


void ASS_zero(ASS A, unsigned int I){
    f_elm_t t0 = {0};
    to_ASS(t0, I, A);
}

void RSS_zero(ASS A){
    f_elm_t t0 = {0};
    to_RSS(t0, A);
}

void DRSS_zero(ASS A){
    f_elm_t t0 = {0};
    to_DRSS(t0, A);
}


void open_ASS(const ASS A, const unsigned int I, f_elm_t a){
    f_elm_t t0 = {0};

    for(ind_T T_ind = 0; T_ind < I; T_ind++)
        f_add(t0, A[T_ind], t0);
    
    f_copy(t0, a);
}

void open_RSS(const ASS A, f_elm_t a){
    f_elm_t t0 = {0};

    for(ind_T T_ind = 0; T_ind < TAU; T_ind++)
        f_add(t0, A[T_ind], t0);
    
    f_copy(t0, a);
}

void open_DRSS(const ASS A, f_elm_t a){
    f_elm_t t0 = {0};

    for(ind_T T_ind = 0; T_ind < TAU * TAU; T_ind++)
        f_add(t0, A[T_ind], t0);
    
    f_copy(t0, a);
}


void ASS_i_to_ASS(const ASS_i* A_i, const unsigned int I, ASS A){
    for(unsigned int i = 0; i < I; i++)
        f_copy(A_i[i][0], A[i]);
}
// Number of servers that hold both T0 and T1
// Number of indices i not in T0 nor T1
// CONST_N - HAMMING_WEIGHT(T0 | T1)
int S_TT(share_T T0, share_T T1){
    return CONST_N - HWT_32BIT(T0.bin | T1.bin);
}


void mul_RSS_ASS(const RSS_i a, const RSS_i b, ASS_i c){
    share_T T0, T1;

    f_elm_t t0 = {0}, t1;

    for(T0 = T_0; T0.ind < TAU_i; next_T(&T0)){
    for(T1 = T_0; T1.ind < TAU_i; next_T(&T1)){

        f_mul(a[T0.ind], b[T1.ind], t1);
        f_mul(t1, f_inverses[S_TT(T0, T1)], t1);

        f_add(t0, t1, t0);
    }}
    f_copy(t0, c[0]);
}


void mul_RSS_DRSS(const RSS_i a, const RSS_i b, DRSS_i c){
    share_T T0, T1;

    f_elm_t t0;

    for(T0 = T_0; T0.ind < TAU_i; next_T(&T0)){
    for(T1 = T_0; T1.ind < TAU_i; next_T(&T1)){

        f_mul(a[T0.ind], b[T1.ind], t0);
        f_mul(t0, f_inverses[S_TT(T0, T1)], t0);

        f_copy(t0, c[T0.ind * TAU_i + T1.ind]);
    }}
}



// Distribute a RSS x to CONST_N parties
// Each party recieves x_i[i] shares
void distribute_RSS(const RSS x, RSS_i* x_i){
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T = T_0; T.ind < TAU; next_T(&T)){
        for(int i = 0; i < CONST_N; i++){
            if(i_hold_T(i, T.bin)){
                f_copy(x[T.ind], x_i[i][Ti_inds[i]++]);
            }
        }
    }
}

// Reconstructs a RSS x from party shares x_i.
// Returns 0 if it is well formed, 0xFF otherwise
unsigned char reconstruct_RSS(const RSS_i* x_i, RSS x){
    unsigned char all_tests = 0, flag = 0;
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T = T_0; T.ind < TAU; next_T(&T)){
        flag = 0;

        for(int i = CONST_N - 1; i >= 0; i--){
            if(i_hold_T(i, T.bin)){
                if(!flag){
                    f_copy(x_i[i][Ti_inds[i]], x[T.ind]);
                    flag = 1;
                }
                all_tests |= f_eq(x[T.ind], x_i[i][Ti_inds[i]++]);
            }
        }
    }
    return all_tests;
}


// Distribute a DRSS x to CONST_N parties
// Each party recieves x_i[i] shares
void distribute_DRSS(const DRSS x, DRSS_i* x_i){
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        for(int i = 0; i < CONST_N; i++){
            if(i_hold_T(i, (T0.bin | T1.bin))){
                f_copy(x[T0.ind * TAU + T1.ind], x_i[i][Ti_inds[i]++]);
            }
        }
    }}
}

// Reconstructs a DRSS x from party shares x_i.
// Returns 0 if it is well formed, 0xFF otherwise
unsigned char reconstruct_DRSS(const DRSS_i* x_i, DRSS x){
    unsigned char all_tests = 0, flag = 0;
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        flag = 0;

        for(int i = CONST_N - 1; i >= 0; i--){
            if(i_hold_T(i, (T0.bin | T1.bin))){
                if(!flag){
                    f_copy(x_i[i][Ti_inds[i]], x[T0.ind * TAU + T1.ind]);
                    flag = 1;
                }
                all_tests |= f_eq(x[T0.ind * TAU + T1.ind], x_i[i][Ti_inds[i]++]);
            }
        }
    }}
    return all_tests;
}


// Returns 0 if the RSS shares x_i are well formed, 0xFF otherwise
unsigned char RSS_well_formed(const RSS_i* x_i){
    unsigned char all_tests = 0, flag = 0;
    ind_T Ti_inds[CONST_N] = {0};
    share_T T;
    f_elm_t t0;

    for(T = T_0; T.ind < TAU; next_T(&T)){
        flag = 0;

        for(int i = CONST_N - 1; i >= 0; i--){
            if(i_hold_T(i, T.bin)){
                if(!flag){
                    f_copy(x_i[i][Ti_inds[i]], t0);
                    flag = 1;
                }
                all_tests |= f_eq(t0, x_i[i][Ti_inds[i]++]);
            }
        }
    }
    return all_tests;
}

// Print RSS secret sharing and the distribution to n parties
// For debugging purposes and checking if shares are ok
unsigned char test_RSS_print(const RSS x, const RSS_i* x_i){
    share_T T;
    ind_T Ti_inds[CONST_N] = {0};

    unsigned char all_tests = 0, test = 0;

    for(T = T_0; T.ind < TAU; next_T(&T)){
        print_T(T);
        print_f_elm(x[T.ind]); printf("  |\t");
        test = 0;

        for(int i = CONST_N - 1; i >= 0; i--){
            if(i_hold_T(i, T.bin)){
                test |= f_eq(x[T.ind], x_i[i][Ti_inds[i]]);
                print_f_elm(x_i[i][Ti_inds[i]++]);
            }
            else
                printf("%*c", 2 + 2*NBYTES_FIELD + 3, ' ');

            printf("\t");
        }

        all_tests |= test;
        printf("%s\n", PASS(test));
    }

    return all_tests;
}


// SEMI-HONEST PROTOCOLS
void sh_setup(const f_elm_t k[LAMBDA], f_elm_t s2[LAMBDA], ASS_i r_i[CONST_N][LAMBDA], RSS_i k_i[CONST_N][LAMBDA], RSS_i s2_i[CONST_N][LAMBDA]){
    RSS K, S2;
    RSS_i temp_k_i[LAMBDA][CONST_N], temp_s2_i[LAMBDA][CONST_N];
    ASS temp_r_i[LAMBDA];


    for(int j = 0; j < LAMBDA; j++){
        to_RSS(k[j], K);
        distribute_RSS(K, temp_k_i[j]);

        f_rand(s2[j]);
        to_RSS(s2[j], S2);
        distribute_RSS(S2, temp_s2_i[j]);

        ASS_zero(temp_r_i[j], CONST_N);
    }

    for(int i = 0; i < CONST_N; i++){
    for(int j = 0; j < LAMBDA; j++){
    for(int T = 0; T < TAU_i; T++){
        f_copy(temp_k_i[j][i][T], k_i[i][j][T]);
        f_copy(temp_s2_i[j][i][T], s2_i[i][j][T]);
        }
        f_copy(temp_r_i[j][i], r_i[i][j][0]);
    }}

}

void input(const f_elm_t x, RSS_i x_i[CONST_N]){
    RSS X;
    to_RSS(x, X);
    distribute_RSS(X, x_i);
}

 
void sh_evaluation(const RSS_i x_i, const RSS_i k_i[LAMBDA], const RSS_i s2_i[LAMBDA], const ASS_i r_i[LAMBDA], ASS_i o_i[LAMBDA]){
    RSS_i a_i;
    for(int j = 0; j < LAMBDA; j++){
        add_RSS_i(x_i, k_i[j], a_i);
        mul_RSS_ASS(a_i, s2_i[j], o_i[j]);
        f_add(o_i[j][0], r_i[j][0], o_i[j][0]);
    }

}


void sh_reconstruction(const ASS_i o_i[CONST_N][LAMBDA], f_elm_t o[LAMBDA]){
    ASS o_j;
    for(int j = 0; j < LAMBDA; j++){
        for(int i = 0; i < CONST_N; i++)
            f_copy(o_i[i][j][0], o_j[i]);
        open_ASS(o_j, CONST_N, o[j]);
    }
}




void calc_symbols(f_elm_t o[LAMBDA], unsigned char LOPRF[PRF_BYTES]){
    int index, pos;
    unsigned char t = 0;
    for(int j = 0; j < LAMBDA; j++){
        index = j / PRF_BYTES;
        pos = j % PRF_BYTES;

        f_leg(o[j], &t);
        LOPRF[index] |= (t << pos);
    }
}






// SEMI-HONEST PROTOCOL
unsigned char sh_protocol(){

    // SERVER PRECOMPUTATION
    f_elm_t k[LAMBDA];
    RSS_i k_i[CONST_N][LAMBDA];

    f_elm_t s2[LAMBDA];
    RSS_i s2_i[CONST_N][LAMBDA];
    ASS_i r_i[CONST_N][LAMBDA];
    
    for(int j = 0; j < LAMBDA; j++)
        f_rand(k[j]);

    sh_setup(k, s2, r_i, k_i, s2_i);


    // CLIENT INPUT STAGE
    f_elm_t x;
    RSS_i x_i[CONST_N];

    f_rand(x);
    input(x, x_i);


    // SERVERS EVALUATION STAGE
    ASS_i o_i[CONST_N][LAMBDA];

    for(int i = 0; i < CONST_N; i++)
        sh_evaluation(x_i[i], k_i[i], s2_i[i], r_i[i], o_i[i]);

 
    // CLIENT RECONSTRUCTION STAGE
    f_elm_t o[LAMBDA];
    sh_reconstruction(o_i, o);


    unsigned char test = 0;
    f_elm_t z;  

    for(int j = 0; j < LAMBDA; j++){
        f_add(x, k[j], z);
        f_mul(z, s2[j], z);        
        test |= f_eq(z, o[j]);
    }


    unsigned char L[PRF_BYTES];

    calc_symbols(o, L);

    return(test);
}







// MALICIOUS PROTOCOLS

// MALICIOUS SETUP
void mal_setup(const f_elm_t k[LAMBDA], f_elm_t s2[LAMBDA], DRSS_i r_i[CONST_N][LAMBDA], ASS_i ta_i[CONST_N][LAMBDA][TAU_i * TAU_i], ASS_i tb_i[CONST_N][LAMBDA][TAU_i * TAU_i], ASS_i tr_i[CONST_N][LAMBDA][TAU_i * TAU_i], RSS_i k_i[CONST_N][LAMBDA], RSS_i s2_i[CONST_N][LAMBDA]){
    
    RSS K, S2;
    RSS_i temp_k_i[LAMBDA][CONST_N], temp_s2_i[LAMBDA][CONST_N];
    DRSS temp_r[LAMBDA];
    ASS temp_ta;
    ASS temp_tb;
    ASS temp_tr;
    
    for(int j = 0; j < LAMBDA; j++){
        to_RSS(k[j], K);
        distribute_RSS(K, temp_k_i[j]);

        f_rand(s2[j]);
        to_RSS(s2[j], S2);
        distribute_RSS(S2, temp_s2_i[j]);

        DRSS_zero(temp_r[j]);
    }

    
    for(int i = 0; i < CONST_N; i++){
    for(int j = 0; j < LAMBDA; j++){
    for(int T = 0; T < TAU_i; T++){
        f_copy(temp_k_i[j][i][T], k_i[i][j][T]);
        f_copy(temp_s2_i[j][i][T], s2_i[i][j][T]);
    }}}




    for(int j = 0; j < LAMBDA; j++){
    ind_T Ti_inds[CONST_N] = {0};
    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        ASS_zero(temp_ta, S_TT(T0, T1));
        ASS_zero(temp_tb, S_TT(T0, T1));
        ASS_zero(temp_tr, S_TT(T0, T1));
        int l = 0;
        for(int i = 0; i < CONST_N; i++){
            if(i_hold_T(i, (T0.bin | T1.bin))){
                f_copy(temp_ta[l], ta_i[i][j][Ti_inds[i]][0]);
                f_copy(temp_tb[l], tb_i[i][j][Ti_inds[i]][0]);
                f_copy(temp_tr[l], tr_i[i][j][Ti_inds[i]][0]);
                f_copy(temp_r[j][T0.ind * TAU + T1.ind], r_i[i][j][Ti_inds[i]]);
                Ti_inds[i]++; l++;
            }
        }
    }}}
}

// MALICIOUS SETUP
void mal_setup_loc(const f_elm_t k[LAMBDA], f_elm_t s2[LAMBDA], DRSS_i r_i[LAMBDA], ASS_i ta_i[LAMBDA][TAU_i * TAU_i], ASS_i tb_i[LAMBDA][TAU_i * TAU_i], ASS_i tr_i[LAMBDA][TAU_i * TAU_i], RSS_i k_i[CONST_N][LAMBDA], RSS_i s2_i[CONST_N][LAMBDA]){
    
    RSS K, S2;
    RSS_i temp_k_i[LAMBDA][CONST_N], temp_s2_i[LAMBDA][CONST_N];
    DRSS temp_r[LAMBDA];
    ASS temp_ta;
    ASS temp_tb;
    ASS temp_tr;
    
    for(int j = 0; j < LAMBDA; j++){
        to_RSS(k[j], K);
        distribute_RSS(K, temp_k_i[j]);

        f_rand(s2[j]);
        to_RSS(s2[j], S2);
        distribute_RSS(S2, temp_s2_i[j]);

        DRSS_zero(temp_r[j]);
    }

    
    for(int i = 0; i < CONST_N; i++){
    for(int j = 0; j < LAMBDA; j++){
    for(int T = 0; T < TAU_i; T++){
        f_copy(temp_k_i[j][i][T], k_i[i][j][T]);
        f_copy(temp_s2_i[j][i][T], s2_i[i][j][T]);
    }}}




    for(int j = 0; j < LAMBDA; j++){
    ind_T Ti_inds[CONST_N] = {0};
    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        ASS_zero(temp_ta, S_TT(T0, T1));
        ASS_zero(temp_tb, S_TT(T0, T1));
        ASS_zero(temp_tr, S_TT(T0, T1));
        int l = 0;
        for(int i = 0; i < CONST_N; i++){
            if(i_hold_T(i, (T0.bin | T1.bin))){
                f_copy(temp_ta[l], ta_i[j][Ti_inds[i]][0]);
                f_copy(temp_tb[l], tb_i[j][Ti_inds[i]][0]);
                f_copy(temp_tr[l], tr_i[j][Ti_inds[i]][0]);
                f_copy(temp_r[j][T0.ind * TAU + T1.ind], r_i[j][Ti_inds[i]]);
                Ti_inds[i]++; l++;
            }
        }
    }}}
}


// MALICIOUS EVALUATION
void mal_evaluation(const RSS_i x_i, const RSS_i k_i[LAMBDA], const RSS_i s2_i[LAMBDA], const DRSS_i r_i[LAMBDA], const ASS_i t_ai[LAMBDA][TAU_i * TAU_i], const ASS_i t_bi[LAMBDA][TAU_i * TAU_i], const ASS_i t_ri[LAMBDA][TAU_i * TAU_i], DRSS_i o_i[LAMBDA]){
    RSS_i a_i;
    f_elm_t t0, t1;
    
    for(int j = 0; j < LAMBDA; j++){
        add_RSS_i(x_i, k_i[j], a_i);

        for(share_T T0 = T_0; T0.ind < TAU_i; next_T(&T0)){
        for(share_T T1 = T_0; T1.ind < TAU_i; next_T(&T1)){

            f_mul(a_i[T0.ind], s2_i[j][T1.ind], t0);
            f_add(t0, r_i[j][T0.ind * TAU_i + T1.ind], t0);

            f_mul(a_i[T0.ind], t_ai[j][T0.ind * TAU_i + T1.ind][0], t1);
            f_add(t0, t1, t0);

            f_mul(s2_i[j][T1.ind], t_bi[j][T0.ind * TAU_i + T1.ind][0], t1);
            f_add(t0, t1, t0);

            f_add(t0, t_ri[j][T0.ind * TAU_i + T1.ind][0], t0);

            f_copy(t0, o_i[j][T0.ind * TAU_i + T1.ind]);
        }}
    }

}



void mal_reconstruction(const DRSS_i o_i[CONST_N][LAMBDA], f_elm_t o[LAMBDA]){

    for(int j = 0; j < LAMBDA; j++){
        ind_T Ti_inds[CONST_N] = {0};
        f_elm_t v = {0};

        for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
        for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
            f_elm_t v_TT = {0};

            for(int i = 0; i < CONST_N; i++){
                if(i_hold_T(i, (T0.bin | T1.bin))){
                    f_add(v_TT, o_i[i][j][Ti_inds[i]++], v_TT);
                }            
            }
            f_mul(v_TT, f_inverses[S_TT(T0, T1)], v_TT);
            f_add(v, v_TT, v);  
        }}
        f_copy(v, o[j]);
    }

}



// MALICIOUS - PROTOCOL
unsigned char mal_protocol(){

    // SERVER PRECOMPUTATION
    f_elm_t k[LAMBDA];
    RSS_i k_i[CONST_N][LAMBDA];

    f_elm_t s2[LAMBDA];
    RSS_i s2_i[CONST_N][LAMBDA];
    
    DRSS_i r_i[CONST_N][LAMBDA];
    ASS_i ta_i[CONST_N][LAMBDA][TAU_i * TAU_i];
    ASS_i tb_i[CONST_N][LAMBDA][TAU_i * TAU_i];
    ASS_i tr_i[CONST_N][LAMBDA][TAU_i * TAU_i];

    for(int j = 0; j < LAMBDA; j++)
        f_rand(k[j]);

    mal_setup(k, s2, r_i, ta_i, tb_i, tr_i, k_i, s2_i);


    // CLIENT INPUT STAGE
    f_elm_t x;
    RSS_i x_i[CONST_N];

    f_rand(x);
    input(x, x_i);

    
    // SERVERS EVALUATION STAGE
    DRSS_i o_i[CONST_N][LAMBDA];

    for(int i = 0; i < CONST_N; i++)
        mal_evaluation(x_i[i], k_i[i], s2_i[i], r_i[i], ta_i[i], tb_i[i], tr_i[i], o_i[i]);

     
    // // CLIENT RECONSTRUCTION STAGE
    f_elm_t o[LAMBDA];
    mal_reconstruction(o_i, o);


    unsigned char test = 0;
    f_elm_t z;  

    for(int j = 0; j < LAMBDA; j++){
        f_add(x, k[j], z);
        f_mul(z, s2[j], z);        
        test |= f_eq(z, o[j]);
    }


    unsigned char L[PRF_BYTES];

    calc_symbols(o, L);

    return(test);
}



void bench_sh(int function_selector, const f_elm_t k[LAMBDA], f_elm_t s2[LAMBDA], ASS_i r_i[CONST_N][LAMBDA], RSS_i k_i[CONST_N][LAMBDA], RSS_i s2_i[CONST_N][LAMBDA], ASS_i o_i[CONST_N][LAMBDA], const f_elm_t x, RSS_i x_i[CONST_N], f_elm_t o[LAMBDA], unsigned char L[PRF_BYTES], int i){
    switch (function_selector) {
        case 0:
            sh_setup(k, s2, r_i, k_i, s2_i);
            break;
        case 1:
            input(x, x_i);
            break;
        case 2:
            sh_evaluation(x_i[i % CONST_N], k_i[i % CONST_N], s2_i[i % CONST_N], r_i[i % CONST_N], o_i[i % CONST_N]);
            break;
        case 3:
            sh_reconstruction(o_i, o);
            break;
        case 4:
            calc_symbols(o, L);
            break;
        default:
            printf("Invalid function_selector. Please choose 1 for sh_setup, 2 for sh_input, 3 for sh_evaluation, 4 for sh_reconstruction.\n");
    }
}


// void bench_mal(int function_selector, const f_elm_t k[LAMBDA], f_elm_t s2[LAMBDA], DRSS_i r_i[CONST_N][LAMBDA], ASS_i ta_i[CONST_N][LAMBDA][TAU_i * TAU_i], ASS_i tb_i[CONST_N][LAMBDA][TAU_i * TAU_i], ASS_i tr_i[CONST_N][LAMBDA][TAU_i * TAU_i], RSS_i k_i[CONST_N][LAMBDA], RSS_i s2_i[CONST_N][LAMBDA], DRSS_i o_i[CONST_N][LAMBDA], const f_elm_t x, RSS_i x_i[CONST_N], f_elm_t o[LAMBDA], unsigned char L[PRF_BYTES], int i){
void bench_mal(int function_selector, const f_elm_t k[LAMBDA], f_elm_t s2[LAMBDA], DRSS_i r_i[LAMBDA], ASS_i ta_i[LAMBDA][TAU_i * TAU_i], ASS_i tb_i[LAMBDA][TAU_i * TAU_i], ASS_i tr_i[LAMBDA][TAU_i * TAU_i], RSS_i k_i[CONST_N][LAMBDA], RSS_i s2_i[CONST_N][LAMBDA], DRSS_i o_i[CONST_N][LAMBDA], const f_elm_t x, RSS_i x_i[CONST_N], f_elm_t o[LAMBDA], unsigned char L[PRF_BYTES], int i){
    switch (function_selector) {
        case 5:
            // mal_setup(k, s2, r_i, ta_i, tb_i, tr_i, k_i, s2_i);
            mal_setup_loc(k, s2, r_i, ta_i, tb_i, tr_i, k_i, s2_i);
            break;
        case 6:
            input(x, x_i);
            break;
        case 7:
            mal_evaluation(x_i[i % CONST_N], k_i[i % CONST_N], s2_i[i % CONST_N], r_i, ta_i, tb_i, tr_i, o_i[i % CONST_N]);
            // mal_evaluation(x_i[i % CONST_N], k_i[i % CONST_N], s2_i[i % CONST_N], r_i[i % CONST_N], ta_i[i % CONST_N], tb_i[i % CONST_N], tr_i[i % CONST_N], o_i[i % CONST_N]);
            break;
        case 8:
            mal_reconstruction(o_i, o);
            break;
        case 9:
            calc_symbols(o, L);
            break;
        default:
            printf("Invalid function_selector. Please choose 1 for sh_setup, 2 for sh_input, 3 for sh_evaluation, 4 for sh_reconstruction.\n");
    }
}






int main(int argc, char* argv[]){
    // Remove compilation warnings
    (void)argv[0];
    (void)argc;

    init_inverses();
    generate_table();
    const char *function_names[] = {\
    "sh_setup runs in .........................", \
    "sh_input runs in .........................", \
    "sh_evaluation runs in ....................", \
    "sh_reconstruction runs in ................", \
    "calc_symbols runs in .....................", \
    "mal_setup runs in ........................", \
    "mal_input runs in ........................", \
    "mal_evaluation runs in ...................", \
    "mal_reconstruction runs in ...............", \
    "calc_symbols runs in ....................."};
    


    // BENCHMARKING
    uint64_t nsecs_pre, nsecs_post, nsecs;




    #define LOOPS       (50)

    #if (ADVERSARY==SEMIHONEST)

        f_elm_t k[LAMBDA];
        RSS_i k_i[CONST_N][LAMBDA];

        f_elm_t s2[LAMBDA];
        RSS_i s2_i[CONST_N][LAMBDA];

        ASS_i r_i[CONST_N][LAMBDA];

        f_elm_t x;
        RSS_i x_i[CONST_N];

        ASS_i o_i[CONST_N][LAMBDA];

        f_elm_t o[LAMBDA];
        unsigned char LOPRF[PRF_BYTES];

        int test = 0; //sh_protocol();

        printf("%3d bits, Semi-honest                                     %s\n\n", NBITS_FIELD, PASS(test));
        // printf("Semi-honest                                               %s\n", PASS(tsh));
        for(int function_selector = 0; function_selector < 5; function_selector++){
            nsecs_pre = 0, nsecs_post = 0, nsecs = 0;

            nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
            for(int i = 0; i < LOOPS; i++)
                bench_sh(function_selector, k, s2, r_i, k_i, s2_i, o_i, x, x_i, o, LOPRF, i);
            nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
            nsecs += (nsecs_post-nsecs_pre);
            
            printf("%s %13s ns\n", function_names[function_selector], print_num((double)((uint64_t)(nsecs/(LOOPS)))));
        }


    #elif(ADVERSARY==MALICIOUS)

        f_elm_t k[LAMBDA];
        RSS_i k_i[CONST_N][LAMBDA];

        f_elm_t s2[LAMBDA];
        RSS_i s2_i[CONST_N][LAMBDA];

        // Removed to benchmark only local operations.
        // Uncomment to consider the "global" view which controlls all secret shares.
        // Then change the bench_mal cases 5 and 7 correspondingly, and change the variable types for bench_mal arguments
        // DRSS_i  r_i[CONST_N][LAMBDA];
        // ASS_i   ta_i[CONST_N][LAMBDA][TAU_i * TAU_i];
        // ASS_i   tb_i[CONST_N][LAMBDA][TAU_i * TAU_i];
        // ASS_i   tr_i[CONST_N][LAMBDA][TAU_i * TAU_i];

        DRSS_i  r_i[LAMBDA];
        ASS_i   ta_i[LAMBDA][TAU_i * TAU_i];
        ASS_i   tb_i[LAMBDA][TAU_i * TAU_i];
        ASS_i   tr_i[LAMBDA][TAU_i * TAU_i];

        f_elm_t x;
        RSS_i x_i[CONST_N];
  
        DRSS_i o_i[CONST_N][LAMBDA];
        f_elm_t o[LAMBDA];
        unsigned char LOPRF[PRF_BYTES];

        // int test = mal_protocol();
        int test = 0;

        for(int j = 0; j < LAMBDA; j++){
            f_rand(k[j]);
            f_rand(x);
        }


        printf("%3d bits, Malicious                                       %s\n\n", NBITS_FIELD, PASS(test));

        // printf("\nMalicious                                                 %s\n", PASS(tmal));
        for(int function_selector = 5; function_selector < 10; function_selector++){
            nsecs_pre = 0, nsecs_post = 0, nsecs = 0;

            nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
            for(int i = 0; i < LOOPS; i++)
                bench_mal(function_selector, k, s2, r_i, ta_i, tb_i, tr_i, k_i, s2_i, o_i, x, x_i, o, LOPRF, i);
            nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
            nsecs += (nsecs_post-nsecs_pre);

            printf("%s %13s ns\n", function_names[function_selector], print_num((double)((uint64_t)(nsecs/(LOOPS)))));
        }

    #endif


    printf("...........................................................\n\n");



    return 0;
}






















