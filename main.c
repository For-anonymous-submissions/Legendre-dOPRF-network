#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "bench.h"
#include "arith.h"
#include "dOPRF.h"


#if (ADVERSARY==SEMIHONEST)
    #define LOOPS       50
    #define START_FUNC  1
    #define END_FUNC    8
#elif(ADVERSARY==MALICIOUS)
    #define LOOPS       30
    #define START_FUNC  9
    #define END_FUNC    16
#endif





// PROTOCOLS


/////////////////////////////////////////////
// SEMI-HONEST SUBPROTOCOLS
/////////////////////////////////////////////




/////////////////////////////////////////////
// SEMI-HONEST SETUP (with distributed secret shares of zero)
/////////////////////////////////////////////
void sh_setup(const f_elm_t k[LAMBDA], f_elm_t s2[LAMBDA], ASS_i r_i[CONST_N][LAMBDA], RSS_i k_i[CONST_N][LAMBDA], RSS_i s2_i[CONST_N][LAMBDA]){
    RSS K, S2;
    RSS_i temp_k_i[LAMBDA][CONST_N], temp_s2_i[LAMBDA][CONST_N];
    ASS temp_r_i[LAMBDA];

    subset_S S = S_N;

    for(int j = 0; j < LAMBDA; j++){
        to_RSS(k[j], K);
        distribute_RSS(K, temp_k_i[j]);

        f_rand(s2[j]);
        to_RSS(s2[j], S2);
        distribute_RSS(S2, temp_s2_i[j]);

        ASS_zero(temp_r_i[j], S);
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


/////////////////////////////////////////////
// SEMI-HONEST SETUP (only distribute seeds, parties compute secret shares of zero locally)
/////////////////////////////////////////////
void sh_tp_setup(prng_seed seed, RSS_i k_i[CONST_N][LAMBDA], RSS_seed_i rseed_i[CONST_N], ASS_seed_zero_i aseedz_i[CONST_N]){
    /////////////////////////////////////////////
    // Random RSS key k (LAMBDA many)
    /////////////////////////////////////////////
    RSS (*k) = malloc(sizeof(RSS[LAMBDA]));
    RSS_i (*k_i_temp)[CONST_N] = malloc(sizeof(RSS_i[LAMBDA][CONST_N]));

    RSS_rand_array(k, seed);

    for(int j = 0; j < LAMBDA; j++)
        distribute_RSS(k[j], k_i_temp[j]);
    for(int i = 0; i < CONST_N; i++)
        for(int j = 0; j < LAMBDA; j++)
            memcpy(k_i[i][j], k_i_temp[j][i], sizeof(RSS_i));

    /////////////////////////////////////////////

    free(k);
    free(k_i_temp);

    /////////////////////////////////////////////
    // Seeds for generating random RSS shares 
    /////////////////////////////////////////////
    prng_seed newseed;
    randombytes(newseed, NBYTES_SEED);

    // RSS
    RSS_seed (*rseed) = malloc(sizeof(RSS_seed));
    // RSS_seed_i (*rseed_i) = malloc(sizeof(RSS_seed_i[CONST_N]));

    random_RSS_seed(*rseed, newseed);
    distribute_RSS_seed(*rseed, rseed_i);

    /////////////////////////////////////////////

    free(rseed);


    /////////////////////////////////////////////
    // Seeds for generating ASS secret shares of zero
    /////////////////////////////////////////////
    // ASS
    ASS_seed_zero (*aseedz) = malloc(sizeof(ASS_seed_zero));
    // ASS_seed_zero_i (*aseedz_i) = malloc(sizeof(ASS_seed_zero_i[CONST_N]));

    random_ASS_seed_zero(*aseedz, newseed);
    distribute_ASS_seed_zero(*aseedz, aseedz_i);
    free(aseedz);

    /////////////////////////////////////////////
}


/////////////////////////////////////////////
// SEMI-HONEST ZERO GENERATION
/////////////////////////////////////////////
void sh_setup_zeros(const int i, ASS_i az_i[2][LAMBDA], ASS_seed_zero_i aseedz_i){
    f_elm_t (*az_i_t)  = malloc(sizeof(f_elm_t[2][LAMBDA]));
    subset_S S = S_N;

    ASS_zero_local_array(S, i, 2*LAMBDA, az_i_t, aseedz_i);

    memcpy(az_i, az_i_t, sizeof(ASS_i[2][LAMBDA]));

    free(az_i_t);
}


/////////////////////////////////////////////
// First phase - Computing the product
// Parties compute first part of local RSS->RSS mult and send outputs
/////////////////////////////////////////////
void sh_setup_mul_1(const ASS_i az_i[LAMBDA], RSS_i c_i[LAMBDA][CONST_N], RSS_seed_i rseed_i){
    prng_seed seed;
    randombytes(seed, NBYTES_SEED);

    RSS (*ct)  = malloc(sizeof(RSS[LAMBDA]));
    RSS_i (*s) = malloc(sizeof(RSS_i[LAMBDA]));
    ASS_i (*c) = malloc(sizeof(ASS_i[LAMBDA]));

    random_RSS_local_array(LAMBDA, s, rseed_i);

    for(int j = 0; j < LAMBDA; j++)
        mul_RSS_ASS(s[j], s[j], c[j]);

    free(s);

    for(int j = 0; j < LAMBDA; j++)
        f_add(c[j][0], az_i[j][0], c[j][0]);

    for(int j = 0; j < LAMBDA; j++)
        to_RSS(c[j][0], ct[j], seed);

    free(c);

    for(int j = 0; j < LAMBDA; j++){
        ind_T T_ind[CONST_N] = {0};
        for(int i = 0; i < CONST_N; i++){
            for(share_T T = T_0; T.ind < TAU; next_T(&T)){
                if(i_hold_T(i, T)){
                    f_copy(ct[j][T.ind], c_i[j][i][T_ind[i]++]);
                }
            }
        }
    }
    free(ct);
}


/////////////////////////////////////////////
// Re-indexes the outputs so that
// output of party i is sent to party j 
// Not needed in real life execution
// Just in the simulation where we simulate all parties
/////////////////////////////////////////////
void sh_setup_reindex_outputs(RSS_i c[CONST_N][LAMBDA][CONST_N]){
    // f_elm_t c_i[party i that computes output][LAMBDA][party j that receives from i][TAU_i * TAU_i shares of computer][TAU_j * TAU_j shares of receiver]
    RSS_i (*temp) = malloc(sizeof(RSS_i));

    for(int i = 0; i < CONST_N; i++){
        for(int l = 0; l < LAMBDA; l++){
            for(int j = 0; j < i; j++){
                memcpy(temp,        c[j][l][i], sizeof(RSS_i));
                memcpy(c[j][l][i],  c[i][l][j], sizeof(RSS_i));
                memcpy(c[i][l][j],  temp,       sizeof(RSS_i));
            }
        }
    }
    free(temp);
}


/////////////////////////////////////////////
// Second phase - reconstructing the product
// Parties receive outputs and reconstruct the output
/////////////////////////////////////////////
void sh_setup_mul_2(const RSS_i c_i[LAMBDA][CONST_N], RSS_i s2_i[LAMBDA]){
    memset(s2_i, 0, sizeof(RSS_i[LAMBDA]));

    for(int j = 0; j < LAMBDA; j++){
        for(share_T T = T_0; T.ind < TAU_i; next_T(&T)){
            f_elm_t t0 = {0};
            for(int i = 0; i < CONST_N; i++)
                f_add(c_i[j][i][T.ind], t0, t0);
            f_copy(t0, s2_i[j][T.ind]);
        }
    }
}


/////////////////////////////////////////////
// INPUT (SEMI-HONEST AND MALICIOUS)
/////////////////////////////////////////////
void input(const f_elm_t x, RSS_i x_i[CONST_N], prng_seed seed){
    RSS X;
    to_RSS(x, X, seed);
    distribute_RSS(X, x_i);
}

/////////////////////////////////////////////
// SEMI-HONEST EVALUATION
/////////////////////////////////////////////
void sh_evaluation(const RSS_i x_i, const RSS_i k_i[LAMBDA], const RSS_i s2_i[LAMBDA], const ASS_i az_i[LAMBDA], ASS_i o_i[LAMBDA]){
    RSS_i a_i;
    for(int j = 0; j < LAMBDA; j++){
        add_RSS_i(x_i, k_i[j], a_i);
        mul_RSS_ASS(a_i, s2_i[j], o_i[j]);
        f_add(o_i[j][0], az_i[j][0], o_i[j][0]);
    }
}

/////////////////////////////////////////////
// SEMI-HONEST RECONSTRUCTION
/////////////////////////////////////////////
void sh_reconstruction(const ASS_i o_i[CONST_N][LAMBDA], f_elm_t o[LAMBDA]){
    ASS o_j;
    subset_S S = S_N;

    for(int j = 0; j < LAMBDA; j++){
        for(int i = 0; i < CONST_N; i++)
            f_copy(o_i[i][j][0], o_j[i]);
        open_ASS(o_j, S, o[j]);
    }
}

/////////////////////////////////////////////
// SEMI-HONEST PROTOCOL
/////////////////////////////////////////////
unsigned char sh_protocol(prng_seed seed){


    // SERVER PRECOMPUTATION
    f_elm_t k[LAMBDA];
    RSS_i k_i[CONST_N][LAMBDA];

    f_elm_t s2[LAMBDA];
    RSS_i s2_i[CONST_N][LAMBDA];
    ASS_i az_i[CONST_N][LAMBDA];
    
    for(int j = 0; j < LAMBDA; j++)
        f_rand(k[j]);

    sh_setup(k, s2, az_i, k_i, s2_i);


    // CLIENT INPUT STAGE
    f_elm_t x;
    RSS_i x_i[CONST_N];

    f_rand(x);
    input(x, x_i, seed);


    // SERVERS EVALUATION STAGE
    ASS_i o_i[CONST_N][LAMBDA];

    for(int i = 0; i < CONST_N; i++)
        sh_evaluation(x_i[i], k_i[i], s2_i[i], az_i[i], o_i[i]);

 
    // CLIENT RECONSTRUCTION STAGE
    f_elm_t o[LAMBDA];
    sh_reconstruction(o_i, o);


    unsigned char test = 0;
    f_elm_t z;  

    for(int j = 0; j < LAMBDA; j++){
        f_add(x, k[j], z);
        f_mul(z, s2[j], z);        
        test |= f_neq(z, o[j]);
    }


    unsigned char L[PRF_BYTES];

    calc_symbols(o, L);

    return(test);
}




/////////////////////////////////////////////
// SEMI-HONEST PROTOCOL
/////////////////////////////////////////////
unsigned char sh_protocol_full(prng_seed seed){
    int test = 0;

    prng_seed curr_seed;
    memcpy(curr_seed, seed, NBYTES_SEED);

    ////////////////////////////////////////////
    // Generate key
    // Same key as inside sh_tp_setup
    ////////////////////////////////////////////

    RSS (*rk_temp) = malloc(sizeof(RSS[LAMBDA]));
    f_elm_t (*k) = malloc(sizeof(f_elm_t[LAMBDA]));
    
    RSS_rand_array(rk_temp, curr_seed);

    for(int j = 0; j < LAMBDA; j++)
        open_RSS(rk_temp[j], k[j]);

    memcpy(curr_seed, seed, NBYTES_SEED);
    
    ////////////////////////////////////////////
    free(rk_temp);

    
    ////////////////////////////////////////////
    // TRUSTED PARTY SEED AND KEY DISTRIBUTION
    ////////////////////////////////////////////

    RSS_i (*k_i)[LAMBDA] = malloc(sizeof(RSS_i[CONST_N][LAMBDA]));
    RSS_seed_i (*rseed_i) = malloc(sizeof(RSS_seed_i[CONST_N]));
    ASS_seed_zero_i (*aseedz_i) = malloc(sizeof(ASS_seed_zero_i[CONST_N]));

    sh_tp_setup(seed, k_i, rseed_i, aseedz_i);

    ////////////////////////////////////////////


    ////////////////////////////////////////////
    // LOCAL SETUP STAGE
    ////////////////////////////////////////////

    ASS_i (*az_i)[2][LAMBDA]  = malloc(sizeof(ASS_i[CONST_N][2][LAMBDA]));

    ////////////////////////////////////////////
    // Compute all secret shares of zero to be used later
    
    for(int i = 0; i < CONST_N; i++)
        sh_setup_zeros(i, az_i[i], aseedz_i[i]);

    ////////////////////////////////////////////
    free(aseedz_i);


    ////////////////////////////////////////////
    // First step of RSS multiplication

    RSS_i (*c_i)[LAMBDA][CONST_N] = malloc(sizeof(RSS_i[CONST_N][LAMBDA][CONST_N]));


    for(int i = 0; i < CONST_N; i++)
        sh_setup_mul_1(az_i[i][0], c_i[i], rseed_i[i]);

    ////////////////////////////////////////////
    free(rseed_i);


    ////////////////////////////////////////////
    // Re-indexing, i.e., sending messages
    
    sh_setup_reindex_outputs(c_i);


    ////////////////////////////////////////////
    // Second step of RSS multiplication

    RSS_i (*s2_i)[LAMBDA] = malloc(sizeof(RSS_i[CONST_N][LAMBDA]));

    for(int i = 0; i < CONST_N; i++)
        sh_setup_mul_2(c_i[i], s2_i[i]);

    ////////////////////////////////////////////
    free(c_i);


    ////////////////////////////////////////////
    // CLIENT INPUT STAGE
    ////////////////////////////////////////////

    f_elm_t x;
    RSS_i (*x_i) = malloc(sizeof(RSS_i[CONST_N]));

    f_rand(x, seed);
    input(x, x_i, seed);

    ////////////////////////////////////////////



    ////////////////////////////////////////////
    // SERVERS EVALUATION STAGE
    ////////////////////////////////////////////

    ASS_i (*o_i)[LAMBDA] = malloc(sizeof(ASS_i[CONST_N][LAMBDA]));

    for(int i = 0; i < CONST_N; i++)
        sh_evaluation(x_i[i], k_i[i], s2_i[i], az_i[i][1], o_i[i]);

    ////////////////////////////////////////////
    free(x_i);
    free(k_i);
    free(az_i);


    ////////////////////////////////////////////
    // Reconstruct squares, for testing purposes

    f_elm_t (*s2) = malloc(sizeof(f_elm_t[LAMBDA]));
    RSS (*s2_rss) = malloc(sizeof(RSS[LAMBDA]));
    RSS_i (*s2_i_temp)[CONST_N] = malloc(sizeof(RSS_i[LAMBDA][CONST_N]));
    for(int i = 0; i < CONST_N; i++){
        for(int j = 0; j < LAMBDA; j++)
            memcpy(s2_i_temp[j][i], s2_i[i][j], sizeof(RSS_i));
    }

    for(int j = 0; j < LAMBDA; j++){
        test |= reconstruct_RSS(s2_i_temp[j], s2_rss[j]);
        open_RSS(s2_rss[j], s2[j]);
    }

    ////////////////////////////////////////////
    free(s2_i_temp);
    free(s2_rss);
    free(s2_i);


    ////////////////////////////////////////////
    // CLIENT RECONSTRUCTION STAGE
    ////////////////////////////////////////////
    f_elm_t o[LAMBDA];
    unsigned char L[PRF_BYTES] = {0};

    sh_reconstruction(o_i, o);

    ////////////////////////////////////////////
    // Client computes Legendre symbols
    
    calc_symbols(o, L);


    printf("\n");
    printf("Output: "); print_hex(L, PRF_BYTES);

    ////////////////////////////////////////////
    free(o_i);


    ////////////////////////////////////////////
    // CORRECTNESS CHECK
    ////////////////////////////////////////////

    f_elm_t z;  

    for(int j = 0; j < LAMBDA; j++){
        f_add(x, k[j], z);
        f_mul(z, s2[j], z);        
        test |= f_neq(z, o[j]);
    }

    ////////////////////////////////////////////
    free(s2);
    free(k);

    return(test);
}







/////////////////////////////////////////////
// Generate secret sharings of zero for testing
/////////////////////////////////////////////
int gen_zeros(prng_seed seed){
    int test = 0;

    randombytes(seed, NBYTES_SEED);

    /////////////////////////////////////////////
    // Testing ASS zero generation from seeds
    /////////////////////////////////////////////
    ASS_seed_zero (*aseed) = malloc(sizeof(ASS_seed_zero));
    ASS_seed_zero_i (*aseed_i) = malloc(sizeof(ASS_seed_zero_i[CONST_N]));


    random_ASS_seed_zero(aseed[0], seed);
    distribute_ASS_seed_zero(aseed[0], aseed_i);

    free(aseed);


    ASS_i (*za_i) = malloc(sizeof(ASS_i[CONST_N]));
    ASS za;
    f_elm_t za_f;

    subset_S S;

    for(int i = 0; i < (1 << CONST_N); i++){
        S.bin = i; update_S(&S, 'b');        
        for(int j = 0; j < S.card; j++)
            ASS_zero_local(S, S.com[j], za_i[S.com[j]], aseed_i[S.com[j]]);
        reconstruct_ASS(za_i, S, za);
        open_ASS(za, S, za_f);
        test |= f_neq(za_f, Zero);
        // for(int j = 0; j < S.card; j++){
        //     printf("%d ", S.com[j]);
        //     print_f_elm(za_i[S.com[j]][0]);
        // }
        // printf("\n");
    }


    free(aseed_i);
    free(za_i);





    /////////////////////////////////////////////
    // Testing RSS zero generation from seeds
    /////////////////////////////////////////////
    RSS_seed_zero (*rseed) = malloc(sizeof(RSS_seed_zero));
    RSS_seed_zero_i (*rseed_i) = malloc(sizeof(RSS_seed_zero_i[CONST_N]));


    random_RSS_seed_zero(rseed[0], seed);
    distribute_RSS_seed_zero(rseed[0], rseed_i);

    free(rseed);


    RSS_i (*zr_i) = malloc(sizeof(RSS_i[CONST_N]));
    RSS zr;
    f_elm_t zr_f;

    for(int i = 0; i < CONST_N; i++)
        RSS_zero_local(i, zr_i[i], rseed_i[i]);


    test |= reconstruct_RSS(zr_i, zr);

    open_RSS(zr, zr_f);

    test |= f_neq(zr_f, Zero);


    free(rseed_i);
    free(zr_i);



    /////////////////////////////////////////////
    // Testing DRSS zero generation from seeds
    /////////////////////////////////////////////
    DRSS_seed_zero (*dseed) = malloc(sizeof(DRSS_seed_zero));
    DRSS_seed_zero_i (*dseed_i) = malloc(sizeof(DRSS_seed_zero_i[CONST_N]));


    random_DRSS_seed_zero(dseed[0], seed);
    distribute_DRSS_seed_zero(dseed[0], dseed_i);

    free(dseed);


    DRSS_i (*zd_i) = malloc(sizeof(DRSS_i[CONST_N]));
    DRSS zd;
    f_elm_t zd_f;

    for(int i = 0; i < CONST_N; i++)
        DRSS_zero_local(i, zd_i[i], dseed_i[i]);


    test |= reconstruct_DRSS(zd_i, zd);

    open_DRSS(zd, zd_f);

    test |= f_neq(zd_f, Zero);


    free(dseed_i);
    free(zd_i);



    return test;
}







/////////////////////////////////////////////
// MALICIOUS PROTOCOLS
/////////////////////////////////////////////

/////////////////////////////////////////////
// Trusted party setup
// Provide common shared strings
// Can also be done via PKI
/////////////////////////////////////////////
void mal_tp_setup(prng_seed seed, RSS_i k_i[CONST_N][LAMBDA], RSS_seed_i rseed_i[CONST_N], DRSS_seed_i dseed_i[CONST_N], ASS_seed_zero_i aseedz_i[CONST_N], RSS_seed_zero_i rseedz_i[CONST_N], DRSS_seed_zero_i dseedz_i[CONST_N]){

    /////////////////////////////////////////////
    // Random RSS key k (LAMBDA many)
    /////////////////////////////////////////////
    RSS (*k) = malloc(sizeof(RSS[LAMBDA]));
    RSS_i (*k_i_temp)[CONST_N] = malloc(sizeof(RSS_i[LAMBDA][CONST_N]));

    RSS_rand_array(k, seed);

    for(int j = 0; j < LAMBDA; j++)
        distribute_RSS(k[j], k_i_temp[j]);
    for(int i = 0; i < CONST_N; i++)
        for(int j = 0; j < LAMBDA; j++)
            memcpy(k_i[i][j], k_i_temp[j][i], sizeof(RSS_i));

    /////////////////////////////////////////////

    free(k);
    free(k_i_temp);

    /////////////////////////////////////////////
    // Seeds for generating random RSS & DRSS shares 
    /////////////////////////////////////////////
    // Use a random seed for all random single-use elements
    prng_seed newseed;
    randombytes(newseed, NBYTES_SEED);

    // RSS
    RSS_seed (*rseed) = malloc(sizeof(RSS_seed));
    // RSS_seed_i (*rseed_i) = malloc(sizeof(RSS_seed_i[CONST_N]));

    random_RSS_seed(*rseed, newseed);
    distribute_RSS_seed(*rseed, rseed_i);
    free(rseed);


    // DRSS
    DRSS_seed (*dseed) = malloc(sizeof(DRSS_seed));
    // DRSS_seed_i (*dseed_i) = malloc(sizeof(DRSS_seed_i[CONST_N]));

    random_DRSS_seed(*dseed, newseed);
    distribute_DRSS_seed(*dseed, dseed_i);
    free(dseed);
    /////////////////////////////////////////////


    /////////////////////////////////////////////
    // Seeds for generating ASS, RSS & DRSS secret shares of zero
    /////////////////////////////////////////////
    // ASS
    ASS_seed_zero (*aseedz) = malloc(sizeof(ASS_seed_zero));
    // ASS_seed_zero_i (*aseedz_i) = malloc(sizeof(ASS_seed_zero_i[CONST_N]));

    random_ASS_seed_zero(*aseedz, newseed);
    distribute_ASS_seed_zero(*aseedz, aseedz_i);
    free(aseedz);


    // RSS
    RSS_seed_zero (*rseedz) = malloc(sizeof(RSS_seed_zero));
    // RSS_seed_zero_i (*rseedz_i) = malloc(sizeof(RSS_seed_zero_i[CONST_N]));

    random_RSS_seed_zero(*rseedz, newseed);
    distribute_RSS_seed_zero(*rseedz, rseedz_i);
    free(rseedz);


    // DRSS
    DRSS_seed_zero (*dseedz) = malloc(sizeof(DRSS_seed_zero));
    // DRSS_seed_zero_i (*dseedz_i) = malloc(sizeof(DRSS_seed_zero_i[CONST_N]));

    random_DRSS_seed_zero(*dseedz, newseed);
    distribute_DRSS_seed_zero(*dseedz, dseedz_i);
    free(dseedz);

    /////////////////////////////////////////////
}




/////////////////////////////////////////////
// Generate secret shares of zero to be used in online part of protocol
/////////////////////////////////////////////
void mal_setup_zeros(const int i, ASS_seed_zero_i aseedz_i, RSS_seed_zero_i rseedz_i, DRSS_seed_zero_i dseedz_i, ASS_i az_i[LAMBDA][TAU_i * TAU_i][3], RSS_i rz_i[LAMBDA], DRSS_i dz_i[LAMBDA]){

    /////////////////////////////////////////////
    // Generate all the ASS shares of zero
    /////////////////////////////////////////////
    f_elm_t (*az_temp)[3 * LAMBDA] = malloc(sizeof(f_elm_t[TAU_i * TAU_i][3 * LAMBDA]));

    ind_T T_ind = 0;
    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        if(i_hold_TT(i, T0, T1)){
            subset_S S; subset_holding_TT(T0, T1, &S);
            ASS_zero_local_array(S, i, 3 * LAMBDA, az_temp[T_ind++], aseedz_i);
        }
    }}

    T_ind = 0;
    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        if(i_hold_TT(i, T0, T1)){
            for(int j = 0; j < LAMBDA; j++)
                memcpy(az_i[j][T_ind], az_temp[T_ind][3*j], sizeof(f_elm_t[3]));
            T_ind++;
        }
    }}
    /////////////////////////////////////////////
    free(az_temp);



    /////////////////////////////////////////////
    // Generate one RSS share of zero
    /////////////////////////////////////////////
    // f_elm_t (*rz_i)[TAU] = malloc(sizeof(f_elm_t[LAMBDA][TAU_i]));
    f_elm_t (*rz_temp)[LAMBDA] = malloc(sizeof(f_elm_t[TAU_i][LAMBDA]));
    RSS_zero_local_array(i, LAMBDA, rz_temp, rseedz_i);
    
    T_ind = 0;
    for(share_T T = T_0; T.ind < TAU; next_T(&T)){
        if(i_hold_T(i, T)){
            for(int j = 0; j < LAMBDA; j++)
                memcpy(rz_i[j][T_ind], rz_temp[T_ind][j], sizeof(f_elm_t));
            T_ind++;
        }
    }
    /////////////////////////////////////////////
    free(rz_temp);


    /////////////////////////////////////////////
    // Generate one RSS share of zero
    /////////////////////////////////////////////
    f_elm_t (*dz_temp)[LAMBDA] = malloc(sizeof(f_elm_t[TAU_i * TAU_i][LAMBDA]));
    DRSS_zero_local_array(i, LAMBDA, dz_temp, dseedz_i);

    T_ind = 0;
    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        if(i_hold_TT(i, T0, T1)){
            for(int j = 0; j < LAMBDA; j++)
                memcpy(dz_i[j][T_ind], dz_temp[T_ind][j], sizeof(f_elm_t));
            T_ind++;
        }
    }}
    /////////////////////////////////////////////
    free(dz_temp);
}



