#!/bin/bash
set -e  # Exit immediately if a command exits with a non-zero status

SEMIHONEST_PARAM_SETS=(
    "1 3"  # First set: CONST_T=1, CONST_N=3
    "2 5"  # Second set: CONST_T=2, CONST_N=5
    "3 7"  # Second set: CONST_T=2, CONST_N=5
)

# Define parameter sets for each setting
MALICIOUS_PARAM_SETS=(
    "1 4"  # First set: CONST_T=1, CONST_N=4
    "2 7"  # Second set: CONST_T=2, CONST_N=7
)

# File containing the macros
HEADER_FILE="dOPRF.h"

# Function to clean up server processes
cleanup_servers() {
    if [[ ${#SERVER_PIDS[@]} -gt 0 ]]; then
        echo "Waiting for server processes to finish..."
        for PID in "${SERVER_PIDS[@]}"; do
            wait $PID 2>/dev/null || true  # Wait for each server process to complete
        done
        echo "All server processes completed."
    fi
}

# Ensure cleanup on exit (even if the script fails)
# trap cleanup_servers EXIT

# Function to run tests
run_tests () {
    local SETTING=$1
    shift
    local PARAM_SETS=("$@")
    for PARAMS in "${PARAM_SETS[@]}"; do
        set -- $PARAMS
        CONST_T=$1
        CONST_N=$2

        echo "Running tests with CONST_T=$CONST_T and CONST_N=$CONST_N"

        # Modify the macro definitions in the header file
        # Problems with OSX - keeps creating new files.
        # sed -i'' -e "s/^#define CONST_T .*/#define CONST_T $CONST_T/" $HEADER_FILE
        # sed -i'' -e "s/^#define CONST_N .*/#define CONST_N $CONST_N/" $HEADER_FILE

        # Temporarily redirect the output to a new file and replace the original
        sed "s/^#define CONST_T .*/#define CONST_T $CONST_T/" $HEADER_FILE > tmpfile && mv tmpfile $HEADER_FILE
        sed "s/^#define CONST_N .*/#define CONST_N $CONST_N/" $HEADER_FILE > tmpfile && mv tmpfile $HEADER_FILE


        # Compile the code
        make clean > /dev/null 2>&1
        make > /dev/null 2>&1

        # Define server amounts based on CONST_N
        IDS=($(seq 0 $((CONST_N - 1))))

        # Start the servers and track PIDs
        SERVER_PIDS=()  # Reset the PID list
        for ID in "${IDS[@]}"; do
            # echo "Starting server $ID with CONST_T=$CONST_T and CONST_N=$CONST_N"
            ./server256 $ID &
            SERVER_PIDS+=($!)  # Store the PID of the server
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
        ./client256 > "client256_T${CONST_T}_N${CONST_N}_${SETTING}.log" 2>&1

        # Wait for client to finish
        CLIENT_PID=$!
        wait $CLIENT_PID

        echo "Completed tests with CONST_T=$CONST_T and CONST_N=$CONST_N"

        # Kill all server processes for this run
        cleanup_servers

    done
}

# Run tests for semi-honest setting
# sed -i'' -e "s/^#define ADVERSARY .*/#define ADVERSARY SEMIHONEST/" $HEADER_FILE
# run_tests  "SH" "${SEMIHONEST_PARAM_SETS[@]}"
# python3 measure_offline.py 0 128

# Run tests for malicious setting
sed -i'' -e "s/^#define ADVERSARY .*/#define ADVERSARY MALICIOUS/" $HEADER_FILE
run_tests  "MAL" "${MALICIOUS_PARAM_SETS[@]}"
python3 measure_offline.py 1 128


make clean > /dev/null 2>&1
cleanup_servers

echo "All tests completed. Logs are available in client256_T*_N*.log."