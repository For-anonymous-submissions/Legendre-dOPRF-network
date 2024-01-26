#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "arith.h"
#include "bench.h"



#define MAX(a,b)    (((a)>(b))?(a):(b))

#define RED_TESTS   3
#define ADD_TESTS   4
#define NEG_TESTS   2
#define SUB_TESTS   4
#define MUL_TESTS   5
#define LEG_TESTS   2
#define INV_TESTS   2
#define NUM_TESTS   7

#define TESTS_PAD   MAX(MAX(MAX(RED_TESTS, ADD_TESTS), MAX(NEG_TESTS, SUB_TESTS)), MAX(MAX(MUL_TESTS, LEG_TESTS), MAX(INV_TESTS, 1)))
#define TESTS_LEN(x) (  (x) == 0 ? RED_TESTS :\
                        (x) == 1 ? ADD_TESTS :\
                        (x) == 2 ? NEG_TESTS :\
                        (x) == 3 ? SUB_TESTS :\
                        (x) == 4 ? MUL_TESTS :\
                        (x) == 5 ? LEG_TESTS :\
                        (x) == 6 ? INV_TESTS : -1)


#define PASS(x)                 ((x) ? "\033[31m✗\033[0m" : "\033[0;32m✔\033[0m")

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
        default:
            printf("Invalid function_selector. Please choose 1 for f_add, 2 for f_neg, 3 for f_sub, 4 for f_mul, 5 for f_leg, 6 for f_inv.\n");
    }
}








int main(int argc, char* argv[]){
    // Remove compilation warnings
    (void)argv[0];
    (void)argc;

    const char *function_names[] = {"f_red", "f_add", "f_neg", "f_sub", "f_mul", "f_leg", "f_inv"};
    int function_selector;
    unsigned char *s = calloc(BENCH_LOOPS, sizeof(unsigned char));    
    f_elm_t *t0 = malloc(BENCH_LOOPS * sizeof(f_elm_t));    
    f_elm_t *t1 = malloc(BENCH_LOOPS * sizeof(f_elm_t));
    f_elm_t *t2 = malloc(BENCH_LOOPS * sizeof(f_elm_t));
    f_elm_t s0, s1, s2;

    // Assign random field values
    for(int i = 0; i < BENCH_LOOPS; i++){
        f_rand(t0[i]);
        f_rand(t1[i]);
        f_rand(t2[i]);
    }


    // Arithmetic tests
    #if (NBITS_FIELD == 256)
    f_elm_t max_val = {0}; max_val[0] = 0x0000000000000012; max_val[WORDS_FIELD - 1] = 0x8000000000000000; // R - 1 - p because R > 2*p only for 256 bits
    #else
    f_elm_t max_val; for(int i = 0; i < WORDS_FIELD; i++) max_val[i] = 0xFFFFFFFFFFFFFFFF;
    #endif
    f_elm_t Rm1; for(int i = 1; i < WORDS_FIELD; i++) Rm1[i] = Mont_one[i]; Rm1[0] = Mont_one[0] - 1;
    unsigned char tests[7][256] = {0};

    for(int i = 0; i < TEST_LOOPS; i++){
        // Reduction check
        f_copy(t0[i], s0); f_red(s0);                   // s0 = t0 % p, t0 should already be reduced
        tests[0][0] |= f_eq(s0, t0[i]);

        f_copy(max_val, s0); f_red(s0);                 // s0 = (R - 1) % p
        tests[0][1] |= f_eq(s0, Rm1);

        f_copy(p, s0); f_red(s0);                       // s0 = p % p
        tests[0][2] |= f_eq(s0, Zero);


        // Addition check
        f_add(t0[i], t1[i], s0); f_add(s0, t2[i], s0);  // s0 = (t0 + t1) + t2
        f_add(t1[i], t2[i], s1); f_add(t0[i], s1, s1);  // s1 = t0 + (t1 + t2)
        tests[1][0] |= f_eq(s0, s1);

        f_add(t0[i], t1[i], s0);                        // s0 = t0 + t1
        f_add(t1[i], t0[i], s1);                        // s1 = t1 + t0
        tests[1][1] |= f_eq(s0, s1);

        f_add(t0[i], Zero, s0);                         // s0 = t0 + 0
        tests[1][2] |= f_eq(s0, t0[i]);

        f_neg(t0[i], s0); f_add(s0, t0[i], s1);         // s0 = -t0 + t0
        tests[1][3] |= f_eq(s1, Zero);


        // Negation check
        f_neg(t0[i], s0); f_add(t0[i], s0, s1);         // s0 = t0 - t0
        tests[2][0] |= f_eq(s1, Zero);

        f_neg(Zero, s1);                                // s0 = -0
        tests[2][1] |= f_eq(s1, Zero);


        // Subtraction check
        f_sub(t0[i], t1[i], s0); f_sub(s0, t2[i], s0);  // s0 = (t0 - t1) - t2
        f_add(t1[i], t2[i], s1); f_sub(t0[i], s1, s1);  // s1 = t0 - (t1 + t2)
        tests[3][0] |= f_eq(s0, s1);

        f_sub(t0[i], t1[i], s0);                        // s0 = t0 - t1
        f_sub(t1[i], t0[i], s1); f_neg(s1, s1);         // s1 = -(t1 - t0)
        tests[3][1] |= f_eq(s0, s1);

        f_sub(t0[i], Zero, s0);                         // s0 = t0 - 0
        tests[3][2] |= f_eq(s0, t0[i]);

        f_sub(Zero, t0[i], s0);                         // s0 = 0 - t0
        f_neg(t0[i], s1);                               // s1 = -t0
        tests[3][3] |= f_eq(s0, s1);


        // Multiplication checks
        f_add(t0[i], t1[i], s0); f_mul(s0, t2[i], s0);                       // s0 = (t0 + t1) * t2
        f_mul(t0[i], t2[i], s1); f_mul(t1[i], t2[i], s2); f_add(s1, s2, s1); // s1 = t0 * t2 + t1 * t2
        tests[4][0] |= f_eq(s0, s1);

        f_mul(t0[i], t1[i], s0); f_mul(s0, t2[i], s0);  // s0 = (t0 * t1) * t2
        f_mul(t1[i], t2[i], s1); f_mul(t0[i], s1, s1);  // s1 = t0 * (t1 * t2)
        tests[4][1] |= f_eq(s0, s1);

        f_mul(t0[i], t1[i], s0);                        // s0 = t0 * t1
        f_mul(t1[i], t0[i], s1);                        // s1 = t1 * t0
        tests[4][2] |= f_eq(s0, s1);

        f_mul(t0[i], Mont_one, s0);                     // s0 = t0 * 1
        tests[4][3] |= f_eq(s0, t0[i]);
        
        f_mul(t0[i], Zero, s0);                         // s0 = t0 * 0
        tests[4][4] |= f_eq(s0, Zero);


        // Legendre check
        f_leg(t0[i], s);                                                // s   = f_leg(t0)
        f_mul(t1[i], t1[i], s1); f_mul(s1, t0[i], s1); f_leg(s1, s+1);  // s+1 = f_leg(t0 * t1 * t1)
        tests[5][0] |= s[0] ^ s[1];

        f_leg(t0[i], s);                                // s   = f_leg(t0)
        f_leg(t1[i], s+1);                              // s+1 = f_leg(t1)
        f_mul(t0[i], t1[i], s0); f_leg(s0, s+2);        // s+2 = f_leg(t0 * t1)
        tests[5][1] |= s[0] ^ s[1] ^ s[2];
        

        // Inverse check
        f_inv(t0[i], s0);                               // s0 = t0^(-1)
        f_mul(s0, t0[i], s0);                           // s0 = s0 * s0^(-1)
        tests[6][0] |= f_eq(s0, Mont_one);

        f_inv(t0[i], s0);                               // s0 = t0^(-1)
        f_inv(s0, s1);                                  // s1 = s0^(-1) = t0
        tests[6][1] |= f_eq(s1, t0[i]);

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
        printf("%s check   .........................        %s\n", function_names[function_selector], pass_check(tests[j], TESTS_LEN(j)));
        printf("%s runs in ......................... %9s ns\n", function_names[function_selector], print_num((double)(nsecs/BENCH_LOOPS)));
        printf("\n");
    }
    printf("....................................................\n\n");

    free(t0);
    free(t1);
    free(s);



    return 0;
}