/////////////////////////////////////////////
// First phase - Computing the product
// Parties compute first part of local RSS->RSS mult and send outputs
/////////////////////////////////////////////
void mal_setup_mul_1(const RSS_i rz_i[LAMBDA], RSS_i c_i[LAMBDA][CONST_N][TAU_i * TAU_i], RSS_seed_i rseed_i, DRSS_seed_i dseed_i){

    /////////////////////////////////////////////
    // Compute the input of the multiplication
    // A random RSS element s, and another one s+0
    /////////////////////////////////////////////
    RSS_i (*s)[LAMBDA] = malloc(sizeof(RSS_i[2][LAMBDA]));

    random_RSS_local_array(LAMBDA, s[0], rseed_i);
    for(int l = 0; l < LAMBDA; l++)
        add_RSS_i(s[0][l], rz_i[l], s[1][l]);


    /////////////////////////////////////////////
    // Compute the square
    /////////////////////////////////////////////
    RSS (*c)[TAU_i * TAU_i] = malloc(sizeof(RSS[LAMBDA][TAU_i * TAU_i]));

    for(int l = 0; l < LAMBDA; l++)
        mult_RSS_to_RSS_evaluation(s[0][l], s[1][l], c[l], dseed_i);

    /////////////////////////////////////////////
    free(s);

    /////////////////////////////////////////////
    // Party computes an (many) RSS share
    // Send shares T_j to party j
    // Re_indexing of these outputs
    /////////////////////////////////////////////
    for(int l = 0; l < LAMBDA; l++){
        for(int j = 0; j < CONST_N; j++){
            for(share_T T0 = T_0; T0.ind < TAU_i; next_T(&T0)){
            for(share_T T1 = T_0; T1.ind < TAU_i; next_T(&T1)){
                ind_T T_ind = 0;
                for(share_T T = T_0; T.ind < TAU; next_T(&T)){
                    if(i_hold_T(j, T))
                        f_copy(c[l][T0.ind * TAU_i + T1.ind][T.ind] , c_i[l][j][T0.ind * TAU_i + T1.ind][T_ind++]);
                }
            }}
        }
    }
    /////////////////////////////////////////////
    free(c);
}


