#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "arith.h"
#include "bench.h"






#define RED_TESTS   3
#define ADD_TESTS   3
#define NEG_TESTS   3
#define SUB_TESTS   5
#define MUL_TESTS   5
#define LEG_TESTS   3
#define INV_TESTS   2
#define SQRT_TESTS  1
#define RNG_TESTS   0
#define RHS_TESTS   0
#define NUM_TESTS   10

#define TESTS_PAD   MAX(MAX(MAX(RED_TESTS, ADD_TESTS), MAX(NEG_TESTS, SUB_TESTS)), MAX(MAX(MUL_TESTS, LEG_TESTS), MAX(INV_TESTS, SQRT_TESTS)))
#define TESTS_LEN(x) (  (x) == 0 ? RED_TESTS  : \
                        (x) == 1 ? ADD_TESTS  : \
                        (x) == 2 ? NEG_TESTS  : \
                        (x) == 3 ? SUB_TESTS  : \
                        (x) == 4 ? MUL_TESTS  : \
                        (x) == 5 ? LEG_TESTS  : \
                        (x) == 6 ? INV_TESTS  : \
                        (x) == 7 ? SQRT_TESTS : \
                        (x) == 8 ? RNG_TESTS  : \
                        (x) == 9 ? RHS_TESTS  : -1)


const char* pass_check(unsigned char arr[], int n) {
    static char buffer[256] = "";
    buffer[0] = '\0';
    for(int i = 0; i < TESTS_PAD - n; i++)
        strcat(buffer, " ");
    for(int i = 0; i < n; i++)
        strcat(buffer, PASS(arr[i]));
    return buffer;
}



static inline void bench_fun(int function_selector, f_elm_t *t0, f_elm_t* t1, unsigned char* s, int i){
    switch (function_selector) {
        case 0:
            f_red(t0[i]);
            break;
        case 1:
            f_add(t0[i], t1[i], t1[i]);
            break;
        case 2:
            f_neg(t0[i], t1[i]);
            break;
        case 3:
            f_sub(t0[i], t1[i], t1[i]);
            break;
        case 4:
            f_mul(t0[i], t1[i], t1[i]);
            break;
        case 5:
            f_leg(t0[i], s + i);
            break;
        case 6:
            f_inv(t0[i], t1[i]);
            break;
        case 7:
            f_sqrt(t0[i], t1[i]);
            break;
        case 8:
            f_rand_urandom(t0[i]);
            break;
        case 9:
            f_rand_prng(t0[i], (unsigned char *)t1[i]);
            break;
        default:
            printf("Invalid function_selector. Please choose 1 for f_add, 2 for f_neg, 3 for f_sub, 4 for f_mul, 5 for f_leg, 6 for f_inv.\n");
    }
}







