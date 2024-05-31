# Legendre-dOPRF
An arithmetic library for the Legendre PRF based distributed OPRF

The optimal way to run experiments is to use the `go.sh` script.
Otherwise, read below how to use the makefile.

### Dependencies
`gcc, blake3`
OSX:
	Install the blake3 library with homebrew
	`brew install blake3`
	The library is loaded with the `-lblake3` flag in the makefile
Linux:
	Install blake3 by following the instructions here
	https://github.com/BLAKE3-team/BLAKE3
	https://github.com/BLAKE3-team/BLAKE3/tree/master/c


# How to get efficiency results for the adpated network communication

To get the results for a security level of 128, simply run `go128.sh`, then you will obtain a list of files 
`client256*.log` file which records the online phase efficiency regarding online time and communication, additionally you get estimated offline phase run time at files `offline_SH_cost.log`or `offline_Mal_cost.log`.

Similarly, you run `go256.sh` if you want to get result for a security level of 256.

# Using the benching script

The simplest way to run all experiments is to use the benching script `go.sh`.
Start by giving it execution priviledges.
`chmod +x go.sh`

The script uses bash, so make sure the bash location is correct.
Change the first line `#!/usr/bin/env bash` if needed

This script cleans, compiles and runs the program. Make sure execution priviledges are provided for the directory. 

The script accepts four types of arguments:
- `dOPRF` or `arith` to run the distributed OPRF, or the arithmetic tests and benchmarks
- `64`, `128`, `192`, `256`, or `512` to set the bitsize of the prime
- `ALT` to select alternative prime values
- `FAST` to enable ARM assembly speedups

By default, the script runs `dOPRF` tests for all prime sizes with original primes and no assembly.

Examples of usage:

- `./go.sh` Makes all prime sizes and runs all tests and benchmarks for dOPRF

- `./go.sh FAST` Makes all prime sizes with ARM64 assebly, and runs all tests and benchmark for dOPRF

- `./go.sh X` Makes X bit prime arithmetic, runs all tests and benchmark for dOPRF

- `./go.sh X FAST` Makes X bit prime arithmetic with assembly, runs all tests and benchmarks dOPRF

- `./go.sh arith 192 ALT FAST` Runs arithmetic tests and benchmarks for alternative 192 bit prime with ARM assembly on


# Using the makefile

Run `make` which will compile all dOPRF tests and benchmarks as well as arithmetic tests and benchmarks for 64,128,192,256,512 bit sized prime fields. The executables are dOPRF64, dOPRF128, dOPRF192, dOPRF256, dOPRF512, and arith64, arith128, arith192, arith256, arith512 correspondingly.

By running `make dOPRF64` you will only compile the 64 bit prime dOPRF (and respectively for 128, 192, 256, 512). 

By running `make arith64` you will only compile the 64 bit prime dOPRF (and respectively for 128, 192, 256, 512). 

The makefile accepts the following flags
- `make ALT_PRIMES=ALT` if you want to use alternative primes
- `make OPT_LEVEL=FAST` if you want to use ARM assembly


Example usage:
- `make dOPRF256 OPT_LEVEL=FAST` makes the executable for 256-bit prime with ARM assembly. You can run it with `./dOPRF256`. 



# Changing parameters

- The prime size parameter is selected at make, and can be chosen to be `64`, `128`, `192`, `256` or `512`. Internally it is referred to as `NBITS_FIELD`.
- Each prime size has two hard-coded primes. By default `PRIMES=ORIGINAL` is selected. In the makefile you can choose to change to `PRIMES=ALT` in order to use the alternative primes. Primes are in `parameters.h`.

- The number of output bits is determined by the `LAMBDA` paramter. It is set to be `NBITS_FIELD/2` as to preserve a security level of `LAMBDA`. It can be changed in `dOPRF.h` at line 32
- The adversarial type is determined by the `ADVERSASRY` parameter. By default it is set to `MALICIOUS`. It can be changed to `SEMIHONEST` in `dOPRF.h` by commenting out lines 19-20.
- The threshold security parameters `(t,n)` are called `CONST_T` and `CONST_N`. By default they are `(1,4)`. They can be changed in `dOPRF.h` in lines 24, 28.
- The randomness seed size is determined by the `NBYTES_SEED` parameter. By default `NBYTES_SEED=NBYTES_PRIME`. You can change it in `parameters.h` in line 45.
- The hash digest size is determined by the `NBYTES_DIGEST` parameter. By default `NBYTES_DIGEST=NBYTES_PRIME`. You can change it in `parameters.h` in line 48.

- The number of test loops for benchmarking the dOPRF is defined with the `LOOPS`  parameter in lines 12-25 of `main.c`.
- The number of test loops for benchmarking the arithmetic is defined in `bench.h` file.

---

# Copyright 2024 REDACTED FOR ANONYMITY

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