/////////////////////////////////////////////
// Re-indexes the outputs so that
// output of party i is sent to party j 
// Not needed in real life execution
// Just in the simulation where we simulate all parties
/////////////////////////////////////////////
void mal_setup_reindex_outputs(RSS_i c[CONST_N][LAMBDA][CONST_N][TAU_i * TAU_i]){
    // f_elm_t c_i[party i that computes output][LAMBDA][party j that receives from i][TAU_i * TAU_i shares of computer][TAU_j * TAU_j shares of receiver]
    RSS_i (*temp) = malloc(sizeof(RSS_i[TAU_i * TAU_i]));

    for(int i = 0; i < CONST_N; i++){
        for(int l = 0; l < LAMBDA; l++){
            for(int j = 0; j < i; j++){
                memcpy(temp,        c[j][l][i], sizeof(RSS_i[TAU_i * TAU_i]));
                memcpy(c[j][l][i],  c[i][l][j], sizeof(RSS_i[TAU_i * TAU_i]));
                memcpy(c[i][l][j],  temp,       sizeof(RSS_i[TAU_i * TAU_i]));
            }
        }
    }
    free(temp);
}

/////////////////////////////////////////////
// Second phase - reconstructing the product
// Parties receive outputs, verify consistency of shares
// and reconstruct the output
/////////////////////////////////////////////
int mal_setup_mul_2(RSS_i c_i[LAMBDA][CONST_N][TAU_i * TAU_i], RSS_i s2_i[LAMBDA]){
    int test = 0;

    for(int l = 0; l < LAMBDA; l++)
        test |= mult_RSS_to_RSS_reconstruction(c_i[l], s2_i[l]);

    /////////////////////////////////////////////

    return test;
}








