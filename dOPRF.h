#ifndef dOPRF_H
#define dOPRF_H

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "bench.h"
#include "arith.h"



#define SEMIHONEST  0
#define MALICIOUS   1

#define ADVERSARY   MALICIOUS
// #define ADVERSARY   SEMIHONEST

#define CONST_T     1
#define CONST_N     4





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




#define TAU         (BINOMIAL_TABLE(CONST_N, CONST_T))
#define TAU_i       (BINOMIAL_TABLE(CONST_N - 1, CONST_T))

#define MUL_FACTOR  2
#define LAMBDA      (NBITS_FIELD/MUL_FACTOR)
#define PRF_BYTES   ((LAMBDA+7)/8)



// Generate table with binomial values
void generate_table();

// Function to compute binomials (just read from table)
uint32_t binomial(unsigned int n, unsigned int t);

// Initialise inverse elements 1/1, 1/2, ..., 1/CONST_N, once at main call
void init_inverses();







/////////////////////////////////////////////
//  SHARES T
/////////////////////////////////////////////

// FIRST SHARE, i.e. FIRST COMBINATION OF T SUBSETS OF N
#define STARTING_T_0    {}
#define STARTING_T_1    {0}
#define STARTING_T_2    {0, 1}
#define STARTING_T_3    {0, 1, 2}
#define STARTING_T_4    {0, 1, 2, 3}
#define STARTING_T_5    {0, 1, 2, 3, 4}
#define STARTING_T_6    {0, 1, 2, 3, 4, 5}
#define STARTING_T_7    {0, 1, 2, 3, 4, 5, 6}
#define STARTING_T_8    {0, 1, 2, 3, 4, 5, 6, 7}
#define STARTING_T_9    {0, 1, 2, 3, 4, 5, 6, 7, 8}
#define STARTING_T_10   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
#define STARTING_T_11   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
#define STARTING_T_12   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}
#define STARTING_T_13   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}
#define STARTING_T_14   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}
#define STARTING_T_15   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14}
#define STARTING_T_16   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}
#define STARTING_T_17   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}
#define STARTING_T_18   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}
#define STARTING_T_19   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18}
#define STARTING_T_20   {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}

#define STARTING_T(i)   CONCATENATE(STARTING_T_,i)
#define FULL_SET(i)     CONCATENATE(STARTING_T_,i)


// Define shares T \subseteq \{1, ..., CONST_N\} | #T = CONST_T
// Shares can be represented as integers, sets or binary strings
typedef uint32_t    ind_T;
typedef uint8_t     com_T[CONST_T];
typedef uint32_t    bin_T;

typedef struct {
    ind_T ind;      // share represented as a natural numbers in [0,infty) but actually bounded by (CONST_N choose CONST_T)
    com_T com;      // share represented as a combinations i.e. subsets of CONST_T elements from the set {0,1,...,CONST_N-1}
    bin_T bin;      // share represented as a binary string of length CONST_N with exactly CONST_T bits equal to 1 (bit i = 0 iff index i is selected)
} share_T;

#define T_0   (share_T) {                           \
                            0,                      \
                            STARTING_T(CONST_T),    \
                            (1ULL << CONST_T) - 1   \
                        }



#define REMOVE_BIT(x, i)        (((x) & MASK(0, i)) | (((x) & MASK(i+1, 63)) >> 1))
#define INSERT_BIT(x, b, i)     (((x) & MASK(0, i)) | (((x) & MASK(i, 63)) << 1) | ((b) << (i)))
#define i_hold_T(i, T)          (!((T.bin) & (1 << (i))))
#define i_hold_TT(i, T1, T2)    (!(((T1.bin) | (T2.bin))& (1 << (i))))


/////////////////////////////////////////////
// Printing shares
/////////////////////////////////////////////

// Print share T as indice, i.e. integer
void print_ind_T(const ind_T T_ind);

// Print share T as combination, i.e., set
void print_com_T(const com_T T_com);

// Print share T as binary string with CONST_T 1s and CONST_N - CONST_T 0s
void print_bin_T(const bin_T T_bin);

// Print share T in all formats
void print_T(const share_T T);


/////////////////////////////////////////////
// Share conversion between different representations
/////////////////////////////////////////////

// Convert share from binary format to integer format
void ind_to_com_T(const ind_T* T_ind, com_T* T_com);

