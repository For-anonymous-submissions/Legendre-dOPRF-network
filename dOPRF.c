#include "dOPRF.h"
#include "bench.h"
#include "arith.h"



// ONE-TIME-COMPUTATION

// Precomputed constants, filled in at main call
f_elm_t f_inverses[CONST_N + 1] = {0};

// Table for holding binomial values, precomputed at main call
uint32_t binom_table[CONST_N + 1][CONST_N + 1] = {0};



// Generate table with binomial values
void generate_table(){
    for(int n = 0; n < CONST_N + 1; n++)
        for(int t = 0; t <= n; t++)
            binom_table[n][t] = BINOMIAL_TABLE(n,t);
}

// Function to compute binomials (just read from table)
uint32_t binomial(unsigned int n, unsigned int t){
    // return BINOMIAL_TABLE(n, t);
    return binom_table[n][t];
}

// Initialise inverse elements 1/1, 1/2, ..., 1/CONST_N, once at main call
void init_inverses() {
    f_elm_t t0 = {0};

    for(int i = 0; i < CONST_N + 1; i++){
        f_inv(t0, f_inverses[i]);
        f_add(t0, Mont_one, t0);
    }
}




/////////////////////////////////////////////
// Printing shares
/////////////////////////////////////////////

// Print share T as indice, i.e. integer
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

// Print share T as combination, i.e., set
void print_com_T(const com_T T_com){
    printf("{");
    for(int i = CONST_T - 1; i > 0; i--)
        printf("%01d, ", T_com[i]);
    printf("%01d", T_com[0]);
    printf("}");
}

// Print share T as binary string with CONST_T 1s and CONST_N - CONST_T 0s
void print_bin_T(const bin_T T_bin){
    for(int i = CONST_N - 1; i >= 0; i--)
        printf("%c", BIT_TO_STR((T_bin) & (1 << i)));
}

// Print share T in all formats
void print_T(const share_T T){
    print_ind_T(T.ind);printf("   ");
    print_com_T(T.com);printf("   ");
    print_bin_T(T.bin);printf("   ");
}


/////////////////////////////////////////////
// Share conversion between different representations
/////////////////////////////////////////////

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
        if(T_bin_t & 0x01)
            (*T_com)[j++] = i;

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
void ind_to_bin_T(const ind_T* T_ind, bin_T* T_bin){
    com_T T_com[1] = {0};
    ind_to_com_T(T_ind, T_com);
    com_to_bin_T(T_com, T_bin);
}


// Update other representations of share
void update_T(share_T* T, char type){
    switch(type) {
        case 'i':
            ind_to_com_T(&(T->ind), &(T->com));
            com_to_bin_T(&(T->com), &(T->bin));
            break;
        case 'c':
            com_to_ind_T(&(T->com), &(T->ind));
            com_to_bin_T(&(T->com), &(T->bin));
            break;
        case 'b':
            bin_to_com_T(&(T->bin), &(T->com));
            com_to_ind_T(&(T->com), &(T->ind));
            break;
        default:
            printf("Invalid share representation selector. Please choose 'i' for ind, 'c' for com or 'b' for bin.\n");
    }
}

// Choose next share according to order (induced by T.ind)
void next_T(share_T* T){
    (T->ind)++;

    if((T->ind) != TAU)
        update_T(T, 'i');
}

// Number of servers that hold both T0 and T1
int S_TT(share_T T0, share_T T1){
    return CONST_N - HWT_32BIT(T0.bin | T1.bin);
}


/////////////////////////////////////////////
//  Subsets S
/////////////////////////////////////////////

// Update other representations of subset
void update_S(subset_S* S, char type){
    switch(type) {
        case 'c':
            com_to_bin_S(&(S->com), &(S->card), &(S->bin));
            break;
        case 'b':
            bin_to_com_S(&(S->bin), &(S->com), &(S->card));
            break;
        default:
            printf("Invalid subset representation selector. Please choose 'c' for com or 'b' for bin.\n");
    }
}

// Convert share from binary format to index format
void bin_to_com_S(const bin_S* S_bin, com_S* S_com, card_S* S_card){
    bin_S S_bin_t = *S_bin;
    card_S S_card_t = 0;

    for(int i = 0; i < CONST_N; i++){
        if(S_bin_t & 0x01)
            (*S_com)[S_card_t++] = i;
        S_bin_t >>= 1;
    }
    *S_card = S_card_t;
}

// Convert share from index format to binary format
void com_to_bin_S(const com_S* S_com, const card_S* S_card, bin_S* S_bin){
    *S_bin = 0;
    for(int i = 0; i < *S_card; i++)
        *S_bin |= (0x01 << (*S_com)[i]);
}

