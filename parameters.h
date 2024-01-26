#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include <stdio.h>

#define ALT_128 0


#if (SEC_LEVEL == 0)
    #define NBITS_FIELD     64
    #define NBYTES_FIELD    8
    #define WORDS_FIELD     1
#elif (SEC_LEVEL == 1)
    #define NBITS_FIELD     128
    #define NBYTES_FIELD    16
    #define WORDS_FIELD     2
#elif (SEC_LEVEL == 2)
    #define NBITS_FIELD     192
    #define NBYTES_FIELD    24
    #define WORDS_FIELD     3
#elif (SEC_LEVEL == 3)
    #define NBITS_FIELD     256
    #define NBYTES_FIELD    32
    #define WORDS_FIELD     4
#elif (SEC_LEVEL == 4)
    #define NBITS_FIELD     512
    #define NBYTES_FIELD    64
    #define WORDS_FIELD     8
#else
    #error -- "Unsupported SEC_LEVEL"
#endif


// Architecture parameters
#define RADIX           64
#define LOG2RADIX       6  
typedef uint64_t        digit_t;        // Unsigned 64-bit digit
typedef uint32_t        hdigit_t;       // Unsigned 32-bit digit
typedef digit_t         f_elm_t[WORDS_FIELD];
typedef digit_t         sec_key[WORDS_FIELD];
typedef unsigned uint128_t __attribute__((mode(TI)));




#ifndef FIELD_PARAMS_H
#define FIELD_PARAMS_H
#if (NBITS_FIELD == 64)
    static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFFC5}; // Field order p
    static const digit_t Mont_one[WORDS_FIELD]  = {0x000000000000003B}; // R  =  2^{NBITS_PRIME} (mod p)
    static const digit_t R2[WORDS_FIELD]        = {0x0000000000000D99}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
    static const digit_t iR[WORDS_FIELD]        = {0xCBEEA4E1A08AD8C4}; // iR =  R^(-1) (mod p)
    static const digit_t pp[WORDS_FIELD]        = {0xCBEEA4E1A08AD8F3}; // pp = -p^(-1) mod R
    static const digit_t ip[WORDS_FIELD]        = {0x34115B1E5F75270D}; // ip =  p^(-1) mod R    
    static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000}; // Zero = 0
    static const digit_t One[WORDS_FIELD]       = {0x0000000000000001}; // One = 1
#elif (NBITS_FIELD == 128)
    #if (ALT_128)
    static const digit_t p[WORDS_FIELD]         = {0x000000000000001D, 0x8000000000000000}; // Field order p
    static const digit_t Mont_one[WORDS_FIELD]  = {0xFFFFFFFFFFFFFFE3, 0x7FFFFFFFFFFFFFFF}; // R  =  2^{NBITS_PRIME} (mod p)
    static const digit_t R2[WORDS_FIELD]        = {0x0000000000000D24, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
    static const digit_t iR[WORDS_FIELD]        = {0xE58469EE58469EFB, 0x5EE58469EE58469E}; // iR =  R^(-1) (mod p)
    static const digit_t pp[WORDS_FIELD]        = {0xCB08D3DCB08D3DCB, 0xBDCB08D3DCB08D3D}; // pp = -p^(-1) mod R
    static const digit_t ip[WORDS_FIELD]        = {0x34F72C234F72C235, 0x4234F72C234F72C2}; // ip =  p^(-1) mod R
    static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000}; // 0
    static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000}; // 1
    #else
    static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFF61, 0xFFFFFFFFFFFFFFFF}; // Field order p
    static const digit_t Mont_one[WORDS_FIELD]  = {0x000000000000009F, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
    static const digit_t R2[WORDS_FIELD]        = {0x00000000000062C1, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
    static const digit_t iR[WORDS_FIELD]        = {0xB5EFE63D2EB11AF1, 0xB11B5EFE63D2EB11}; // iR =  R^(-1) (mod p)
    static const digit_t pp[WORDS_FIELD]        = {0xB5EFE63D2EB11B5F, 0xB11B5EFE63D2EB11}; // pp = -p^(-1) mod R
    static const digit_t ip[WORDS_FIELD]        = {0x4A1019C2D14EE4A1, 0x4EE4A1019C2D14EE}; // ip =  p^(-1) mod R
    static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000}; // 0
    static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000}; // 1
    #endif