// #else
//     int test_a = 10000, test_b = 10001, test_c = 10003;
//     int s[10003] = {0};
//     mpz_t p, result;
//     mpz_t a[10003], b[10003], c[10003];
//     gmp_randstate_t state;

//     // Initialize variables
//     mpz_init(p);
//     gmp_randinit_default(state);

//     // Define the finite field
//     mpz_set_str(p, "7ffffffffffffffffffffffffffffffffffffed", 16);


//     // Initialize the mpz_t variables in the array
//     for (int i = 0; i < BENCH_LOOPS; i++) {
//         mpz_init(a[i%test_a]);
//         mpz_init(b[i%test_b]);
//         mpz_init(c[i%test_c]);
//         mpz_urandomm(a[i%test_a], state, p);
//         mpz_urandomm(b[i%test_b], state, p);

//     }



//     for(int i = 0; i < WARMUP; i++){
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]);
//     }

//     uint64_t nsecs_pre = 0, nsecs_post = 0, nsecs = 0;
//     for(int i = 0; i < 1E3*BENCH_LOOPS; ){
//         nsecs_pre = cpucycles();

//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;
//         mpz_mul(c[i%test_c], a[i%test_a], b[i%test_b]); i++;

//         nsecs_post = cpucycles();
//         nsecs += (nsecs_post-nsecs_pre);
//     }
//     printf("f_mul runs in ......................... %9s ns\n", print_num((uint64_t)((double)nsecs/(1E3*BENCH_LOOPS))));

//     nsecs_pre = 0, nsecs_post = 0, nsecs = 0;
//     for(int i = 0; i < 1E3*BENCH_LOOPS; ){
//         nsecs_pre = cpucycles();

//         s[i%test_c] = mpz_legendre(a[i%test_a], p); i++;
//         s[i%test_c] = mpz_legendre(a[i%test_a], p); i++;
//         s[i%test_c] = mpz_legendre(a[i%test_a], p); i++;
//         s[i%test_c] = mpz_legendre(a[i%test_a], p); i++;
//         s[i%test_c] = mpz_legendre(a[i%test_a], p); i++;
//         s[i%test_c] = mpz_legendre(b[i%test_b], p); i++;
//         s[i%test_c] = mpz_legendre(b[i%test_b], p); i++;
//         s[i%test_c] = mpz_legendre(b[i%test_b], p); i++;
//         s[i%test_c] = mpz_legendre(b[i%test_b], p); i++;
//         s[i%test_c] = mpz_legendre(b[i%test_b], p); i++;

//         nsecs_post = cpucycles();
//         nsecs += (nsecs_post-nsecs_pre);
//     }
//     printf("f_leg runs in ......................... %9s ns\n", print_num((uint64_t)((double)nsecs/(1E3*BENCH_LOOPS))));

    
//     #endif