// Define set of parties S holding a share T
void subset_holding_T(const share_T T, subset_S* S){
    S->bin = (~T.bin) & ((1 << CONST_N) - 1);
    bin_to_com_S(&(S->bin), &(S->com), &(S->card));
}

// Define set of parties holding S holding two shares T0, T1
void subset_holding_TT(const share_T T0, const share_T T1, subset_S* S){
    S->bin = (~(T0.bin | T1.bin)) & ((1 << CONST_N) - 1);
    bin_to_com_S(&(S->bin), &(S->com), &(S->card));
}




/////////////////////////////////////////////
// Basic arithmetic of secret shares
/////////////////////////////////////////////

// Add two RSS shares
void add_RSS(const RSS a, const RSS b, RSS c){
    for(ind_T T_ind = 0; T_ind < TAU; T_ind ++)
        f_add(a[T_ind], b[T_ind], c[T_ind]);
}

// Add two RSS shares locally
void add_RSS_i(const RSS_i a, const RSS_i b, RSS_i c){
    for(ind_T T_ind = 0; T_ind < TAU_i; T_ind ++)
        f_add(a[T_ind], b[T_ind], c[T_ind]);
}

// Subtract two RSS shares
void sub_RSS(const RSS a, const RSS b, RSS c){
    for(ind_T T_ind = 0; T_ind < TAU; T_ind ++)
        f_sub(a[T_ind], b[T_ind], c[T_ind]);
}

// Subtract two RSS shares locally
void sub_RSS_i(const RSS_i a, const RSS_i b, RSS_i c){
    for(ind_T T_ind = 0; T_ind < TAU_i; T_ind ++)
        f_sub(a[T_ind], b[T_ind], c[T_ind]);
}

// Add two DRSS shares
void add_DRSS(const DRSS a, const DRSS b, DRSS c){
    for(ind_T T_ind = 0; T_ind < TAU*TAU; T_ind ++)
        f_add(a[T_ind], b[T_ind], c[T_ind]);
}

// Add two DRSS shares locally
void add_DRSS_i(const DRSS_i a, const DRSS_i b, DRSS_i c){
    for(ind_T T_ind = 0; T_ind < TAU_i*TAU_i; T_ind ++)
        f_add(a[T_ind], b[T_ind], c[T_ind]);
}

// Subtract two DRSS shares
void sub_DRSS(const DRSS a, const DRSS b, DRSS c){
    for(ind_T T_ind = 0; T_ind < TAU*TAU; T_ind ++)
        f_sub(a[T_ind], b[T_ind], c[T_ind]);
}

// Subtract two DRSS shares locally
void sub_DRSS_i(const DRSS_i a, const DRSS_i b, DRSS_i c){
    for(ind_T T_ind = 0; T_ind < TAU_i*TAU_i; T_ind ++)
        f_sub(a[T_ind], b[T_ind], c[T_ind]);
}






/////////////////////////////////////////////
// Random secret sharing of a field element
/////////////////////////////////////////////

// Create a random ASS secret sharing of a
void to_ASS_urandom(const f_elm_t a, ASS A, const subset_S S){
    f_elm_t t0 = {0};

    f_rand_array(A, CONST_N);
    for(ind_T T_ind = 0; T_ind < S.card - 1; T_ind++)
        f_add(t0, A[S.com[T_ind]], t0);

    f_sub(a, t0, A[S.com[S.card - 1]]);
}

void to_ASS_prng(const f_elm_t a, ASS A, const subset_S S, prng_seed seed){
    f_elm_t t0 = {0};

    f_rand_array(A, CONST_N, seed);
    for(int i = 0; i < S.card - 1; i++)
        f_add(t0, A[S.com[i]], t0);

    f_sub(a, t0, A[S.com[S.card - 1]]);
}


// Create a random RSS secret sharing of a
void to_RSS_urandom(const f_elm_t a, RSS A){
    f_elm_t t0 = {0};

    f_rand_array(A, TAU - 1);
    for(ind_T T_ind = 0; T_ind < TAU - 1; T_ind++)
        f_add(t0, A[T_ind], t0);

    f_sub(a, t0, A[TAU - 1]);
}

void to_RSS_prng(const f_elm_t a, RSS A, prng_seed seed){
    f_elm_t t0 = {0};

    f_rand_array(A, TAU - 1, seed);
    for(ind_T T_ind = 0; T_ind < TAU - 1; T_ind++)
        f_add(t0, A[T_ind], t0);

    f_sub(a, t0, A[TAU - 1]);
}

