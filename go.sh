#!/usr/bin/env bash

TARGET="dOPRF"
BITSIZES=("128" "192" "256")
FAST="GENERIC"
TYPE_PRIMES="ORIGINAL"

for arg in "$@"
do
    if [[ "$arg" == "dOPRF" || "$arg" == "arith" ]]; then
        TARGET="$arg"
    elif [[ "$arg" =~ ^64$|^128$|^192$|^256$|^512$ ]]; then
        BITSIZES=("$arg")
    elif [[ "$arg" == "FAST" ]]; then
        FAST="$arg"
    elif [[ "$arg" == "ALT" ]]; then
        TYPE_PRIMES="$arg"
    else 
        echo "Warning: Unknown argument $arg"
        echo "Accepted arguments are:"
        echo "  - 'dOPRF' or 'arith' to set the TARGET"
        echo "  - '64', '128', '192', '256', or '512' to set the BITSIZE"
        echo "  - 'ALT' to enable alternative prime values"
        echo "  - 'FAST' to enable ARM assembly"
        exit 1
    fi
done


make clean > /dev/null
for BITSIZE in "${BITSIZES[@]}"
do
    make ${TARGET}${BITSIZE} OPT_LEVEL=${FAST} ALT_PRIMES=${TYPE_PRIMES} > /dev/null
    # make ${TARGET}${BITSIZE} OPT_LEVEL=${FAST} ALT_PRIMES=${TYPE_PRIMES}
    ./${TARGET}${BITSIZE}
    make clean > /dev/null
done

