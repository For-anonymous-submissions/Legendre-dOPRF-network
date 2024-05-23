# Makefile
CC = gcc
CFLAGS += -Wall -Wextra -std=gnu11 -Wcomment -O3 -Ofast
# CFLAGS += -g -fsanitize=address -ferror-limit=1
CFLAGS += -I/usr/local/include
LDFLAGS += -lblake3

ifneq ($(wildcard /usr/local/lib/libblake3.a),)
    LIBS += /usr/local/lib/libblake3.a
endif

.NOTPARALLEL:

# Optimization flag
OPT_LEVEL = $(DEFAULT_OPT_LEVEL)
DEFAULT_OPT_LEVEL = GENERIC

# Alternative primes
ifeq ($(ALT_PRIMES),ALT)
PRIMES_VAL=1
else
PRIMES_VAL=0
endif

# Basic source files
SRC = random/random.c \
      arith.c \
 	  bench.c \
	  dOPRF.c \
	  network.c \

# Add arithmetic source files
ifeq ($(OPT_LEVEL),GENERIC)
SRC64 = $(SRC)  p64/generic/arith_generic.c
SRC128 = $(SRC) p128/generic/arith_generic.c
SRC192 = $(SRC) p192/generic/arith_generic.c
SRC256 = $(SRC) p256/generic/arith_generic.c
SRC512 = $(SRC) p512/generic/arith_generic.c
else ifeq ($(OPT_LEVEL),FAST)
SRC64 = $(SRC)  p64/arm64/arith_arm64.c   p64/arm64/arith_arm64.S
SRC128 = $(SRC) p128/arm64/arith_arm128.c p128/arm64/arith_arm128.S
SRC192 = $(SRC) p192/arm64/arith_arm192.c p192/arm64/arith_arm192.S
SRC256 = $(SRC) p256/arm64/arith_arm256.c p256/arm64/arith_arm256.S
SRC512 = $(SRC) p512/arm64/arith_arm512.c p512/arm64/arith_arm512.S
endif


.PHONY: all client server clean

all:  client server

client: client128 

server: server128
# client: client128 client256 client512

# server: server128 server256 server512

clean:
	rm -f client128  server128
# clean:
# 	rm -f client128 client256 client512 server128 server256 server512


client128: client.c $(SRC128)
	$(CC) $(CFLAGS)   -DSEC_LEVEL=1 -DPRIMES=$(PRIMES_VAL) -o $@ $^ $(LIBS)

# client256: client.c $(SRC256)
# 	$(CC) $(CFLAGS)   -DSEC_LEVEL=1 -DPRIMES=$(PRIMES_VAL) -o $@ $^ $(LIBS)

# client512: client.c $(SRC512)
# 	$(CC) $(CFLAGS)   -DSEC_LEVEL=1 -DPRIMES=$(PRIMES_VAL) -o $@ $^ $(LIBS)

server128: server.c $(SRC128)
	$(CC) $(CFLAGS)   -DSEC_LEVEL=1 -DPRIMES=$(PRIMES_VAL) -o $@ $^ $(LIBS)

# server256: server.c $(SRC256)
# 	$(CC) $(CFLAGS)   -DSEC_LEVEL=1 -DPRIMES=$(PRIMES_VAL) -o $@ $^ $(LIBS)

# server512: server.c $(SRC512)
# 	$(CC) $(CFLAGS)   -DSEC_LEVEL=1 -DPRIMES=$(PRIMES_VAL) -o $@ $^ $(LIBS)