// *Uses a longer seed, presendted as a pointer to 'nseeds' many seeds
void to_RSS_prng_long(const f_elm_t a, RSS A, prng_seed* seed, unsigned long long nseeds){
    f_elm_t t0 = {0};

    f_rand_array(A, TAU - 1, seed, nseeds);
    for(ind_T T_ind = 0; T_ind < TAU - 1; T_ind++)
        f_add(t0, A[T_ind], t0);

    f_sub(a, t0, A[TAU - 1]);
}


// Create a random DRSS secret sharing of a
void to_DRSS_urandom(const f_elm_t a, DRSS A){
    f_elm_t t0 = {0};

    f_rand_array(A, TAU * TAU - 1);
    for(ind_T T_ind = 0; T_ind < TAU * TAU - 1; T_ind++)
        f_add(t0, A[T_ind], t0);

    f_sub(a, t0, A[TAU * TAU - 1]);
}

void to_DRSS_prng(const f_elm_t a, DRSS A, prng_seed seed){
    f_elm_t t0 = {0};

    f_rand_array(A, TAU * TAU - 1, seed);
    for(ind_T T_ind = 0; T_ind < TAU * TAU - 1; T_ind++)
        f_add(t0, A[T_ind], t0);

    f_sub(a, t0, A[TAU * TAU - 1]);
}



/////////////////////////////////////////////
// Random secret shares (of random elements)
/////////////////////////////////////////////

// Random ASS secret share
void ASS_rand_urandom(ASS A, const subset_S S){
    for(int i = 0; i < S.card; i++)
        f_rand(A[S.com[i]]);
}

void ASS_rand_prng(ASS A, const subset_S S, prng_seed seed){
    for(int i = 0; i < S.card; i++)
        f_rand(A[S.com[i]], seed);
}


// Random RSS secret share
void RSS_rand_urandom(RSS A){
    f_rand_array(A, TAU);
}

void RSS_rand_prng(RSS A, prng_seed seed){
    f_rand_array(A, TAU, seed);
}


// Array of LAMBDA many random RSS secret shares
void RSS_rand_array_urandom(RSS A[LAMBDA]){
    f_rand_array(*A, TAU * LAMBDA);
}

void RSS_rand_array_prng(RSS A[LAMBDA], prng_seed seed){
    f_rand_array(*A, TAU * LAMBDA, seed);
}


// Random DRSS secret share
void DRSS_rand_urandom(DRSS A){
    f_rand_array(A, TAU * TAU);
}

void DRSS_rand_prng(DRSS A, prng_seed seed){
    f_rand_array(A, TAU * TAU, seed);
}



/////////////////////////////////////////////
// Random secret shares of zero
/////////////////////////////////////////////

// Random ASS secret share of zero
void ASS_zero_urandom(ASS A, const subset_S S){
    f_elm_t t0 = {0};
    to_ASS(t0, A, S);
}

void ASS_zero_prng(ASS A, const subset_S S, prng_seed seed){
    f_elm_t t0 = {0};
    to_ASS(t0, A, S, seed);
}


// Random RSS secret share of zero
void RSS_zero_urandom(RSS A){
    f_elm_t t0 = {0};
    to_RSS(t0, A);
}

void RSS_zero_prng(RSS A, prng_seed seed){
    f_elm_t t0 = {0};
    to_RSS(t0, A, seed);
}


// Random DRSS secret share of zero
void DRSS_zero_urandom(DRSS A){
    f_elm_t t0 = {0};
    to_DRSS(t0, A);
}

void DRSS_zero_prng(DRSS A, prng_seed seed){
    f_elm_t t0 = {0};
    to_DRSS(t0, A, seed);
}


/////////////////////////////////////////////
// Secret share opening
/////////////////////////////////////////////

// Open an ASS secret share
void open_ASS(const ASS A, const subset_S S, f_elm_t a){
    f_elm_t t0 = {0};

    for(ind_T T_ind = 0; T_ind < S.card; T_ind++)
        f_add(t0, A[S.com[T_ind]], t0);
    
    f_copy(t0, a);
}

// Open an RSS secret share
void open_RSS(const RSS A, f_elm_t a){
    f_elm_t t0 = {0};

    for(ind_T T_ind = 0; T_ind < TAU; T_ind++)
        f_add(t0, A[T_ind], t0);
    
    f_copy(t0, a);
}

// Open a DRSS secret share
void open_DRSS(const DRSS A, f_elm_t a){
    f_elm_t t0 = {0};

    for(ind_T T_ind = 0; T_ind < TAU * TAU; T_ind++)
        f_add(t0, A[T_ind], t0);
    
    f_copy(t0, a);
}



/////////////////////////////////////////////
// Secret share distribution to parties
/////////////////////////////////////////////