// Convert share from index format to integer format
void com_to_ind_T(const com_T* T_com, ind_T* T_ind);

// Convert share from binary format to index format
void bin_to_com_T(const bin_T* T_bin, com_T* T_com);

// Convert share from index format to binary format
void com_to_bin_T(const com_T* T_com, bin_T* T_bin);

// Convert share from binary format to integer format
void bin_to_ind_T(const bin_T* T_bin, ind_T* T_ind);

// Convert share from int format to binary format
void ind_to_bin_T(const ind_T* T_ind, bin_T* T_bin);


// Choose next share according to order (induced by T.ind)
void next_T(share_T* T);

// Update other representations of share
void update_T(share_T* T, char type);

// Number of servers that hold both T0 and T1
int S_TT(share_T T0, share_T T1);




/////////////////////////////////////////////
//  Subsets S
/////////////////////////////////////////////

typedef uint8_t     com_S[CONST_N];
typedef uint32_t    bin_S;
typedef uint8_t     card_S;

typedef struct {
    com_S  com;      // Subset represented as a combinations i.e. subsets of elements from the set {0,1,...,CONST_N-1}
    bin_S  bin;      // Subset represented as a binary string of length CONST_N (bit i = 1 iff index i is in set)
    card_S card;     // Cardinality of subset
} subset_S;

#define S_N   (subset_S) {                          \
                            FULL_SET(CONST_N),      \
                            (1ULL << CONST_N) - 1,  \
                            CONST_N                 \
                        }



// Update other representations of share
void update_S(subset_S* S, char type);

// Convert share from binary format to index format
void bin_to_com_S(const bin_S* S_bin, com_S* S_com, card_S* S_card);

// Convert share from index format to binary format
void com_to_bin_S(const com_S* S_com, const card_S* S_card, bin_S* S_bin);

// Define set of parties S holding a share T
void subset_holding_T(const share_T T, subset_S* S);

// Define set of parties holding S holding two shares T0, T1
void subset_holding_TT(const share_T T0, const share_T T1, subset_S* S);



/////////////////////////////////////////////
// Definition of basic secret sharing types
/////////////////////////////////////////////

// Additive secret shares global and local
typedef f_elm_t     ASS[CONST_N];
typedef f_elm_t     ASS_i[1];

// Replicated secret shares global and local
typedef f_elm_t     RSS[TAU];
typedef f_elm_t     RSS_i[TAU_i];

// Doubly replicated secret shares global and local
typedef f_elm_t     DRSS[TAU * TAU];
typedef f_elm_t     DRSS_i[TAU_i * TAU_i];


// Doubly replicated hash digests global and local
typedef hash_digest DRSS_digest[TAU * TAU];
typedef hash_digest DRSS_digest_i[TAU_i * TAU_i];

/////////////////////////////////////////////
// Seeds for generating random secret shares and other uses
/////////////////////////////////////////////

// RSS seeds
typedef prng_seed   RSS_seed[TAU];
typedef prng_seed   RSS_seed_i[TAU_i];

// DRSS seeds
typedef prng_seed   DRSS_seed[TAU * TAU];
typedef prng_seed   DRSS_seed_i[TAU_i * TAU_i];

/////////////////////////////////////////////
// Seeds for generating secret shares of zero
/////////////////////////////////////////////

// Seeds for generating ASS of zero
typedef prng_seed   ASS_seed_zero[CONST_N][CONST_N];
typedef prng_seed   ASS_seed_zero_i[CONST_N];

// Seeds for generating RSS of zero
typedef prng_seed   RSS_seed_zero[TAU][TAU];
typedef prng_seed   RSS_seed_zero_i[TAU_i][TAU];

// Seeds for generating DRSS of zero
typedef prng_seed   DRSS_seed_zero[TAU * TAU][TAU * TAU];
typedef prng_seed   DRSS_seed_zero_i[TAU_i * TAU_i][TAU * TAU];




/////////////////////////////////////////////
// Basic arithmetic of secret shares
/////////////////////////////////////////////

// Add two RSS shares
void add_RSS(const RSS a, const RSS b, RSS c);

// Add two RSS shares locally
void add_RSS_i(const RSS_i a, const RSS_i b, RSS_i c);

// Subtract two RSS shares
void sub_RSS(const RSS a, const RSS b, RSS c);

// Subtract two RSS shares locally
void sub_RSS_i(const RSS_i a, const RSS_i b, RSS_i c);

