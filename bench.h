#ifndef BENCH_H
#define BENCH_H

// #include <stdint.h>
// #include <stdio.h>

// #include "random/random.h"

// Benchmarking
#define WARMUP          10000
#define BENCH_LOOPS     20000
#define TEST_LOOPS      1000
#define CLK_PER_SEC     3.5E9
#define CLK_PER_nSEC    3.5

uint64_t cpucycles(void);
const char* print_num(double a);


#endif