// Distribute a RSS x to CONST_N parties, each party recieves x_i[i] shares
void distribute_RSS(const RSS x, RSS_i x_i[CONST_N]){
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T = T_0; T.ind < TAU; next_T(&T)){
        for(int i = 0; i < CONST_N; i++){
            if(i_hold_T(i, T)){
                f_copy(x[T.ind], x_i[i][Ti_inds[i]++]);
            }
        }
    }
}

// Distribute a DRSS x to CONST_N parties, each party recieves x_i[i] shares
void distribute_DRSS(const DRSS x, DRSS_i x_i[CONST_N]){
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        for(int i = 0; i < CONST_N; i++){
            if(i_hold_TT(i, T0, T1)){
                f_copy(x[T0.ind * TAU + T1.ind], x_i[i][Ti_inds[i]++]);
            }
        }
    }}
}



/////////////////////////////////////////////
// Secret share reconstruction from party shares, with verification
/////////////////////////////////////////////

// Reconstructs an ASS x from party shares x_i
void reconstruct_ASS(const ASS_i* x_i, const subset_S S, ASS x){
    for(unsigned int i = 0; i < S.card; i++)
        f_copy(x_i[S.com[i]][0], x[S.com[i]]);
}

// Reconstructs an RSS x from party shares x_i, returns 0 if it is well formed, 1 otherwise
int reconstruct_RSS(const RSS_i x_i[CONST_N], RSS x){
    int test = 0;
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T = T_0; T.ind < TAU; next_T(&T)){
        int flag = 0;

        for(int i = CONST_N - 1; i >= 0; i--){
            if(i_hold_T(i, T)){
                if(!flag)
                    f_copy(x_i[i][Ti_inds[i]], x[T.ind]);
                test |= f_neq(x[T.ind], x_i[i][Ti_inds[i]++]);
                flag = 1;
            }
        }
    }
    return test;
}

// Reconstructs a DRSS x from party shares x_i, returns 0 if it is well formed, 1 otherwise
int reconstruct_DRSS(const DRSS_i x_i[CONST_N], DRSS x){
    int test = 0, flag = 0;
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        flag = 0;

        for(int i = CONST_N - 1; i >= 0; i--){
            if(i_hold_TT(i, T0, T1)){
                if(!flag){
                    f_copy(x_i[i][Ti_inds[i]], x[T0.ind * TAU + T1.ind]);
                    flag = 1;
                }
                test |= f_neq(x[T0.ind * TAU + T1.ind], x_i[i][Ti_inds[i]++]);
            }
        }
    }}
    return test;
}








/////////////////////////////////////////////
// Random seeds, their generation, distribution, and use for generating new secret shares
/////////////////////////////////////////////



/////////////////////////////////////////////
// Generating shared random seeds for generating secret shares of random elements.
/////////////////////////////////////////////

// Generate an RSS randomness seed from /dev/urandom
void random_RSS_seed_urandom(RSS_seed rseed){
    randombytes((unsigned char *)rseed, sizeof(RSS_seed));
}

// Generate an RSS randomness seed from a seed
void random_RSS_seed_prng(RSS_seed rseed, prng_seed seed){
    randombytes((unsigned char *)rseed, sizeof(RSS_seed), seed);
}


// Generate a DRSS randomness seed from /dev/urandom
void random_DRSS_seed_urandom(DRSS_seed dseed){
    randombytes((unsigned char *)dseed, sizeof(DRSS_seed));
}

// Generate a DRSS randomness seed from a seed
void random_DRSS_seed_prng(DRSS_seed dseed, prng_seed seed){
    randombytes((unsigned char *)dseed, sizeof(DRSS_seed), seed);
}




/////////////////////////////////////////////
// Distributing a pregenerated shared random seed for generating random elements
/////////////////////////////////////////////

// Distribute an ASS seed aseed to CONST_N parties, each party recieves aseed_i[i] shares
// Not needed, also it's trivial, just generate a random field element locally

// Distribute an RSS seed rseed to CONST_N parties, each party recieves rseed_i[i] shares
void distribute_RSS_seed(const RSS_seed rseed, RSS_seed_i rseed_i[CONST_N]){
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T = T_0; T.ind < TAU; next_T(&T)){
        for(int i = 0; i < CONST_N; i++){
            if(i_hold_T(i, T)){
                memcpy(rseed_i[i][Ti_inds[i]++], rseed[T.ind], NBYTES_SEED);
            }
        }
    }
}