// Add two DRSS shares
void add_DRSS(const DRSS a, const DRSS b, DRSS c);

// Add two DRSS shares locally
void add_DRSS_i(const DRSS_i a, const DRSS_i b, DRSS_i c);

// Subtract two DRSS shares
void sub_DRSS(const DRSS a, const DRSS b, DRSS c);

// Subtract two DRSS shares locally
void sub_DRSS_i(const DRSS_i a, const DRSS_i b, DRSS_i c);






/////////////////////////////////////////////
// Random secret sharing of a field element
/////////////////////////////////////////////

// Create a random ASS secret sharing of a
void to_ASS_urandom(const f_elm_t a, ASS A, const subset_S S);
void to_ASS_prng(const f_elm_t a, ASS A, const subset_S S, prng_seed seed);

#define to_ASS_3 to_ASS_urandom
#define to_ASS_4 to_ASS_prng
#define to_ASS(...) EXPAND(CONCATENATE(to_ASS_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Create a random RSS secret sharing of a
void to_RSS_urandom(const f_elm_t a, RSS A);
void to_RSS_prng(const f_elm_t a, RSS A, prng_seed seed);
void to_RSS_prng_long(const f_elm_t a, RSS A, prng_seed* seed, unsigned long long nseeds);

#define to_RSS_2 to_RSS_urandom
#define to_RSS_3 to_RSS_prng
#define to_RSS_4 to_RSS_prng_long
#define to_RSS(...) EXPAND(CONCATENATE(to_RSS_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Create a random DRSS secret sharing of a
void to_DRSS_urandom(const f_elm_t a, DRSS A);
void to_DRSS_prng(const f_elm_t a, DRSS A, prng_seed seed);

#define to_DRSS_2 to_DRSS_urandom
#define to_DRSS_3 to_DRSS_prng
#define to_DRSS(...) EXPAND(CONCATENATE(to_DRSS_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


/////////////////////////////////////////////
// Random secret shares (of random elements)
/////////////////////////////////////////////

// Random ASS secret share
void ASS_rand_urandom(ASS A, const subset_S S);
void ASS_rand_prng(ASS A, const subset_S S, prng_seed seed);

#define ASS_rand_2 ASS_rand_urandom
#define ASS_rand_3 ASS_rand_prng
#define ASS_rand(...) EXPAND(CONCATENATE(ASS_rand_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Random RSS secret share
void RSS_rand_urandom(RSS A);
void RSS_rand_prng(RSS A, prng_seed seed);

#define RSS_rand_1 RSS_rand_urandom
#define RSS_rand_2 RSS_rand_prng
#define RSS_rand(...) EXPAND(CONCATENATE(RSS_rand_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Random RSS array secret share
void RSS_rand_array_urandom(RSS A[LAMBDA]);
void RSS_rand_array_prng(RSS A[LAMBDA], prng_seed seed);

#define RSS_rand_array_1 RSS_rand_array_urandom
#define RSS_rand_array_2 RSS_rand_array_prng
#define RSS_rand_array(...) EXPAND(CONCATENATE(RSS_rand_array_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Random DRSS secret share
void DRSS_rand_urandom(DRSS A);
void DRSS_rand_prng(DRSS A, prng_seed seed);

#define DRSS_rand_1 DRSS_rand_urandom
#define DRSS_rand_2 DRSS_rand_prng
#define DRSS_rand(...) EXPAND(CONCATENATE(DRSS_rand_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


/////////////////////////////////////////////
// Random secret shares of zero
/////////////////////////////////////////////

// Random ASS secret share of zero
void ASS_zero_urandom(ASS A, const subset_S S);
void ASS_zero_prng(ASS A, const subset_S S, prng_seed seed);

#define ASS_zero_2 ASS_zero_urandom
#define ASS_zero_3 ASS_zero_prng
#define ASS_zero(...) EXPAND(CONCATENATE(ASS_zero_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Random RSS secret share of zero
void RSS_zero_urandom(RSS A);
void RSS_zero_prng(RSS A, prng_seed seed);

#define RSS_zero_1 RSS_zero_urandom
#define RSS_zero_2 RSS_zero_prng
#define RSS_zero(...) EXPAND(CONCATENATE(RSS_zero_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Random DRSS secret share of zero
void DRSS_zero_urandom(DRSS A);
void DRSS_zero_prng(DRSS A, prng_seed seed);

