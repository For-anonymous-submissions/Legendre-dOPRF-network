#!/bin/bash
# Define parameter sets for CONST_T and CONST_N
PARAM_SETS=(
    "1 4"  # First set: CONST_T=1, CONST_N=4
    "2 7"  # Second set: CONST_T=2, CONST_N=8
)

# File containing the macros
HEADER_FILE="dOPRF.h"

# Iterate over each parameter set
for PARAMS in "${PARAM_SETS[@]}"; do
    set -- $PARAMS
    CONST_T=$1
    CONST_N=$2

    echo "Running tests with CONST_T=$CONST_T and CONST_N=$CONST_N"
    # Modify the macro definitions in the header file
    sed -i '' "s/^#define CONST_T .*/#define CONST_T $CONST_T/" $HEADER_FILE
    sed -i '' "s/^#define CONST_N .*/#define CONST_N $CONST_N/" $HEADER_FILE

    echo "Running tests with CONST_T=$CONST_T and CONST_N=$CONST_N"

    # Compile the code
    make clean
    make

   # Define server amounts based on CONST_N
    IDS=($(seq 0 $((CONST_N - 1))))

    # Start the servers
    SERVER_PIDS=()
    for ID in "${IDS[@]}"; do
        echo "Starting server $ID with CONST_T=$CONST_T and CONST_N=$CONST_N"
        ./server128 $ID &
        SERVER_PIDS+=($!)
    done

    # Give servers a moment to start up
    sleep 2

    # Start the client
    echo "Starting client with CONST_T=$CONST_T and CONST_N=$CONST_N"
    ./client128 > "client128_T${CONST_T}_N${CONST_N}.log" 2>&1

    # Wait for client to finish
    CLIENT_PID=$!
    wait $CLIENT_PID

    echo "Completed tests with CONST_T=$CONST_T and CONST_N=$CONST_N"
done

echo "All tests completed. Logs are available in client128_T*_N*.log."