// Distribute a DRSS seed dseed to CONST_N parties, each party recieves dseed_i[i] shares
void distribute_DRSS_seed(const DRSS_seed dseed, DRSS_seed_i dseed_i[CONST_N]){
    ind_T Ti_inds[CONST_N] = {0};

    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
        for(int i = 0; i < CONST_N; i++){
            if(i_hold_TT(i, T0, T1)){
                memcpy(dseed_i[i][Ti_inds[i]++], dseed[T0.ind * TAU + T1.ind], sizeof(prng_seed));
            }
        }
    }}
}


/////////////////////////////////////////////
// Compute a secret shares of a random element locally from a pre-shared seed
/////////////////////////////////////////////



// Compute an RSS share of a random element locally from seed
void random_RSS_local(RSS_i a, RSS_seed_i rseed_i){
    for(share_T T = T_0; T.ind < TAU_i; next_T(&T))
        f_rand(a[T.ind], rseed_i[T.ind]);
}

// Compute LAMBDA many RSS shares of a random element locally from seed
void random_RSS_local_array(const int nelms, RSS_i a[nelms], RSS_seed_i rseed_i){
    f_elm_t (*t)[nelms] = malloc(sizeof(f_elm_t[TAU_i][nelms]));

    for(share_T T = T_0; T.ind < TAU_i; next_T(&T))
        f_rand_array(t[T.ind], nelms, rseed_i[T.ind]);

    for(int j = 0; j < nelms; j++){
        for(share_T T = T_0; T.ind < TAU_i; next_T(&T))
            f_copy(t[T.ind][j], a[j][T.ind]);
    }
    
    free(t);
}






/////////////////////////////////////////////
// Generating shared random seeds for generating secret shares of zero.
/////////////////////////////////////////////

// Generate an ASS randomness seed for zeros from /dev/urandom
void random_ASS_seed_zero_urandom(ASS_seed_zero aseedz){
    randombytes((unsigned char *)aseedz, sizeof(ASS_seed_zero));

    for(int i = 0; i < CONST_N; i++)
        for(int j = 0; j < i; j++)
            memcpy(aseedz[i][j], aseedz[j][i], NBYTES_SEED);    
}

// Generate an ASS randomness seed for zeros from a seed
void random_ASS_seed_zero_prng(ASS_seed_zero aseedz, prng_seed seed){
    randombytes((unsigned char *)aseedz, sizeof(ASS_seed_zero), seed);

    for(int i = 0; i < CONST_N; i++)
        for(int j = 0; j < i; j++)
            memcpy(aseedz[i][j], aseedz[j][i], NBYTES_SEED);    
}

// Generate an RSS randomness seed for zeros from /dev/urandom
void random_RSS_seed_zero_urandom(RSS_seed_zero rseedz){
    randombytes((unsigned char *)rseedz, sizeof(RSS_seed_zero));

    for(int i = 0; i < TAU; i++)
        for(int j = 0; j < i; j++)
            memcpy(rseedz[i][j], rseedz[j][i], NBYTES_SEED);    
}

// Generate an RSS randomness seed for zeros from a seed
void random_RSS_seed_zero_prng(RSS_seed_zero rseedz, prng_seed seed){
    randombytes((unsigned char *)rseedz, sizeof(RSS_seed_zero), seed);

    for(int i = 0; i < TAU; i++)
        for(int j = 0; j < i; j++)
            memcpy(rseedz[i][j], rseedz[j][i], NBYTES_SEED);    
}

// Generate an DRSS randomness seed for zeros from /dev/urandom
void random_DRSS_seed_zero_urandom(DRSS_seed_zero dseedz){
    randombytes((unsigned char *)dseedz, sizeof(DRSS_seed_zero));

    for(int i = 0; i < TAU * TAU; i++)
        for(int j = 0; j < i; j++)
            memcpy(dseedz[i][j], dseedz[j][i], NBYTES_SEED);    
}

// Generate an DRSS randomness seed for zeros from a seed
void random_DRSS_seed_zero_prng(DRSS_seed_zero dseedz, prng_seed seed){
    randombytes((unsigned char *)dseedz, sizeof(DRSS_seed_zero), seed);
    for(int i = 0; i < TAU * TAU; i++){
        for(int j = 0; j < i; j++)
            memcpy(dseedz[i][j], dseedz[j][i], NBYTES_SEED);    
    }
}



/////////////////////////////////////////////
// Distributing a pregenerated shared random seed for generating shares of zero
/////////////////////////////////////////////

// Distribute an ASS_seed_zero aseedz to CONST_N parties, each party recieves aseedz_i[i] shares
void distribute_ASS_seed_zero(const ASS_seed_zero aseedz, ASS_seed_zero_i aseedz_i[CONST_N]){
    for(int i = 0; i < CONST_N; i++)
        memcpy(aseedz_i[i], aseedz[i], sizeof(prng_seed[CONST_N]));
}