/////////////////////////////////////////////
// MALICIOUS SETUP BY TRUSTED PARTY
/////////////////////////////////////////////
void mal_setup(const f_elm_t k[LAMBDA], f_elm_t s2[LAMBDA], DRSS_i r_i[CONST_N][LAMBDA], ASS_i az_i[CONST_N][LAMBDA][TAU_i * TAU_i][3], RSS_i k_i[CONST_N][LAMBDA], RSS_i s2_i[CONST_N][LAMBDA], prng_seed seed){

    RSS (*temp) = malloc(sizeof(RSS));
    RSS_i (*temp_k_i)[CONST_N]  = malloc(sizeof(RSS_i[LAMBDA][CONST_N]));
    RSS_i (*temp_s2_i)[CONST_N] = malloc(sizeof(RSS_i[LAMBDA][CONST_N]));
    DRSS (*temp_r) = malloc(sizeof(DRSS[LAMBDA]));


    ASS temp_ta;
    ASS temp_tb;
    ASS temp_tr;


    f_rand_array(s2, LAMBDA, seed); // NOT YET A SQUARE !




    for(int j = 0; j < LAMBDA; j++){
        to_RSS(k[j], temp[0], seed);
        distribute_RSS(temp[0], temp_k_i[j]);

        f_mul(s2[j], s2[j], s2[j]);
        to_RSS(s2[j], temp[0], seed);

        distribute_RSS(temp[0], temp_s2_i[j]);

        DRSS_zero(temp_r[j], seed);
    }


    for(int i = 0; i < CONST_N; i++){
    for(int j = 0; j < LAMBDA; j++){
    for(int T = 0; T < TAU_i; T++){
        f_copy(temp_k_i[j][i][T], k_i[i][j][T]);
        f_copy(temp_s2_i[j][i][T], s2_i[i][j][T]);
    }}}


    free(temp);
    free(temp_k_i);
    free(temp_s2_i);





    for(int j = 0; j < LAMBDA; j++){
    ind_T Ti_inds[CONST_N] = {0};
    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        subset_S S; subset_holding_TT(T0, T1, &S);


        ASS_zero(temp_ta, S, seed);
        ASS_zero(temp_tb, S, seed);
        ASS_zero(temp_tr, S, seed);

        for(int i = 0; i < S.card; i++){
            f_copy(temp_ta[S.com[i]], az_i[S.com[i]][j][Ti_inds[S.com[i]]][0][0]);
            f_copy(temp_tb[S.com[i]], az_i[S.com[i]][j][Ti_inds[S.com[i]]][1][0]);
            f_copy(temp_tr[S.com[i]], az_i[S.com[i]][j][Ti_inds[S.com[i]]][2][0]);
            f_copy(temp_r[j][T0.ind * TAU + T1.ind], r_i[S.com[i]][j][Ti_inds[S.com[i]]]);
            Ti_inds[S.com[i]]++;
        }
    }}}

    free(temp_r);
}




