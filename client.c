#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "arith.h"
#include "dOPRF.h"
#include "network.h"

void input(const f_elm_t x, RSS_i x_i[CONST_N], prng_seed seed)
{
    RSS X;
    to_RSS(x, X, seed);
    distribute_RSS(X, x_i);
}

void *communicate_with_server(void *arg)
{
    server_data_t *data = (server_data_t *)arg;
    int sock = data->sock;

    // Serialize RSS_i data
    uint8_t serialized_data[sizeof(RSS_i)];
    serialize_RSS_i(data->message, serialized_data);

    send(sock, serialized_data, sizeof(serialized_data), 0);



    // Receive response from server
    int total_size = (sizeof(DRSS_i) * LAMBDA) + sizeof(DRSS_digest_i);
    int bytes_received = 0;
    while (bytes_received < total_size) {
        int result = read(sock, data->response + bytes_received, total_size - bytes_received);
        if (result < 0) {
            perror("Read error");
            close(sock);
            return NULL;
        }
        bytes_received += result;
    }

    

    printf("Received and deserialized message from server %d\n", data->server_index + 1);

    close(sock);
    return NULL;
}

/////////////////////////////////////////////
// MALICIOUS RECONSTRUCTION PROTOCOL
/////////////////////////////////////////////
int mal_reconstruction(const DRSS_i o_i[CONST_N][LAMBDA], const DRSS_digest_i h_i[CONST_N], f_elm_t o[LAMBDA])
{

    hash_digest t_hd;
    f_elm_t(*temp)[TAU * TAU][LAMBDA] = malloc(sizeof(f_elm_t[CONST_N][TAU * TAU][LAMBDA]));
    int ret = 0; // Return 1 if detected anomalies (i.e. hashes incorrect)

    for (int j = 0; j < LAMBDA; j++)
    {
        ind_T Ti_inds_0[CONST_N] = {0};
        f_elm_t v = {0}; // v =?= a_{T_0} * b_{T_1} + r_{T_0T_1} = o_{T_0T_1}

        for (share_T T0 = T_0; T0.ind < TAU; next_T(&T0))
        {
            for (share_T T1 = T_0; T1.ind < TAU; next_T(&T1))
            {
                f_elm_t v_TT = {0};

                for (int i = 0; i < CONST_N; i++)
                {
                    if (i_hold_TT(i, T0, T1))
                    {
                        f_add(v_TT, o_i[i][j][Ti_inds_0[i]++], v_TT);
                    }
                }
                f_mul(v_TT, f_inverses[S_TT(T0, T1)], v_TT);
                f_add(v, v_TT, v);

                f_copy(v_TT, temp[0][T0.ind * TAU + T1.ind][j]);
            }
        }
        f_copy(v, o[j]);
    }

    ind_T Ti_inds_0[CONST_N] = {0};

    for (share_T T0 = T_0; T0.ind < TAU; next_T(&T0))
    {
        for (share_T T1 = T_0; T1.ind < TAU; next_T(&T1))
        {

            hash_array(temp[0][T0.ind * TAU + T1.ind], LAMBDA, t_hd); // h =?= o_{T_0T_1}
            for (int i = 0; i < CONST_N; i++)
            {
                if (i_hold_TT(i, T0, T1))
                {
                    if (memcmp(h_i[i][Ti_inds_0[i]++], t_hd, NBYTES_DIGEST) != 0)
                        ret = 1;
                }
            }
        }
    }

    free(temp);

    return ret;
}

int main()
{   
    int all_commu_size=0;

    init_inverses();
    generate_table();

    pthread_t threads[CONST_N];
    server_data_t server_data[CONST_N];
    int socks[CONST_N];
    struct sockaddr_in serv_addrs[CONST_N];
    struct timespec start, end;
    long elapsed_ns;
    double elapsed_ms;

    // Establish socket connections
    for (int i = 0; i < CONST_N; i++)
    {
        setup_client_socket(&server_data[i].sock, "127.0.0.1", SERVER_PORT_BASE + i);
        server_data[i].server_index = i;
    }

    // Get the start time before dealing with messages
    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1)
    {
        error("clock_gettime");
    }

/*********Here starts the client input stage************/

    prng_seed seed = {0};
    f_elm_t x;
    RSS_i(*x_i) = malloc(sizeof(RSS_i[CONST_N]));

    f_rand(x, seed);
    input(x, x_i, seed);
/*********Here ends the client input stage************/

    all_commu_size += CONST_N*sizeof(RSS_i);

        for (int i = 0; i < CONST_N; i++)
    {
        memcpy(server_data[i].message, x_i[i], sizeof(RSS_i));
    }

    // Create threads to communicate with servers simultaneously
    for (int i = 0; i < CONST_N; i++)
    {
        if (pthread_create(&threads[i], NULL, communicate_with_server, &server_data[i]) != 0)
        {
            error("Failed to create thread");
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < CONST_N; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Correct memory allocation
    DRSS_i (*o_i_all)[CONST_N][LAMBDA] = malloc(CONST_N * LAMBDA * sizeof(DRSS_i));
    DRSS_digest_i (*h_i_all)[CONST_N] = malloc(CONST_N * sizeof(DRSS_digest_i));

    all_commu_size += CONST_N * (sizeof(DRSS_i) * LAMBDA + sizeof(DRSS_digest_i));

    //Deserialize the data from the response buffer
    for (int i = 0; i < CONST_N; i++) {
       
       int offset = 0;
       for (int j = 0; j < LAMBDA; j++)
       {
           deserialize_DRSS_i(server_data[i].response + offset, (*o_i_all)[i][j]);
           offset += sizeof(DRSS_i);
       }
       deserialize_DRSS_digest_i(server_data->response + offset, (*h_i_all)[i]);
    }

    // CLIENT RECONSTRUCTION STAGE
    ////////////////////////////////////////////
    f_elm_t(*o) = malloc(LAMBDA * sizeof(f_elm_t));
    unsigned char L[PRF_BYTES] = {0};
    ////////////////////////////////////////////
    // Client reconstructs received values
    // Returns 1 if servers acted dishonestly, otherwise 0
    int test=0;
    test |= mal_reconstruction(o_i_all, h_i_all, o);

    ////////////////////////////////////////////
    // Client computes Legendre symbols
    calc_symbols(o, L);

    // Free allocated memory
    free(o_i_all);
    free(h_i_all);
    free(o);


    // Get the end time after all messages have been dealt with
    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1)
    {
        error("clock_gettime");
    }

    // Calculate the elapsed time in milliseconds
    elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    elapsed_ms = elapsed_ns / 1e6;

    printf("Total time required dealing messages: %.2f ms\n", elapsed_ms);
    printf("Total communication size: %.2f KB\n", (float)all_commu_size / 1024);

    // Close all sockets
    for (int i = 0; i < CONST_N; i++)
    {
        close(server_data[i].sock);
    }

    return 0;
}
