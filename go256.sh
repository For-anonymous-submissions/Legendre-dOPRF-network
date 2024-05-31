#!/bin/bash
set -e  # Exit immediately if a command exits with a non-zero status

# Define parameter sets for each setting
MALICIOUS_PARAM_SETS=(
    # "1 4"  # First set: CONST_T=1, CONST_N=4
    "2 7"  # Second set: CONST_T=2, CONST_N=7
    # "3 10"  # Second set: CONST_T=2, CONST_N=7
)

SEMIHONEST_PARAM_SETS=(
    "1 3"  # First set: CONST_T=1, CONST_N=3
    "2 5"  # Second set: CONST_T=2, CONST_N=5
    "3 7"  # Second set: CONST_T=2, CONST_N=5
)

# File containing the macros
HEADER_FILE="dOPRF.h"

# Function to run tests
run_tests () {
    local SETTING=$1
    shift
    local PARAM_SETS=("$@")
    for PARAMS in "${PARAM_SETS[@]}"; do
        set -- $PARAMS
        CONST_T=$1
        CONST_N=$2

        # echo "Running tests with CONST_T=$CONST_T and CONST_N=$CONST_N"
        # Modify the macro definitions in the header file
        sed -i'' -e "s/^#define CONST_T .*/#define CONST_T $CONST_T/" $HEADER_FILE
        sed -i'' -e "s/^#define CONST_N .*/#define CONST_N $CONST_N/" $HEADER_FILE

        echo "Running tests with CONST_T=$CONST_T and CONST_N=$CONST_N"

        # Compile the code
        make clean
        make

        # Define server amounts based on CONST_N
        IDS=($(seq 0 $((CONST_N - 1))))

        # Start the servers
        for ID in "${IDS[@]}"; do
            # echo "Starting server $ID with CONST_T=$CONST_T and CONST_N=$CONST_N"
            ./server512 $ID &
        done

        # Give servers a moment to start up
        # Get the size of the IDS array
        IDS_SIZE=${#IDS[@]}
        if [ $IDS_SIZE -gt 4 ]; then
            sleep 5
        else
            sleep 2
        fi

        # Start the client
        echo "Starting client with CONST_T=$CONST_T and CONST_N=$CONST_N"
        ./client512 > "client512_T${CONST_T}_N${CONST_N}_${SETTING}.log" 2>&1

        # Wait for client to finish
        CLIENT_PID=$!
        wait $CLIENT_PID

        echo "Completed tests with CONST_T=$CONST_T and CONST_N=$CONST_N"
    done
}

# Run tests for semi-honest setting
sed -i'' -e "s/^#define ADVERSARY .*/#define ADVERSARY SEMIHONEST/" $HEADER_FILE
# run_tests  "SH" "${SEMIHONEST_PARAM_SETS[@]}"
# python3 measure_offline.py 0 256

# Run tests for malicious setting
sed -i'' -e "s/^#define ADVERSARY .*/#define ADVERSARY MALICIOUS/" $HEADER_FILE
run_tests  "MAL" "${MALICIOUS_PARAM_SETS[@]}"
python3 measure_offline.py 1 256

echo "All tests completed. Logs are available in client512_T*_N*.log."