/////////////////////////////////////////////
// MALICIOUS EVALUATION
/////////////////////////////////////////////
void mal_evaluation(const RSS_i x_i, const RSS_i k_i[LAMBDA], const RSS_i s2_i[LAMBDA], const DRSS_i r_i[LAMBDA], const ASS_i az_i[LAMBDA][TAU_i * TAU_i][3], DRSS_i o_i[LAMBDA], DRSS_digest_i h_i){
    RSS_i a_i;
    f_elm_t t0, t1;

    f_elm_t (*temp)[TAU_i * TAU_i][LAMBDA] = malloc(sizeof(f_elm_t[TAU_i * TAU_i][LAMBDA]));


    for(int j = 0; j < LAMBDA; j++){
        add_RSS_i(x_i, k_i[j], a_i);

        for(share_T T0 = T_0; T0.ind < TAU_i; next_T(&T0)){
        for(share_T T1 = T_0; T1.ind < TAU_i; next_T(&T1)){

            f_mul(a_i[T0.ind], s2_i[j][T1.ind], t0);                            // t0 = a_{T_0} * b_{T_1}
            f_add(t0, r_i[j][T0.ind * TAU_i + T1.ind], t0);                     // t0 = a_{T_0} * b_{T_1} + r_{T_0T_1} = o_{T_1T_2}

            // hash(t0, h_i[j][T0.ind * TAU_i + T1.ind]);                          // h_i = Hash(t0) = Hash(o_{T_1T_2})
            f_copy(t0, temp[0][T0.ind * TAU_i + T1.ind][j]);

            f_mul(a_i[T0.ind], az_i[j][T0.ind * TAU_i + T1.ind][0][0], t1);        // t1 = a_{T_0} * t^{a, T_0T_1}
            f_add(t0, t1, t0);                                                  // t0 = a_{T_0} * b_{T_1} + r_{T_0T_1} + a_{T_0} * t^{a, T_0T_1}_i

            f_mul(s2_i[j][T1.ind], az_i[j][T0.ind * TAU_i + T1.ind][1][0], t1);    // t1 = b_{T_1} * t^{b, T_0T_1}_i
            f_add(t0, t1, t0);                                                  // t0 = a_{T_0} * b_{T_1} + r_{T_0T_1} + a_{T_0} * t^{a, T_0T_1}_i + b_{T_1} * t^{b, T_0T_1}_i

            f_add(t0, az_i[j][T0.ind * TAU_i + T1.ind][2][0], t0);                 // t0 = a_{T_0} * b_{T_1} + r_{T_0T_1} + a_{T_0} * t^{a, T_0T_1}_i + b_{T_1} * t^{b, T_0T_1}_i + t^{r, T_0T_1}_i

            f_copy(t0, o_i[j][T0.ind * TAU_i + T1.ind]);
        }}
    }

    for(share_T T0 = T_0; T0.ind < TAU_i; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU_i; next_T(&T1)){
        hash_array(temp[0][T0.ind * TAU_i + T1.ind], LAMBDA, h_i[T0.ind * TAU_i + T1.ind]);    // h_i = Hash( o^0_{T_0T_1}, o^1_{T_0T_1}, ..., o^{LAMBDA - 1}_{T_0T_1})
    }}

    free(temp);
}

/////////////////////////////////////////////
// MALICIOUS RECONSTRUCTION PROTOCOL
/////////////////////////////////////////////
int mal_reconstruction(const DRSS_i o_i[CONST_N][LAMBDA], const DRSS_digest_i h_i[CONST_N], f_elm_t o[LAMBDA]){

    hash_digest t_hd;
    f_elm_t (*temp)[TAU * TAU][LAMBDA] = malloc(sizeof(f_elm_t[CONST_N][TAU * TAU][LAMBDA]));
    int ret = 0;    //Return 1 if detected anomalies (i.e. hashes incorrect)


    for(int j = 0; j < LAMBDA; j++){
        ind_T Ti_inds_0[CONST_N] = {0};
        f_elm_t v = {0};                // v =?= a_{T_0} * b_{T_1} + r_{T_0T_1} = o_{T_0T_1}

        for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
        for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
            f_elm_t v_TT = {0};

            for(int i = 0; i < CONST_N; i++){
                if(i_hold_TT(i, T0, T1)){
                    f_add(v_TT, o_i[i][j][Ti_inds_0[i]++], v_TT);   
                }            
            }
            f_mul(v_TT, f_inverses[S_TT(T0, T1)], v_TT);
            f_add(v, v_TT, v);

            f_copy(v_TT, temp[0][T0.ind * TAU + T1.ind][j]);
        }}
        f_copy(v, o[j]);
    }

    ind_T Ti_inds_0[CONST_N] = {0};

    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){

        hash_array(temp[0][T0.ind * TAU + T1.ind], LAMBDA, t_hd);        // h =?= o_{T_0T_1}
        for(int i = 0; i < CONST_N; i++){
            if(i_hold_TT(i, T0, T1)){
                if(memcmp(h_i[i][Ti_inds_0[i]++], t_hd, NBYTES_DIGEST) != 0)
                    ret = 1;
            }            
        }
    }}

    free(temp);

    return ret;
}