#define DRSS_zero_1 DRSS_zero_urandom
#define DRSS_zero_2 DRSS_zero_prng
#define DRSS_zero(...) EXPAND(CONCATENATE(DRSS_zero_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))



/////////////////////////////////////////////
// Secret share opening
/////////////////////////////////////////////

// Open an ASS secret share
void open_ASS(const ASS A, const subset_S S, f_elm_t a);

// Open an RSS secret share
void open_RSS(const RSS A, f_elm_t a);

// Open a DRSS secret share
void open_DRSS(const DRSS A, f_elm_t a);



/////////////////////////////////////////////
// Secret share distribution to parties
/////////////////////////////////////////////

// Distribute a RSS x to CONST_N parties, each party recieves x_i[i] shares
void distribute_RSS(const RSS x, RSS_i* x_i);

// Distribute a DRSS x to CONST_N parties, each party recieves x_i[i] shares
void distribute_DRSS(const DRSS x, DRSS_i* x_i);

// Distribute a DRSS seed seed to CONST_N parties, each party recieves seed_i[i] shares
void distribute_DRSS_seed(const DRSS_seed seed, DRSS_seed_i seed_i[CONST_N]);



/////////////////////////////////////////////
// Secret share reconstruction from party shares, with verification
/////////////////////////////////////////////

// Reconstructs an ASS x from party shares x_i
void reconstruct_ASS(const ASS_i* A_i, const subset_S S, ASS A);

// Reconstructs a RSS x from party shares x_i, returns 0 if it is well formed, 1 otherwise
int reconstruct_RSS(const RSS_i* x_i, RSS x);

// Reconstructs a DRSS x from party shares x_i, returns 0 if it is well formed, 1 otherwise
int reconstruct_DRSS(const DRSS_i* x_i, DRSS x);










/////////////////////////////////////////////
// Generating shared random seeds for generating secret shares of random elements.
/////////////////////////////////////////////

// Generate an RSS randomness seed
void random_RSS_seed_urandom(RSS_seed rseed);
void random_RSS_seed_prng(RSS_seed rseed, prng_seed seed);

#define random_RSS_seed_1 random_RSS_seed_urandom
#define random_RSS_seed_2 random_RSS_seed_prng
#define random_RSS_seed(...) EXPAND(CONCATENATE(random_RSS_seed_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Random DRSS seed
void random_DRSS_seed_urandom(DRSS_seed dseed);
void random_DRSS_seed_prng(DRSS_seed dseed, prng_seed seed);

#define random_DRSS_seed_1 random_DRSS_seed_urandom
#define random_DRSS_seed_2 random_DRSS_seed_prng
#define random_DRSS_seed(...) EXPAND(CONCATENATE(random_DRSS_seed_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


/////////////////////////////////////////////
// Distributing a pregenerated shared random seed for generating random elements
/////////////////////////////////////////////

// Distribute an ASS seed aseed to CONST_N parties, each party recieves aseed_i[i] shares
// Not needed, also it's trivial, just generate a random field element locally

// Distribute a DRSS seed rseed to CONST_N parties, each party recieves rseed_i[i] shares
void distribute_RSS_seed(const RSS_seed rseed, RSS_seed_i rseed_i[CONST_N]);

// Distribute a DRSS seed dseed to CONST_N parties, each party recieves dseed_i[i] shares
void distribute_DRSS_seed(const DRSS_seed dseed, DRSS_seed_i dseed_i[CONST_N]);




/////////////////////////////////////////////
// Compute a secret shares of a random element locally from a pre-shared seed
/////////////////////////////////////////////

// Compute an RSS share of a random element locally from seed
void random_RSS_local(RSS_i a, RSS_seed_i rseed_i);

// Compute LAMBDA many RSS shares of a random element locally from seed
void random_RSS_local_array(const int nelms, RSS_i a[nelms], RSS_seed_i rseed_i);






/////////////////////////////////////////////
// Generating shared random seeds for generating secret shares of zero.
/////////////////////////////////////////////

// Generate a randomness seed for creating ASS shares of zero
void random_ASS_seed_zero_urandom(ASS_seed_zero aseedz);
void random_ASS_seed_zero_prng(ASS_seed_zero aseedz, prng_seed seed);