#elif (NBITS_FIELD == 192)
    static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFF13, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF}; // Field order p
    static const digit_t Mont_one[WORDS_FIELD]  = {0x00000000000000ED, 0x0000000000000000, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
    static const digit_t R2[WORDS_FIELD]        = {0x000000000000DB69, 0x0000000000000000, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
    static const digit_t iR[WORDS_FIELD]        = {0xDE83C7D4CB125C9E, 0xF1F532C497393FBA, 0x4CB125CE4FEEB7A0}; // iR =  R^(-1) (mod p)
    static const digit_t pp[WORDS_FIELD]        = {0xDE83C7D4CB125CE5, 0xF1F532C497393FBA, 0x4CB125CE4FEEB7A0}; // pp = -p^(-1) mod R
    static const digit_t ip[WORDS_FIELD]        = {0x217C382B34EDA31B, 0x0E0ACD3B68C6C045, 0xB34EDA31B011485F}; // ip =  p^(-1) mod R
    static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 0
    static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000}; // 1
#elif (NBITS_FIELD == 256)
    static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF}; // Field order p
    static const digit_t Mont_one[WORDS_FIELD]  = {0x0000000000000026, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
    static const digit_t R2[WORDS_FIELD]        = {0x00000000000005A4, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
    static const digit_t iR[WORDS_FIELD]        = {0x435E50D79435E50A, 0x5E50D79435E50D79, 0x50D79435E50D7943, 0x179435E50D79435E}; // iR =  R^(-1) (mod p)
    static const digit_t pp[WORDS_FIELD]        = {0x86BCA1AF286BCA1B, 0xBCA1AF286BCA1AF2, 0xA1AF286BCA1AF286, 0x2F286BCA1AF286BC}; // pp = -p^(-1) mod R
    static const digit_t ip[WORDS_FIELD]        = {0x79435E50D79435E5, 0x435E50D79435E50D, 0x5E50D79435E50D79, 0xD0D79435E50D7943}; // ip =  p^(-1) mod R
    static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 0
    static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 1
#elif (NBITS_FIELD == 512)
    static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFDC7, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF}; // Field order p
    static const digit_t Mont_one[WORDS_FIELD]  = {0x0000000000000239, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
    static const digit_t R2[WORDS_FIELD]        = {0x000000000004F0B1, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
    static const digit_t iR[WORDS_FIELD]        = {0x58A1F7E6CE0F4BD8, 0xCB5C3B636E3A7D13, 0x82064C7C2CFDC01C, 0xF127247160BB29D7, 0x6CE0F4C08FF8CD28, 0x36E3A7D1358A1F7E, 0xC2CFDC01CCB5C3B6, 0x160BB29D782064C7}; // iR =  R^(-1) (mod p)
    static const digit_t pp[WORDS_FIELD]        = {0x58A1F7E6CE0F4C09, 0xCB5C3B636E3A7D13, 0x82064C7C2CFDC01C, 0xF127247160BB29D7, 0x6CE0F4C08FF8CD28, 0x36E3A7D1358A1F7E, 0xC2CFDC01CCB5C3B6, 0x160BB29D782064C7}; // pp = -p^(-1) mod R
    static const digit_t ip[WORDS_FIELD]        = {0xA75E081931F0B3F7, 0x34A3C49C91C582EC, 0x7DF9B383D3023FE3, 0x0ED8DB8E9F44D628, 0x931F0B3F700732D7, 0xC91C582ECA75E081, 0x3D3023FE334A3C49, 0xE9F44D6287DF9B38}; // ip =  p^(-1) mod R
    static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 0
    static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 1
#endif
#endif /* FIELD_PARAMS_H */


#endif