// Distribute an RSS_seed_zero rseedz to CONST_N parties, each party recieves rseedz_i[i] shares
void distribute_RSS_seed_zero(const RSS_seed_zero rseedz, RSS_seed_zero_i rseedz_i[CONST_N]){
    for(int i = 0; i < CONST_N; i++){
        ind_T T_ind = 0;
        for(share_T T = T_0; T.ind < TAU; next_T(&T)){
            if(i_hold_T(i, T))
                memcpy(rseedz_i[i][T_ind++], rseedz[T.ind], sizeof(prng_seed[TAU]));
        }
    }
}

// Distribute an DRSS_seed_zero dseedz to CONST_N parties, each party recieves dseedz_i[i] shares
void distribute_DRSS_seed_zero(const DRSS_seed_zero dseedz, DRSS_seed_zero_i dseedz_i[CONST_N]){
    for(int i = 0; i < CONST_N; i++){
        ind_T T_ind = 0;
        for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
        for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
            if(i_hold_TT(i, T0, T1))
                memcpy(dseedz_i[i][T_ind++], dseedz[T0.ind * TAU + T1.ind], sizeof(prng_seed[TAU * TAU]));

        }}
    }
}



/////////////////////////////////////////////
// Compute secret shares of zero locally from a pre-shared seed
/////////////////////////////////////////////

// Compute an ASS share of zero locally from seed
void ASS_zero_local(const subset_S S, const int i, ASS_i a, ASS_seed_zero_i aseedz_i){
    f_elm_t t0 = {0}, t1;

    for(int j = 0; j < S.card; j++){
        if(S.com[j] < i){
            f_rand(t1, aseedz_i[S.com[j]]);
            f_add(t0, t1, t0);
        }
        else if(S.com[j] > i){
            f_rand(t1, aseedz_i[S.com[j]]);
            f_sub(t0, t1, t0);
        }
    }
    f_copy(t0, a[0]);
}

// Compute LAMBDA many ASS shares of zero locally from seed
void ASS_zero_local_array(const subset_S S, const int i, const int nelms, f_elm_t a[nelms], ASS_seed_zero_i aseedz_i){
    f_elm_t (*t1) = malloc(sizeof(f_elm_t[nelms]));
    f_elm_t (*t0) = malloc(sizeof(f_elm_t[nelms]));
    memset(t0, 0, sizeof(f_elm_t[nelms]));

    for(int j = 0; j < S.card; j++){
        if(S.com[j] < i){
            f_rand_array(t1, nelms, aseedz_i[S.com[j]]);
            for(int k = 0; k < nelms; k++)
                f_add(t0[k], t1[k], t0[k]);
        }
        else if(S.com[j] > i){
            f_rand_array(t1, nelms, aseedz_i[S.com[j]]);
            for(int k = 0; k < nelms; k++)
                f_sub(t0[k], t1[k], t0[k]);
        }

    }
    memcpy(a, t0, sizeof(f_elm_t[nelms]));
    free(t0);
    free(t1);
}


// Compute an RSS share of zero locally from seed
void RSS_zero_local(const int i, RSS_i a, RSS_seed_zero_i rseedz_i){
    f_elm_t t1;
    ind_T T0_ind = 0;

    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
        f_elm_t t0 = {0};
        if(i_hold_T(i, T0)){
            for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
                if(T0.ind < T1.ind){
                    f_rand(t1, rseedz_i[T0_ind][T1.ind]);
                    f_add(t0, t1, t0);
                }
                else if(T0.ind > T1.ind){
                    f_rand(t1, rseedz_i[T0_ind][T1.ind]);
                    f_sub(t0, t1, t0);
                }
            }
            f_copy(t0, a[T0_ind++]);
        }
    }
}

// Compute LAMBDA many RSS shares of zero locally from seed
void RSS_zero_local_array(const int i, const int nelms, f_elm_t a[TAU_i][nelms], RSS_seed_zero_i rseedz_i){
    f_elm_t (*t0) = malloc(sizeof(f_elm_t[nelms]));
    f_elm_t (*t1) = malloc(sizeof(f_elm_t[nelms]));
    ind_T T0_ind = 0;

    for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
        if(i_hold_T(i, T0)){
            memset(t0, 0, sizeof(f_elm_t[nelms]));

            for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
                if(T0.ind < T1.ind){
                    f_rand_array(t1, nelms, rseedz_i[T0_ind][T1.ind]);
                    for(int k = 0; k < nelms; k++)
                        f_add(t0[k], t1[k], t0[k]);
                }
                else if(T0.ind > T1.ind){
                    f_rand_array(t1, nelms, rseedz_i[T0_ind][T1.ind]);
                    for(int k = 0; k < nelms; k++)
                        f_sub(t0[k], t1[k], t0[k]);
                }
            }
            memcpy(a[T0_ind++], t0, sizeof(f_elm_t[nelms]));
        }
    }
    free(t0);
    free(t1);
}