#define random_ASS_seed_zero_1 random_ASS_seed_zero_urandom
#define random_ASS_seed_zero_2 random_ASS_seed_zero_prng
#define random_ASS_seed_zero(...) EXPAND(CONCATENATE(random_ASS_seed_zero_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))

// Generate a randomness seed for creating RSS shares of zero
void random_RSS_seed_zero_urandom(RSS_seed_zero rseedz);
void random_RSS_seed_zero_prng(RSS_seed_zero rseedz, prng_seed seed);

#define random_RSS_seed_zero_1 random_RSS_seed_zero_urandom
#define random_RSS_seed_zero_2 random_RSS_seed_zero_prng
#define random_RSS_seed_zero(...) EXPAND(CONCATENATE(random_RSS_seed_zero_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))


// Generate a randomness seed for creating DRSS shares of zero
void random_DRSS_seed_zero_urandom(DRSS_seed_zero dseedz);
void random_DRSS_seed_zero_prng(DRSS_seed_zero dseedz, prng_seed seed);

#define random_DRSS_seed_zero_1 random_DRSS_seed_zero_urandom
#define random_DRSS_seed_zero_2 random_DRSS_seed_zero_prng
#define random_DRSS_seed_zero(...) EXPAND(CONCATENATE(random_DRSS_seed_zero_, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__))



/////////////////////////////////////////////
// Distributing a pregenerated shared random seed for generating shares of zero
/////////////////////////////////////////////

// Distribute an ASS_seed_zero aseed to CONST_N parties, each party recieves aseed_i[i] shares
void distribute_ASS_seed_zero(const ASS_seed_zero aseedz, ASS_seed_zero_i aseedz_i[CONST_N]);

// Distribute an RSS_seed_zero rseed to CONST_N parties, each party recieves rseed_i[i] shares
void distribute_RSS_seed_zero(const RSS_seed_zero rseedz, RSS_seed_zero_i rseedz_i[CONST_N]);

// Distribute an DRSS_seed_zero rseed to CONST_N parties, each party recieves rseed_i[i] shares
void distribute_DRSS_seed_zero(const DRSS_seed_zero dseedz, DRSS_seed_zero_i dseedz_i[CONST_N]);



/////////////////////////////////////////////
// Compute secret shares of zero locally from a pre-shared seed
/////////////////////////////////////////////

// Compute an ASS share of zero locally from seed
void ASS_zero_local(const subset_S S, const int i, ASS_i a, ASS_seed_zero_i aseedz_i);

// Compute LAMBDA many ASS shares of zero locally from seed
void ASS_zero_local_array(const subset_S S, const int i, const int nelms, f_elm_t a[nelms], ASS_seed_zero_i aseedz_i);

// Compute an RSS share of zero locally from seed
void RSS_zero_local(const int i, RSS_i a, RSS_seed_zero_i rseedz_i);

// Compute LAMBDA many RSS shares of zero locally from seed
void RSS_zero_local_array(const int i, const int nelms, f_elm_t a[TAU_i][nelms], RSS_seed_zero_i rseedz_i);

// Compute an DRSS share of zero locally from seed
void DRSS_zero_local(const int i, DRSS_i a, DRSS_seed_zero_i dseedz_i);

// Compute LAMBDA many DRSS shares of zero locally from seed
void DRSS_zero_local_array(const int i, const int nelms, f_elm_t a[TAU_i * TAU_i][nelms], DRSS_seed_zero_i dseedz_i);







// Multiply two RSS shares to obtain an ASS secret sharing
void mul_RSS_ASS(const RSS_i a, const RSS_i b, ASS_i c);

// Multiply to RSS shares to obtain a DRSS secret sharing
void mul_RSS_DRSS(const RSS_i a, const RSS_i b, DRSS_i c);





// Print RSS secret sharing and the distribution to n parties
// For debugging purposes and checking if shares are ok
unsigned char test_RSS_print(const RSS x, const RSS_i* x_i);



// Maliciously secure RSS to RSS multiplication
// First stage, multiplying and sharing terms
void mult_RSS_to_RSS_evaluation(const RSS_i a, const RSS_i b, RSS C[TAU_i * TAU_i], const DRSS_seed_i seed);
// Second stage, receiving and reconstructing output
int mult_RSS_to_RSS_reconstruction(const RSS_i C_j[CONST_N][TAU_i * TAU_i], RSS_i c);


#endif