/////////////////////////////////////////////
// MALICIOUS PROTOCOL WITHOUT GENERATION OF ZEROS
/////////////////////////////////////////////
unsigned char mal_protocol(prng_seed seed){
    int test = 0;

    f_elm_t (*k) = malloc(sizeof(f_elm_t[LAMBDA]));
    RSS_i (*k_i)[LAMBDA] = malloc(sizeof(RSS_i[CONST_N][LAMBDA]));

    f_elm_t (*s2) = malloc(sizeof(f_elm_t[LAMBDA]));
    RSS_i (*s2_i)[LAMBDA] = malloc(sizeof(RSS_i[CONST_N][LAMBDA]));
    
    ASS_i (*az_i)[LAMBDA][TAU_i * TAU_i][3] = malloc(sizeof(ASS_i[CONST_N][LAMBDA][TAU_i * TAU_i][3]));
    DRSS_i (*dz_i)[LAMBDA] = malloc(sizeof(DRSS_i[CONST_N][LAMBDA]));


    prng_seed curr_seed;
    memcpy(curr_seed, seed, NBYTES_SEED);

    ////////////////////////////////////////////
    // LONG TERM SECRET KEY
    ////////////////////////////////////////////
    f_rand_array(k, LAMBDA, seed);

    ////////////////////////////////////////////


    ////////////////////////////////////////////
    // SETUP PHASE BY TRUSTED PARTY
    ////////////////////////////////////////////
    mal_setup(k, s2, dz_i, az_i, k_i, s2_i, seed);

    ////////////////////////////////////////////



    ////////////////////////////////////////////
    // CLIENT INPUT STAGE
    ////////////////////////////////////////////
    f_elm_t x;
    RSS_i (*x_i) = malloc(sizeof(RSS_i[CONST_N]));

    f_rand(x, seed);
    input(x, x_i, seed);

    ////////////////////////////////////////////


    ////////////////////////////////////////////
    // SERVERS EVALUATION STAGE
    ////////////////////////////////////////////
    DRSS_i (*o_i)[LAMBDA] = malloc(sizeof(DRSS_i[CONST_N][LAMBDA]));
    DRSS_digest_i (*h_i) = malloc(sizeof(DRSS_digest_i[CONST_N]));

    for(int i = 0; i < CONST_N; i++)
        mal_evaluation(x_i[i], k_i[i], s2_i[i], dz_i[i], az_i[i], o_i[i], h_i[i]);

    ////////////////////////////////////////////
    free(x_i);
    free(k_i);
    free(s2_i);
    free(dz_i);
    free(az_i);


    ////////////////////////////////////////////
    // CLIENT RECONSTRUCTION STAGE
    ////////////////////////////////////////////
    f_elm_t (*o) = malloc(sizeof(f_elm_t[LAMBDA]));
    unsigned char L[PRF_BYTES] = {0};

    ////////////////////////////////////////////
    // Client reconstructs received values
    // Returns 1 if servers acted dishonestly, otherwise 0
    
    test |= mal_reconstruction(o_i, h_i, o);

    ////////////////////////////////////////////
    // Client computes Legendre symbols
   
    calc_symbols(o, L);

    printf("\n");
    printf("Output: "); print_hex(L, PRF_BYTES);

    ////////////////////////////////////////////
    free(o_i);
    free(h_i);


    ////////////////////////////////////////////
    // CORRECTNESS CHECK
    ////////////////////////////////////////////
    f_elm_t z;  

    for(int j = 0; j < LAMBDA; j++){
        f_add(x, k[j], z);
        f_mul(z, s2[j], z);        
        test |= f_neq(z, o[j]);
    }

    free(k);
    free(s2);
    free(o);


    return(test);
}


/////////////////////////////////////////////
// MALICIOUS FULL PROTOCOL
/////////////////////////////////////////////
int mal_protocol_full(prng_seed seed){
    int test = 0;

    prng_seed curr_seed;
    memcpy(curr_seed, seed, NBYTES_SEED);

    ////////////////////////////////////////////
    // Generate key
    // Same key as inside mal_tp_setup
    ////////////////////////////////////////////
    RSS (*rk_temp) = malloc(sizeof(RSS[LAMBDA]));
    f_elm_t (*k) = malloc(sizeof(f_elm_t[LAMBDA]));
    
    RSS_rand_array(rk_temp, curr_seed);
    for(int j = 0; j < LAMBDA; j++)
        open_RSS(rk_temp[j], k[j]);

    memcpy(curr_seed, seed, NBYTES_SEED);

    ////////////////////////////////////////////

    free(rk_temp);

    ////////////////////////////////////////////
    // TRUSTED PARTY SEED AND KEY DISTRIBUTION
    ////////////////////////////////////////////

    RSS_i (*k_i)[LAMBDA] = malloc(sizeof(RSS_i[CONST_N][LAMBDA]));
    RSS_seed_i (*rseed_i) = malloc(sizeof(RSS_seed_i[CONST_N]));
    DRSS_seed_i (*dseed_i) = malloc(sizeof(DRSS_seed_i[CONST_N]));
    ASS_seed_zero_i (*aseedz_i) = malloc(sizeof(ASS_seed_zero_i[CONST_N]));
    RSS_seed_zero_i (*rseedz_i) = malloc(sizeof(RSS_seed_zero_i[CONST_N]));
    DRSS_seed_zero_i (*dseedz_i) = malloc(sizeof(DRSS_seed_zero_i[CONST_N]));

    mal_tp_setup(seed, k_i, rseed_i, dseed_i, aseedz_i, rseedz_i, dseedz_i);

    ////////////////////////////////////////////


    ////////////////////////////////////////////
    // LOCAL SETUP STAGE
    ////////////////////////////////////////////
    
    ASS_i (*az_i)[LAMBDA][TAU_i * TAU_i][3]  = malloc(sizeof(ASS_i[CONST_N][LAMBDA][TAU_i * TAU_i][3]));
    RSS_i (*rz_i)[LAMBDA]  = malloc(sizeof(RSS_i[CONST_N][LAMBDA]));
    DRSS_i (*dz_i)[LAMBDA] = malloc(sizeof(DRSS_i[CONST_N][LAMBDA]));

    ////////////////////////////////////////////
    // Compute all secret shares of zero to be used later
    
    for(int i = 0; i < CONST_N; i++)
        mal_setup_zeros(i, aseedz_i[i], rseedz_i[i], dseedz_i[i], az_i[i], rz_i[i], dz_i[i]);
    

    ////////////////////////////////////////////
    // First step of RSS multiplication
    
    RSS_i (*c_i)[LAMBDA][CONST_N][TAU_i * TAU_i] = malloc(sizeof(RSS_i[CONST_N][LAMBDA][CONST_N][TAU_i * TAU_i]));
    
    for(int i = 0; i < CONST_N; i++)
        mal_setup_mul_1(rz_i[i], c_i[i], rseed_i[i], dseed_i[i]);


    ////////////////////////////////////////////
    // Re-indexing, i.e., sending messages
    
    mal_setup_reindex_outputs(c_i);



    ////////////////////////////////////////////
    // Second step of RSS multiplication
    
    RSS_i (*s2_i)[LAMBDA] = malloc(sizeof(RSS_i[CONST_N][LAMBDA]));
    
    for(int i = 0; i < CONST_N; i++)
        test |= mal_setup_mul_2(c_i[i], s2_i[i]);

    ////////////////////////////////////////////
    free(rz_i);
    free(c_i);
    free(rseed_i);
    free(dseed_i);
    free(aseedz_i);
    free(rseedz_i);
    free(dseedz_i);


    ////////////////////////////////////////////
    // CLIENT INPUT STAGE
    ////////////////////////////////////////////
    f_elm_t x;
    RSS_i (*x_i) = malloc(sizeof(RSS_i[CONST_N]));

    f_rand(x, seed);
    input(x, x_i, seed);

    ////////////////////////////////////////////


    ////////////////////////////////////////////
    // SERVERS EVALUATION STAGE
    ////////////////////////////////////////////
    DRSS_i (*o_i)[LAMBDA] = malloc(sizeof(DRSS_i[CONST_N][LAMBDA]));
    DRSS_digest_i (*h_i) = malloc(sizeof(DRSS_digest_i[CONST_N]));
    
    for(int i = 0; i < CONST_N; i++)
        mal_evaluation(x_i[i], k_i[i], s2_i[i], dz_i[i], az_i[i], o_i[i], h_i[i]);

    ////////////////////////////////////////////
    free(x_i);
    free(k_i);
    free(dz_i);
    free(az_i);


    ////////////////////////////////////////////
    // Reconstruct squares, for testing purposes

    f_elm_t (*s2) = malloc(sizeof(f_elm_t[LAMBDA]));
    RSS (*s2_rss) = malloc(sizeof(RSS[LAMBDA]));
    RSS_i (*s2_i_temp)[CONST_N] = malloc(sizeof(RSS_i[LAMBDA][CONST_N]));
    for(int i = 0; i < CONST_N; i++){
        for(int j = 0; j < LAMBDA; j++)
            memcpy(s2_i_temp[j][i], s2_i[i][j], sizeof(RSS_i));
    }

    for(int j = 0; j < LAMBDA; j++){
        test |= reconstruct_RSS(s2_i_temp[j], s2_rss[j]);
        open_RSS(s2_rss[j], s2[j]);
    }

    ////////////////////////////////////////////
    free(s2_rss);
    free(s2_i);
    free(s2_i_temp);


    ////////////////////////////////////////////
    // CLIENT RECONSTRUCTION STAGE
    ////////////////////////////////////////////
    f_elm_t (*o) = malloc(sizeof(f_elm_t[LAMBDA]));
    unsigned char (*L) = malloc(PRF_BYTES);

    ////////////////////////////////////////////
    // Client reconstructs received values
    // Returns 1 if servers acted dishonestly, otherwise 0
    
    test |= mal_reconstruction(o_i, h_i, o);

    ////////////////////////////////////////////
    // Client computes Legendre symbols
    
    calc_symbols(o, L);

    printf("\n");
    printf("Output: "); print_hex(L, PRF_BYTES);

    ////////////////////////////////////////////
    free(o_i);
    free(h_i);
    free(L);



    ////////////////////////////////////////////
    // CORRECTNESS CHECK
    ////////////////////////////////////////////
    f_elm_t z;  

    for(int j = 0; j < LAMBDA; j++){
        f_add(x, k[j], z);
        f_mul(z, s2[j], z);        
        test |= f_neq(z, o[j]);
    }

    free(k);
    free(s2);
    free(o);


    return(test);
}