// Compute an DRSS share of zero locally from seed
void DRSS_zero_local(const int i, DRSS_i a, DRSS_seed_zero_i dseedz_i){
    f_elm_t t1;
    ind_T T0_ind = 0;

    for(share_T T00 = T_0; T00.ind < TAU; next_T(&T00)){
    for(share_T T01 = T_0; T01.ind < TAU; next_T(&T01)){
        if(i_hold_TT(i, T00, T01)){
            f_elm_t t0 = {0};

            for(share_T T10 = T_0; T10.ind < TAU; next_T(&T10)){
            for(share_T T11 = T_0; T11.ind < TAU; next_T(&T11)){
                if(T00.ind * TAU + T01.ind < T10.ind * TAU + T11.ind){
                    f_rand(t1, dseedz_i[T0_ind][T10.ind * TAU + T11.ind]);
                    f_add(t0, t1, t0);
                }
                else if(T00.ind * TAU + T01.ind > T10.ind * TAU + T11.ind){
                    f_rand(t1, dseedz_i[T0_ind][T10.ind * TAU + T11.ind]);
                    f_sub(t0, t1, t0);
                }
            }}
            f_copy(t0, a[T0_ind++]);
        }
    }}
}

// Compute LAMBDA many DRSS shares of zero locally from seed
void DRSS_zero_local_array(const int i, const int nelms, f_elm_t a[TAU_i * TAU_i][nelms], DRSS_seed_zero_i dseedz_i){
    f_elm_t (*t1) = malloc(sizeof(f_elm_t[nelms]));
    f_elm_t (*t0) = malloc(sizeof(f_elm_t[nelms]));
    ind_T T0_ind = 0;
    for(share_T T00 = T_0; T00.ind < TAU; next_T(&T00)){
    for(share_T T01 = T_0; T01.ind < TAU; next_T(&T01)){
        if(i_hold_TT(i, T00, T01)){
            memset(t0, 0, sizeof(f_elm_t[nelms]));

            for(share_T T10 = T_0; T10.ind < TAU; next_T(&T10)){
            for(share_T T11 = T_0; T11.ind < TAU; next_T(&T11)){
                if(T00.ind * TAU + T01.ind < T10.ind * TAU + T11.ind){

                    f_rand_array(t1, nelms, dseedz_i[T0_ind][T10.ind * TAU + T11.ind]);
                    for(int k = 0; k < nelms; k++)
                        f_add(t0[k], t1[k], t0[k]);
                }
                else if(T00.ind * TAU + T01.ind > T10.ind * TAU + T11.ind){
                    f_rand_array(t1, nelms, dseedz_i[T0_ind][T10.ind * TAU + T11.ind]);
                    for(int k = 0; k < nelms; k++)
                        f_sub(t0[k], t1[k], t0[k]);
                }
            }}
            memcpy(a[T0_ind++], t0, sizeof(f_elm_t[nelms]));
        }
    }}
    free(t0);
    free(t1);
}






// Multiply RSS local shares into an ASS local share
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

// Multiply two RSS local shares and output a local DRSS share
void mul_RSS_DRSS(const RSS_i a, const RSS_i b, DRSS_i c){
    share_T T0, T1;

    for(T0 = T_0; T0.ind < TAU_i; next_T(&T0)){
    for(T1 = T_0; T1.ind < TAU_i; next_T(&T1)){
        f_mul(a[T0.ind], b[T1.ind], c[T0.ind * TAU_i + T1.ind]);
    }}
}