int main(int argc, char* argv[]){
    // Remove compilation warnings
    (void)argv[0];
    (void)argc;

    const char *function_names[] = {"f_redc", "f_adds", "f_neg ", "f_subc", "f_mult", "f_leg ", "f_inv ", "f_sqrt", "f_rand", "f_prng"};
    int function_selector;
    unsigned char *s = calloc(BENCH_LOOPS, sizeof(unsigned char));    
    unsigned char f;
    f_elm_t *t0 = malloc(BENCH_LOOPS * sizeof(f_elm_t));    
    f_elm_t *t1 = malloc(BENCH_LOOPS * sizeof(f_elm_t));
    f_elm_t *t2 = malloc(BENCH_LOOPS * sizeof(f_elm_t));
    f_elm_t s0, s1, s2;


    // Randomness seed for replicating experiments
    prng_seed seed;
    randombytes(seed, NBYTES_SEED);     // Comment out if loading seed.
    printf("\n"); print_seed(seed); printf("\n");


    // Assign random field values
    f_rand_array(t0, BENCH_LOOPS, seed);
    f_rand_array(t1, BENCH_LOOPS, seed);
    f_rand_array(t2, BENCH_LOOPS, seed);


    // Arithmetic tests
    f_elm_t max_val; for(int i = 0; i < WORDS_FIELD; i++) max_val[i] = 0xFFFFFFFFFFFFFFFF;
    f_elm_t Rm1; for(int i = 1; i < WORDS_FIELD; i++) Rm1[i] = Mont_one[i]; Rm1[0] = Mont_one[0] - 1;
    unsigned char tests[NUM_TESTS][256] = {0};

    for(int i = 0; i < TEST_LOOPS; i++){

        // Corner cases t = 0 or t = p-1
        if(i == 0){ f_copy(Zero, t0[i]);   f_copy(Zero, t1[i]);   f_copy(Zero, t2[i]); }
        if(i == 1){ f_copy(Zero, t0[i]);   f_copy(Zero, t1[i]);   f_copy(pm1,  t2[i]); }
        if(i == 2){ f_copy(Zero, t0[i]);   f_copy(pm1,  t1[i]);   f_copy(Zero, t2[i]); }
        if(i == 3){ f_copy(Zero, t0[i]);   f_copy(pm1,  t1[i]);   f_copy(pm1,  t2[i]); }
        if(i == 4){ f_copy(pm1,  t0[i]);   f_copy(Zero, t1[i]);   f_copy(Zero, t2[i]); }
        if(i == 5){ f_copy(pm1,  t0[i]);   f_copy(Zero, t1[i]);   f_copy(pm1,  t2[i]); }
        if(i == 6){ f_copy(pm1,  t0[i]);   f_copy(pm1,  t1[i]);   f_copy(Zero, t2[i]); }
        if(i == 7){ f_copy(pm1,  t0[i]);   f_copy(pm1,  t1[i]);   f_copy(pm1,  t2[i]); }

        // Reduction check
        f_copy(t0[i], s0); f_red(s0);                   // s0 = t0 % p, t0 should already be reduced
        tests[0][0] |= f_neq(s0, t0[i]);

        f_copy(max_val, s0); f_red(s0);                 // s0 = (R - 1) % p
        tests[0][1] |= f_neq(s0, Rm1);

        f_copy(p, s0); f_red(s0);                       // s0 = p % p
        tests[0][2] |= f_neq(s0, Zero);


        // Addition check
        f_add(t0[i], t1[i], s0); f_add(s0, t2[i], s0);  // s0 = (t0 + t1) + t2
        f_add(t1[i], t2[i], s1); f_add(t0[i], s1, s1);  // s1 = t0 + (t1 + t2)
        tests[1][0] |= f_neq(s0, s1);

        f_add(t0[i], t1[i], s0);                        // s0 = t0 + t1
        f_add(t1[i], t0[i], s1);                        // s1 = t1 + t0
        tests[1][1] |= f_neq(s0, s1);

        f_add(t0[i], Zero, s0);                         // s0 = t0 + 0
        tests[1][2] |= f_neq(s0, t0[i]);


        // Negation check
        f_neg(t0[i], s0); f_add(s0, t0[i], s1);         // s0 = -t0 + t0
        tests[2][0] |= f_neq(s1, Zero);

        f_neg(t0[i], s0); f_add(t0[i], s0, s1);         // s0 = t0 - t0
        tests[2][1] |= f_neq(s1, Zero);

        f_neg(Zero, s1);                                // s0 = -0
        tests[2][2] |= f_neq(s1, Zero);


        // Subtraction check
        f_sub(t0[i], t1[i], s0); f_sub(s0, t2[i], s0);  // s0 = (t0 - t1) - t2
        f_sub(t0[i], t2[i], s1); f_sub(s1, t1[i], s1);  // s1 = (t0 - t2) - t1
        f_add(t1[i], t2[i], s2); f_sub(t0[i], s2, s2);  // s2 = t0 - (t1 + t2)
        tests[3][0] |= f_neq(s0, s1);
        tests[3][1] |= f_neq(s0, s2);

        f_sub(t0[i], t1[i], s0);                        // s0 = t0 - t1
        f_sub(t1[i], t0[i], s1); f_neg(s1, s1);         // s1 = -(t1 - t0)
        tests[3][2] |= f_neq(s0, s1);

        f_sub(t0[i], Zero, s0);                         // s0 = t0 - 0
        tests[3][3] |= f_neq(s0, t0[i]);

        f_sub(Zero, t0[i], s0);                         // s0 = 0 - t0
        f_neg(t0[i], s1);                               // s1 = -t0
        tests[3][4] |= f_neq(s0, s1);


        // Multiplication checks
        f_add(t0[i], t1[i], s0); f_mul(s0, t2[i], s0);                       // s0 = (t0 + t1) * t2
        f_mul(t0[i], t2[i], s1); f_mul(t1[i], t2[i], s2); f_add(s1, s2, s1); // s1 = t0 * t2 + t1 * t2
        tests[4][0] |= f_neq(s0, s1);

        f_mul(t0[i], t1[i], s0); f_mul(s0, t2[i], s0);  // s0 = (t0 * t1) * t2
        f_mul(t1[i], t2[i], s1); f_mul(t0[i], s1, s1);  // s1 = t0 * (t1 * t2)
        tests[4][1] |= f_neq(s0, s1);

        f_mul(t0[i], t1[i], s0);                        // s0 = t0 * t1
        f_mul(t1[i], t0[i], s1);                        // s1 = t1 * t0
        tests[4][2] |= f_neq(s0, s1);

        f_mul(t0[i], Mont_one, s0);                     // s0 = t0 * 1
        tests[4][3] |= f_neq(s0, t0[i]);

        f_mul(t0[i], Zero, s0);                         // s0 = t0 * 0
        tests[4][4] |= f_neq(s0, Zero);


        // Legendre check
        f_leg(t0[i], s);                                                // s   = f_leg(t0)
        f_mul(t1[i], t1[i], s1); f_mul(s1, t0[i], s1); f_leg(s1, s+1);  // s+1 = f_leg(t0 * t1 * t1)
        if((f_neq(t0[i], Zero)) && (f_neq(t1[i], Zero)))    // Ignore input t0 = 0 or t1 = 0
        tests[5][0] |= s[0] ^ s[1];

        f_leg(t0[i], s);                                    // s   = f_leg(t0)
        f_leg(t1[i], s+1);                                  // s+1 = f_leg(t1)
        f_mul(t0[i], t1[i], s0); f_leg(s0, s+2);            // s+2 = f_leg(t0 * t1)
        if((f_neq(t0[i], Zero)) && (f_neq(t1[i], Zero)))    // Ignore input t0 = 0 or t1 = 0
        tests[5][1] |= s[0] ^ s[1] ^ s[2];

        if(i == 8) f_leg(t0[i], &f);
        if(i >= 8) // Check that all symbols are not equal (to some f = f_leg(t0[8])) 
            tests[5][2] |= (f ^ s[0]) | (f ^ s[1]);         // Skip over corner cases when t0, t1 can be = 0

        if(i == TEST_LOOPS - 1) tests[5][2] = !tests[5][2]; // If they were all equal result would be 0.


        // Inverse check
        f_inv(t0[i], s0);                       // s0 = t0^(-1)
        f_mul(s0, t0[i], s0);                   // s0 = s0 * s0^(-1)
        if(f_neq(t0[i], Zero))                  // Ignore input t0 = 0
        tests[6][0] |= f_neq(s0, Mont_one);

        f_inv(t0[i], s0);                       // s0 = t0^(-1)
        f_inv(s0, s1);                          // s1 = s0^(-1) = t0
        tests[6][1] |= f_neq(s1, t0[i]);


        // Square root test
        f_sqrt(t0[i], s0);                   // s0 = sqrt(t0)
        f_mul(s0, s0, s0);                   // s0 = s0^2 = t0
        f_leg(t0[i], s);
        if(!*s)                              // Test if t0 is a square
        tests[7][0] |= f_neq(t0[i], s0);

    }



    uint64_t nsecs_pre, nsecs_post, nsecs;

    // WARM UP
    for(int i = 0; i < WARMUP; i++){
        f_mul(t0[i], t1[i], t1[i]);
    }


    // BENCHMARKING
    printf("%d bits\n\n", NBITS_FIELD);

    for(int j = 0; j < NUM_TESTS; j++){
        function_selector = j;
        nsecs_pre = 0, nsecs_post = 0, nsecs = 0;

        for(int i = 0; i < BENCH_LOOPS; ){
            nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
            bench_fun(function_selector, t0, t1, s, i); i++;
            bench_fun(function_selector, t0, t1, s, i); i++;
            bench_fun(function_selector, t0, t1, s, i); i++;
            bench_fun(function_selector, t0, t1, s, i); i++;
            bench_fun(function_selector, t0, t1, s, i); i++;
            bench_fun(function_selector, t0, t1, s, i); i++;
            bench_fun(function_selector, t0, t1, s, i); i++;
            bench_fun(function_selector, t0, t1, s, i); i++;
            bench_fun(function_selector, t0, t1, s, i); i++;
            bench_fun(function_selector, t0, t1, s, i); i++;
            nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
            nsecs += (nsecs_post-nsecs_pre);
        }
        // printf("%s runs in ......................... %9" PRIu64 " cycles\n", function_names[function_selector], (uint64_t)(((double)(nsecs/BENCH_LOOPS)*CLK_PER_nSEC)));
        printf("%s check   ........................      %s\n", function_names[function_selector], pass_check(tests[j], TESTS_LEN(j)));
        printf("%s runs in ........................ %7s ns\n", function_names[function_selector], print_num((double)(nsecs/BENCH_LOOPS)));
        printf("\n");
    }
    printf("..................................................\n\n");

    free(t0);
    free(t1);
    free(s);



    return 0;
}