// uint64_t bench(int function_selector, prng_seed seed){
//     uint64_t nsecs_pre = 0, nsecs_post = 0, nsecs = 0;
//     switch (function_selector) {
//         case(0): {
//             RSS_i (*k_i)[LAMBDA] = malloc(sizeof(RSS_i[CONST_N][LAMBDA]));
//             RSS_seed_i (*rseed_i) = malloc(sizeof(RSS_seed_i[CONST_N]));
//             ASS_seed_zero_i (*aseedz_i) = malloc(sizeof(ASS_seed_zero_i[CONST_N]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 sh_tp_setup(seed, k_i, rseed_i, aseedz_i);
//             nsecs_post = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             nsecs = (nsecs_post-nsecs_pre);

//             free(k_i);
//             free(rseed_i);
//             free(aseedz_i);

//             return nsecs;
//         }
//         case(1): {
//             ASS_seed_zero_i (*aseedz_i) = malloc(sizeof(ASS_seed_zero_i));
//             ASS_i (*az_i)[LAMBDA] = malloc(sizeof(ASS_i[2][LAMBDA]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 sh_setup_zeros(i % CONST_N, az_i, *aseedz_i);
//             nsecs_post = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             nsecs = (nsecs_post-nsecs_pre);

//             free(az_i);
//             free(aseedz_i);

//             return nsecs;
//         }
//         case(2): {

//             ASS_i (*az_i) = malloc(sizeof(ASS_i[LAMBDA]));
//             RSS_i (*c_i)[CONST_N] = malloc(sizeof(RSS_i[LAMBDA][CONST_N]));
//             RSS_seed_i (*rseed_i) = malloc(sizeof(RSS_seed_i));


//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 sh_setup_mul_1(az_i, c_i, *rseed_i);
//             nsecs_post = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             nsecs = (nsecs_post-nsecs_pre);

//             free(az_i);
//             free(c_i);
//             free(rseed_i);

//             return nsecs;
//         }
//         case(3): {
//             RSS_i (*c_i)[CONST_N] = malloc(sizeof(RSS_i[LAMBDA][CONST_N]));
//             RSS_i (*s2_i) = malloc(sizeof(RSS_i[LAMBDA]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 sh_setup_mul_2(c_i, s2_i);
//             nsecs_post = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             nsecs = (nsecs_post-nsecs_pre);

//             free(c_i);
//             free(s2_i);

//             return nsecs;
//         }
//         case(4): {
//             f_elm_t x;
//             RSS_i (*x_i) = malloc(sizeof(RSS_i[CONST_N]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 input(x, x_i, seed);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
            
//             nsecs = (nsecs_post-nsecs_pre);

//             free(x_i);
//             return nsecs;
//         }
//         case(5): {
//             RSS_i (*x_i) = malloc(sizeof(RSS_i));
//             RSS_i (*k_i) = malloc(sizeof(RSS_i[LAMBDA]));
//             RSS_i (*s2_i) = malloc(sizeof(RSS_i[LAMBDA]));
//             ASS_i (*az_i) = malloc(sizeof(ASS_i[LAMBDA]));
//             ASS_i (*o_i) = malloc(sizeof(ASS_i[LAMBDA]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 sh_evaluation(*x_i, k_i, s2_i, az_i, o_i);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
            
//             nsecs = (nsecs_post-nsecs_pre);

//             free(x_i);
//             free(k_i);
//             free(s2_i);
//             free(az_i);
//             free(o_i);

//             return nsecs;
//         }
//         case(6): {  // sh_evaluation
//             RSS_i (*x_i) = malloc(sizeof(RSS_i));
//             RSS_i (*k_i) = malloc(sizeof(RSS_i[LAMBDA]));
//             RSS_i (*s2_i) = malloc(sizeof(RSS_i[LAMBDA]));
//             ASS_i (*az_i) = malloc(sizeof(ASS_i[LAMBDA]));
//             ASS_i (*o_i) = malloc(sizeof(ASS_i[LAMBDA]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 sh_evaluation(*x_i, k_i, s2_i, az_i, o_i);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
            
//             nsecs = (nsecs_post-nsecs_pre);

//             free(x_i);
//             free(k_i);
//             free(s2_i);
//             free(az_i);
//             free(o_i);
            
//             return nsecs;
//         }
//         case(7):{ // sh_reconstruction

//             ASS_i (*o_i)[LAMBDA] = malloc(sizeof(ASS_i[CONST_N][LAMBDA]));
//             f_elm_t (*o) = malloc(sizeof(f_elm_t[LAMBDA]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 sh_reconstruction(o_i, o);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
//             nsecs = (nsecs_post - nsecs_pre);

//             free(o);
//             free(o_i);

//             return nsecs;
//         }
//         case(8): { // calc_symbols
//             f_elm_t o[LAMBDA];
//             unsigned char L[PRF_BYTES];

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 calc_symbols(o, L);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
//             nsecs = (nsecs_post - nsecs_pre);
//             return nsecs;
//         }
//         case(9): {  // mal_tp_setup
            
//             RSS_i (*k_i)[LAMBDA] = malloc(sizeof(RSS_i[CONST_N][LAMBDA]));
//             RSS_seed_i (*rseed_i) = malloc(sizeof(RSS_seed_i[CONST_N]));
//             DRSS_seed_i (*dseed_i) = malloc(sizeof(DRSS_seed_i[CONST_N]));
//             ASS_seed_zero_i (*aseedz_i) = malloc(sizeof(ASS_seed_zero_i[CONST_N]));
//             RSS_seed_zero_i (*rseedz_i) = malloc(sizeof(RSS_seed_zero_i[CONST_N]));
//             DRSS_seed_zero_i (*dseedz_i) = malloc(sizeof(DRSS_seed_zero_i[CONST_N]));


//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 mal_tp_setup(seed, k_i, rseed_i, dseed_i, aseedz_i, rseedz_i, dseedz_i);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds

//             nsecs = (nsecs_post-nsecs_pre);

//             free(k_i);
//             free(rseed_i);
//             free(dseed_i);
//             free(aseedz_i);
//             free(rseedz_i);
//             free(dseedz_i);