// Print RSS secret sharing and the distribution to n parties
// For debugging purposes and checking if shares are ok
unsigned char test_RSS_print(const RSS x, const RSS_i x_i[CONST_N]){
    share_T T;
    ind_T Ti_inds[CONST_N] = {0};

    unsigned char all_tests = 0, test = 0;

    for(T = T_0; T.ind < TAU; next_T(&T)){
        print_T(T);
        print_f_elm(x[T.ind]); printf("  |\t");
        test = 0;

        for(int i = CONST_N - 1; i >= 0; i--){
            if(i_hold_T(i, T)){
                test |= f_neq(x[T.ind], x_i[i][Ti_inds[i]]);
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






void mult_RSS_to_RSS_evaluation(const RSS_i a, const RSS_i b, RSS C[TAU_i * TAU_i], DRSS_seed_i seed){
    f_elm_t t0;
    prng_seed rss_seed[2];


    for(share_T T0 = T_0; T0.ind < TAU_i; next_T(&T0)){
    for(share_T T1 = T_0; T1.ind < TAU_i; next_T(&T1)){
        f_mul(a[T0.ind], b[T1.ind], t0);
    
        memcpy(rss_seed[0], seed[T0.ind * TAU_i + T1.ind], NBYTES_SEED);
        memcpy(rss_seed[1], t0,                            NBYTES_SEED);

        to_RSS(t0, C[T0.ind * TAU_i + T1.ind], rss_seed, 2);    
        memcpy(seed[T0.ind * TAU_i + T1.ind], rss_seed[0], NBYTES_SEED);
    }}
}



int mult_RSS_to_RSS_reconstruction(const RSS_i c_j[CONST_N][TAU_i * TAU_i], RSS_i c){
    int test = 0;

    for(share_T T = T_0; T.ind < TAU_i; next_T(&T)){
        ind_T Tj_inds[CONST_N] = {0};
        f_elm_t t_T = {0};

        for(share_T T0 = T_0; T0.ind < TAU; next_T(&T0)){
        for(share_T T1 = T_0; T1.ind < TAU; next_T(&T1)){
            f_elm_t t0;
            int flag = 0;
            int test_T0_T1 = 0;

            for(int j = 0; j < CONST_N; j++){
                if(i_hold_TT(j, T0, T1)){
                    if(!flag) 
                        f_copy(c_j[j][Tj_inds[j]][T.ind], t0);
                    test_T0_T1 |= f_neq(c_j[j][Tj_inds[j]++][T.ind], t0);
                    flag = 1;
                }
            }
            f_add(t_T, t0, t_T);
            test |= test_T0_T1;
        }}
        f_copy(t_T, c[T.ind]);
    }
    return test;
}


void serialize_ASS_i(ASS_i *data, uint8_t *buffer)
{
    memcpy(buffer, &(*data), sizeof(f_elm_t));
}

void deserialize_ASS_i(uint8_t *buffer, ASS_i *data)
{
    memcpy(&(*data), buffer, sizeof(f_elm_t));
}


// Function to serialize RSS_i into a byte array
void serialize_RSS_i(RSS_i *data, uint8_t *buffer)
{
    int offset = 0;
    for (int i = 0; i < TAU_i; i++)
    {
        memcpy(buffer + offset, &(*data)[i], sizeof(f_elm_t));
        offset += sizeof(f_elm_t);
    }
}

// Function to deserialize byte array into RSS_i
void deserialize_RSS_i(uint8_t *buffer, RSS_i *data)
{
    int offset = 0;
    for (int i = 0; i < TAU_i; i++)
    {
        memcpy(&(*data)[i], buffer + offset, sizeof(f_elm_t));
        offset += sizeof(f_elm_t);
    }
}

// Serialize DRSS_i
void serialize_DRSS_i(DRSS_i *data, uint8_t *buffer)
{
    int offset = 0;
    for (int i = 0; i < TAU_i * TAU_i; i++)
    {
        memcpy(buffer + offset, &(*data)[i], sizeof(f_elm_t));
        offset += sizeof(f_elm_t);
    }
}

// Serialize hash_digest_i
void serialize_hash_digest_i(hash_digest *data, uint8_t *buffer)
{
    int offset = 0;
    for (int i = 0; i < 1; i++)
    {
        memcpy(buffer + offset, &(*data)[i], sizeof(hash_digest));
        offset += sizeof(hash_digest);
    }
}

void deserialize_DRSS_i(uint8_t *buffer, DRSS_i *data)
{
    int offset = 0;
    for (int i = 0; i < TAU_i * TAU_i; i++)
    {
        memcpy(&(*data)[i], buffer + offset, sizeof(f_elm_t));
        offset += sizeof(f_elm_t);
    }
}

void deserialize_hash_digest_i(uint8_t *buffer, hash_digest *data)
{
    int offset = 0;
    for (int i = 0; i < 1; i++)
    {
        memcpy(&(*data)[i], buffer + offset, sizeof(hash_digest));
        offset += sizeof(hash_digest);
    }
}

/////////////////////////////////////////////
// CALCULATE LEGENDRE SYMBOLS (SEMI-HONEST AND MALICIOUS)
/////////////////////////////////////////////
void calc_symbols(f_elm_t o[LAMBDA], unsigned char LOPRF[PRF_BYTES])
{
    int index, pos;
    memset(LOPRF, 0, PRF_BYTES);

    for (int j = 0; j < LAMBDA; j++)
    {
        unsigned char t = 0;
        index = j / 8;
        pos = j % 8;

        f_leg(o[j], &t);
        LOPRF[index] |= (t << pos);
    }
}