//             return nsecs;
//         }
//         case(10): {  // mal_setup_zeros

//             ASS_seed_zero_i (*aseedz_i)  = malloc(sizeof(ASS_seed_zero_i));
//             RSS_seed_zero_i (*rseedz_i)  = malloc(sizeof(RSS_seed_zero_i));
//             DRSS_seed_zero_i (*dseedz_i) = malloc(sizeof(DRSS_seed_zero_i));
            
//             ASS_i (*az_i)[TAU_i * TAU_i][3]  = malloc(sizeof(ASS_i[LAMBDA][TAU_i * TAU_i][3]));
//             RSS_i (*rz_i)  = malloc(sizeof(RSS_i[LAMBDA]));
//             DRSS_i (*dz_i) = malloc(sizeof(DRSS_i[LAMBDA]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 mal_setup_zeros(i % CONST_N, *aseedz_i, *rseedz_i, *dseedz_i, az_i, rz_i, dz_i);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
//             nsecs = (nsecs_post-nsecs_pre);

//             free(az_i);
//             free(rz_i);
//             free(dz_i);
//             free(aseedz_i);
//             free(rseedz_i);
//             free(dseedz_i);

//             return nsecs;
//         }
//         case(11): {  // mal_setup_mul_1
            
//             RSS_i (*rz_i) = malloc(sizeof(RSS_i[LAMBDA]));
//             RSS_i (*c_i)[CONST_N][TAU_i * TAU_i] = malloc(sizeof(RSS_i[LAMBDA][CONST_N][TAU_i * TAU_i]));
//             RSS_seed_i (*rseed_i) = malloc(sizeof(RSS_seed_i));
//             DRSS_seed_i (*dseed_i) = malloc(sizeof(DRSS_seed_i));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 mal_setup_mul_1(rz_i, c_i, *rseed_i, *dseed_i);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds

//             nsecs = (nsecs_post-nsecs_pre);

//             free(rz_i);
//             free(c_i);
//             free(rseed_i);
//             free(dseed_i);

//             return nsecs;
//         }
//         case(12): {  // mal_setup_mul_1

//             RSS_i (*c_i)[CONST_N][TAU_i * TAU_i] = malloc(sizeof(RSS_i[LAMBDA][CONST_N][TAU_i * TAU_i]));
//             RSS_i (*s2_i) = malloc(sizeof(RSS_i[LAMBDA]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 mal_setup_mul_2(c_i, s2_i);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds

//             nsecs = (nsecs_post-nsecs_pre);

//             free(c_i);
//             free(s2_i);

//             return nsecs;
//         }
//         case(13): {  // input
            
//             f_elm_t x;
//             RSS_i (*x_i) = malloc(sizeof(RSS_i[CONST_N]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 input(x, x_i, seed);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
            
//             nsecs = (nsecs_post-nsecs_pre);

//             free(x_i);
//             return nsecs;
//         }
//         case(14): {  // mal_evaluation
            
//             RSS_i (*x_i) = malloc(sizeof(RSS_i[1]));
//             RSS_i (*k_i) = malloc(sizeof(RSS_i[LAMBDA]));
//             RSS_i (*s2_i) = malloc(sizeof(RSS_i[LAMBDA]));

//             ASS_i (*az_i)[TAU_i * TAU_i][3] = malloc(sizeof(ASS_i[LAMBDA][TAU_i * TAU_i][3]));
//             DRSS_i (*dz_i) = malloc(sizeof(DRSS_i[LAMBDA]));

//             DRSS_i (*o_i) = malloc(sizeof(DRSS_i[LAMBDA]));
//             DRSS_digest_i (*h_i) = malloc(sizeof(DRSS_digest_i));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 mal_evaluation(*x_i, k_i, s2_i, dz_i, az_i, o_i, *h_i);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
            
//             nsecs = (nsecs_post-nsecs_pre);

//             free(x_i);
//             free(k_i);
//             free(s2_i);
//             free(az_i);
//             free(dz_i);
//             free(o_i);
//             free(h_i);

//             return nsecs;
//         }
//         case(15): { // mal_reconstruction
            
//             DRSS_i (*o_i)[LAMBDA] = malloc(sizeof(DRSS_i[CONST_N][LAMBDA]));
//             DRSS_digest_i (*h_i) = malloc(sizeof(DRSS_digest_i[CONST_N]));
//             f_elm_t (*o) = malloc(sizeof(f_elm_t[LAMBDA]));

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 mal_reconstruction(o_i, h_i, o);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds
            
//             nsecs = (nsecs_post-nsecs_pre);

//             free(o_i);
//             free(h_i);
//             free(o);

//             return nsecs;
//         }
//         case(16): { // calc_symbols
//             f_elm_t o[LAMBDA];
//             unsigned char L[PRF_BYTES];

//             nsecs_pre = cpucycles();    //cpucycles actually doesn't count cycles but counts nanoseconds
//             for(int i = 0; i < LOOPS; i++)
//                 calc_symbols(o, L);
//             nsecs_post = cpucycles();   //cpucycles actually doesn't count cycles but counts nanoseconds

//             nsecs = (nsecs_post-nsecs_pre);

//             return nsecs;
//         }
//         default: {
//             printf("Invalid function_selector. Please choose    \
//                     0 for sh_tp_setup,                          \
//                     1 for sh_input,                             \
//                     2 for sh_evaluation,                        \
//                     3 for sh_reconstruction,                    \
//                     4 for calc_symbols,                         \
//                     5 for mal_tp_setup,                         \
//                     6 for mal_setup_zeros,                      \
//                     7 for mal_setup_mul_1,                      \
//                     8 for mal_setup_mul_2,                      \
//                     9 for mal_input,                            \
//                     10 for mal_evaluation,                      \
//                     11 for mal_reconstruction,                  \
//                     12 for calc_symbols.\n");
//             return 0;
//         }
//         }
// }


int main(int argc, char* argv[]){
    // Remove compilation warnings
    (void)argv[0];
    (void)argc;

    prng_seed seed = {0};
    uint64_t nsecs;
    int test = 1;

    init_inverses();
    generate_table();
    const char *function_names[] = {\
    "sh_tp_setup runs in .....................", \
    "sh_setup_zeros runs in ..................", \
    "sh_setup_mul_1 runs in ..................", \
    "sh_setup_mul_2 runs in ..................", \
    "sh_input runs in ........................", \
    "sh_evaluation runs in ...................", \
    "sh_reconstruction runs in ...............", \
    "calc_symbols runs in ....................", \
    "mal_tp_setup runs in ....................", \
    "mal_setup_zeros runs in .................", \
    "mal_setup_mul_1 runs in .................", \
    "mal_setup_mul_2 runs in .................", \
    "mal_input runs in .......................", \
    "mal_evaluation runs in ..................", \
    "mal_reconstruction runs in ..............", \
    "calc_symbols runs in ...................."};




    #if (ADVERSARY==SEMIHONEST)

        test = sh_protocol_full(seed);

        printf("%3d bits, Semi-honest; Correctness check: %s             \n\n", NBITS_FIELD, PASS(test));

    #elif(ADVERSARY==MALICIOUS)

        #if (CONST_T >= 3)
            test = mal_protocol(seed);
        #elif (CONST_T <= 2)
            test = mal_protocol_full(seed); // test
        #endif

        printf("%3d bits, Malicious; Correctness check: %s \n\n", NBITS_FIELD, PASS(test));

    #endif

    // for(int function_selector = START_FUNC; function_selector < END_FUNC; function_selector++){
    //     nsecs = bench(function_selector, seed);
    //     printf("%s %14s ns\n", function_names[function_selector], print_num((double)((uint64_t)(nsecs/(LOOPS)))));
    // }

    printf("...........................................................\n\n");



    return 0;
}












