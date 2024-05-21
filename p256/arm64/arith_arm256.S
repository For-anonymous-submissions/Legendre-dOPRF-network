// Format function and variable names for Mac OS X
#if defined(__APPLE__)
    #define fmt(f)    _##f
#else
    #define fmt(f)    f
#endif


#define ORIGINAL 0
#define ALT      1


.text
.align 4



#if (PRIMES == ORIGINAL)

// FIELD CONSTANTS

; static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF}; // Field order p
; static const digit_t Mont_one[WORDS_FIELD]  = {0x0000000000000026, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
; static const digit_t R2[WORDS_FIELD]        = {0x00000000000005A4, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
; static const digit_t iR[WORDS_FIELD]        = {0x435E50D79435E50A, 0x5E50D79435E50D79, 0x50D79435E50D7943, 0x179435E50D79435E}; // iR =  R^(-1) (mod p)
; static const digit_t pp[WORDS_FIELD]        = {0x86BCA1AF286BCA1B, 0xBCA1AF286BCA1AF2, 0xA1AF286BCA1AF286, 0x2F286BCA1AF286BC}; // pp = -p^(-1) mod R
; static const digit_t ip[WORDS_FIELD]        = {0x79435E50D79435E5, 0x435E50D79435E50D, 0x5E50D79435E50D79, 0xD0D79435E50D7943}; // ip =  p^(-1) mod R
; static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 0
; static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 1

// Field characterstics
p256:
.quad   0xFFFFFFFFFFFFFFED
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x7FFFFFFFFFFFFFFF

// Montgomery one = R = 2^128 mod p = 2^128 - p ( = -p % R) (p + Mont_one = R)
Rmp:
.quad   0x0000000000000026

// R squared mod p
R2mp:
.quad   0x00000000000005A4

// Inverse of R mod p 
iRmp:
.quad   0x435E50D79435E50A
.quad   0x5E50D79435E50D79
.quad   0x50D79435E50D7943
.quad   0x179435E50D79435E

// Inverse of -p mod R
impmR:
.quad   0x86BCA1AF286BCA1B
.quad   0xBCA1AF286BCA1AF2
.quad   0xA1AF286BCA1AF286
.quad   0x2F286BCA1AF286BC

// Inverse of p mod r
ipmR:
.quad   0x79435E50D79435E5

// Zero // Not actually used
Zero:
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000

// One
One:
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Modular reduction
;  Input: a[x0] 1 word < R
;  Output: c[x1] 1 words < p
;  Operation: c [x1] =  a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_red_asm)
fmt(f_red_asm):
    ldp     x2, x3, [x0, #00]
    ldp     x4, x5, [x0, #16]


    mov     x6, #0xFFFFFFFFFFFFFFED
    mov     x7, #0xFFFFFFFFFFFFFFFF
    mov     x8, #0x7FFFFFFFFFFFFFFF


    mov     x9, #19
    lsr     x11, x5, #63
    mul     x11, x11, x9

    and     x5, x5, x8


    adds    x2, x2, x11
    adcs    x3, x3, xzr
    adcs    x4, x4, xzr
    adc     x5, x5, xzr

    subs    x2, x2, x6
    sbcs    x3, x3, x7
    sbcs    x4, x4, x7
    sbcs    x5, x5, x8

    sbc     x9, xzr, xzr

    and     x6, x6, x9
    and     x8, x8, x9

    adds    x2, x2, x6
    adcs    x3, x3, x9
    adcs    x4, x4, x9
    adc     x5, x5, x8

    stp     x2, x3, [x0, #00]
    stp     x4, x5, [x0, #16]
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field addition
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] + b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_add_asm)
fmt(f_add_asm):
    ldp     x3, x4,  [x0, #00]
    ldp     x5, x6,  [x0, #16]
    ldp     x7, x8,  [x1, #00]
    ldp     x9, x10, [x1, #16]

    mov     x11, #0xFFFFFFFFFFFFFFED
    mov     x12, #0xFFFFFFFFFFFFFFFF
    mov     x13, #0x7FFFFFFFFFFFFFFF


    adds    x3, x3, x7
    adcs    x4, x4, x8
    adcs    x5, x5, x9
    adc     x6, x6, x10

    subs    x3, x3, x11
    sbcs    x4, x4, x12
    sbcs    x5, x5, x12
    sbcs    x6, x6, x13

    sbc     x9, xzr, xzr

    and     x11, x11, x9
    and     x13, x13, x9

    adds    x3, x3, x11
    adcs    x4, x4, x9
    adcs    x5, x5, x9
    adc     x6, x6, x13

    stp     x3, x4, [x2, #00]
    stp     x5, x6, [x2, #16]
    ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field negation
;  Input: a[x0] 1 word < p
;  Output: c[x1] 1 words
;  Operation: c [x1] =  -a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_neg_asm)
fmt(f_neg_asm):
    ldp     x2, x3, [x0, #00]
    ldp     x4, x5, [x0, #16]

    mov     x6, #0xFFFFFFFFFFFFFFED
    mov     x7, #0xFFFFFFFFFFFFFFFF
    mov     x8, #0x7FFFFFFFFFFFFFFF

    subs    x2, x6, x2
    sbcs    x3, x7, x3
    sbcs    x4, x7, x4
    sbc     x5, x8, x5

    subs    x2, x2, x6
    sbcs    x3, x3, x7
    sbcs    x4, x4, x7
    sbcs    x5, x5, x8

    sbc     x9, xzr, xzr

    and     x6, x6, x9
    and     x8, x8, x9

    adds    x2, x2, x6
    adcs    x3, x3, x9
    adcs    x4, x4, x9
    adc     x5, x5, x8

    stp     x2, x3, [x1, #00]
    stp     x4, x5, [x1, #16]
    ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field subtraction
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] - b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_sub_asm)
fmt(f_sub_asm):
    ldp     x3, x4,  [x0, #00]
    ldp     x5, x6,  [x0, #16]
    ldp     x7, x8,  [x1, #00]
    ldp     x9, x10, [x1, #16]

    mov     x11, #0xFFFFFFFFFFFFFFED
    mov     x13, #0x7FFFFFFFFFFFFFFF

    subs    x3, x3, x7
    sbcs    x4, x4, x8
    sbcs    x5, x5, x9
    sbcs    x6, x6, x10

    sbc     x12, xzr, xzr
    and     x11, x11, x12
    and     x13, x13, x12

    adds    x3, x3, x11
    adcs    x4, x4, x12
    adcs    x5, x5, x12
    adc     x6, x6, x13

    stp     x3, x4, [x2, #00]
    stp     x5, x6, [x2, #16]
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Multiprecision multiplication
;  Input: a[x0] 4 words; b[x1] 4 words
;  Output: c[x2] 4 words
;  Operation: c [x2] = a [x0] * b [x1]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
;; THIS CODE IS INCORRECT AND IRRELEVANT
;; LEFT FOR COMPLETENESS
.global fmt(mp_mul_asm)
fmt(mp_mul_asm):
    ldp     x3, x4, [x1]
    ldp     x5, x6, [x0]

    mul     x7, x5, x3
    umulh   x10, x6, x4
    mul     x8, x6, x3
    umulh   x9, x5, x4

    umulh   x11, x5, x3
    mul     x12, x6, x4

    adds    x8, x8, x11
    adcs    x9, x9, x12
    adc     x10, x10, xzr

    mul     x11, x5, x4
    umulh   x12, x6, x3

    adds    x8, x8, x11
    adcs    x9, x9, x12
    adc     x10, x10, xzr

    stp     x7, x8, [x2,#00]
    stp     x9, x10, [x2,#16]
    ret



.exit:
    mov x2, x11


.macro interleave_step, K, \
                        C0, C1, C2, C3, C4, \
                        B0, B1, B2, B3, \
                        T0, T1, T2, T3, \
                        AI, Ap, iP\

    ldr     \AI, [\Ap, #8*\K]

    mul     \T0, \AI, \B0
    umulh   \T1, \AI, \B0
    mul     \T2, \AI, \B2
    umulh   \T3, \AI, \B2

    adds    \C0, \C0, \T0
    adcs    \C1, \C1, \T1
    adcs    \C2, \C2, \T2
    adcs    \C3, \C3, \T3
    adc     \C4, xzr, xzr


    mul     \T0, \AI, \B1
    umulh   \T1, \AI, \B1
    mul     \T2, \AI, \B3
    umulh   \T3, \AI, \B3

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adc     \C4, \C4, \T3

    // q = (c0 * ipmr) % r
    mul     \T0, \C0, \iP
    mov     \T3, #19

    // p = 2^255 - 19
    // q * p = (2^255)q - 19q
    mul     \T1, \T0, \T3
    umulh   \T2, \T0, \T3
    
    lsr     \T3, \T0, #1

    // C += 2^255 * q
    adds    \C3, \C3, \T0, lsl #63
    adcs    \C4, \C4, \T3
    adc     \T0, xzr, xzr
    ; cbnz    \T0, .exit

    // C -= 19 * q
    subs    \C0, \C0, \T1
    sbcs    \C1, \C1, \T2
    sbcs    \C2, \C2, xzr
    sbcs    \C3, \C3, xzr
    sbc     \C4, \C4, xzr

.endm


//***********************************************************************
//  Field multiplication
//  Input: a[x0] 1 word < p; b[x1] 1 word < p
//  Output: c[x2] 1 word < p
//  Operation: c [x2] = a [x0] * b [x1] mod p
//*********************************************************************** 
.global fmt(f_mul_asm)
fmt(f_mul_asm):
    ldr     x3,      [x0, #00]
    ldp     x4, x5,  [x1, #00]
    ldp     x6, x7,  [x1, #16]

    ldr     x8, impmR

    // C = a0 * B
    mul     x9,  x3, x4
    umulh   x10, x3, x4
    mul     x11, x3, x6
    umulh   x12, x3, x6

    mul     x14, x3, x5
    umulh   x15, x3, x5
    mul     x16, x3, x7
    umulh   x13, x3, x7

    adds    x10, x10, x14
    adcs    x11, x11, x15
    adcs    x12, x12, x16
    adc     x13, x13, xzr

    // q = (c0 * ipmr) % r
    mul     x17, x9, x8

    // p = 2^255 - 19
    // q * p = (2^255)q - 19q
    mov     x16, #19

    mul     x14, x17, x16
    umulh   x15, x17, x16
    
    lsr     x16, x17, #1

    // C += 2^255 * q (= 2^192 (q << 63) | 2^256 (q >> 1))
    adds    x12, x12, x17, lsl #63
    adcs    x13, x13, x16
    adc     x3, xzr, xzr
    ; cbnz    x3, .exit


    // C -= 19 * q
    subs    x9,  x9,  x14
    sbcs    x10, x10, x15
    sbcs    x11, x11, xzr
    sbcs    x12, x12, xzr
    sbc     x13, x13, xzr

    interleave_step 1, \
                    x10, x11, x12, x13, x9, \
                    x4, x5, x6, x7, \
                    x14, x15, x16, x17, \
                    x3, x0, x8


    interleave_step 2, \
                    x11, x12, x13, x9, x10, \
                    x4, x5, x6, x7, \
                    x14, x15, x16, x17, \
                    x3, x0, x8


    interleave_step 3, \
                    x12, x13, x9, x10, x11, \
                    x4, x5, x6, x7, \
                    x14, x15, x16, x17, \
                    x3, x0, x8


    mov     x3, #0xFFFFFFFFFFFFFFED
    mov     x4, #0xFFFFFFFFFFFFFFFF
    mov     x5, #0x7FFFFFFFFFFFFFFF

    subs    x13, x13, x3
    sbcs    x9,  x9,  x4
    sbcs    x10, x10, x4
    sbcs    x11, x11, x5

    sbc     x4, xzr, xzr
    and     x3, x3, x4
    and     x5, x5, x4

    adds    x13, x13, x3
    adcs    x9,  x9,  x4
    adcs    x10, x10, x4
    adc     x11, x11, x5

    stp     x13, x9,    [x2, #00]
    stp     x10, x11,   [x2, #16]

    ret









#elif (PRIMES == ALT)




// FIELD CONSTANTS

    ; static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFF, 0x0000000000000000, 0xFFFFFFFF00000001}; // Field order p
    ; static const digit_t Mont_one[WORDS_FIELD]  = {0x0000000000000001, 0xFFFFFFFF00000000, 0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFE}; // R  =  2^{NBITS_PRIME} (mod p)
    ; static const digit_t R2[WORDS_FIELD]        = {0x0000000000000003, 0xFFFFFFFBFFFFFFFF, 0xFFFFFFFFFFFFFFFE, 0x00000004FFFFFFFD}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
    ; static const digit_t iR[WORDS_FIELD]        = {0x0000000300000000, 0x00000001FFFFFFFE, 0xFFFFFFFD00000002, 0xFFFFFFFE00000003}; // iR =  R^(-1) (mod p)
    ; static const digit_t pp[WORDS_FIELD]        = {0x0000000000000001, 0x0000000100000000, 0x0000000000000000, 0xFFFFFFFF00000002}; // pp = -p^(-1) mod R
    ; static const digit_t ip[WORDS_FIELD]        = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFEFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x00000000FFFFFFFD}; // ip =  p^(-1) mod R
    ; static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 0
    ; static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 1

// Field characterstics
p256:
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x00000000FFFFFFFF
.quad   0x0000000000000000
.quad   0xFFFFFFFF00000001

// Montgomery one = R % p
Rmp:
.quad   0x0000000000000001
.quad   0xFFFFFFFF00000000
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x00000000FFFFFFFE

// R squared mod p
R2mp:
.quad   0x0000000000000003
.quad   0xFFFFFFFBFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFE
.quad   0x00000004FFFFFFFD

// Inverse of R mod p 
iRmp:
.quad   0x0000000300000000
.quad   0x00000001FFFFFFFE
.quad   0xFFFFFFFD00000002
.quad   0xFFFFFFFE00000003

// Inverse of -p mod R
impmR:
.quad   0x0000000000000001
.quad   0x0000000100000000
.quad   0x0000000000000000
.quad   0xFFFFFFFF00000002

// Inverse of p mod r
ipmR:
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFEFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x00000000FFFFFFFD

// Zero // Not actually used
Zero:
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000

// One
One:
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Modular reduction
;  Input: a[x0] 4 words < R
;  Output: c[x1] 4 words < p
;  Operation: c [x1] =  a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_red_asm)
fmt(f_red_asm):
    ldp     x2, x3, [x0, #00]
    ldp     x4, x5, [x0, #16]

    mov     x6, #0xFFFFFFFFFFFFFFFF
    mov     x7, #0x00000000FFFFFFFF
    mov     x9, #0xFFFFFFFF00000001

    subs    x2, x2, x6
    sbcs    x3, x3, x7
    sbcs    x4, x4, xzr
    sbcs    x5, x5, x9

    sbc     x10, xzr, xzr
    
    and     x6, x6, x10
    and     x7, x7, x10
    and     x9, x9, x10

    adds    x2, x2, x6
    adcs    x3, x3, x7
    adcs    x4, x4, xzr
    adc     x5, x5, x9

    stp     x2, x3, [x0, #00]
    stp     x4, x5, [x0, #16]
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field addition
;  Input: a[x0] 4 words < p; b[x1] 4 words < p
;  Output: c[x2] 4 words
;  Operation: c [x2] = a [x0] + b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_add_asm)
fmt(f_add_asm):
    ldp     x3, x4,  [x0, #00]
    ldp     x5, x6,  [x0, #16]
    ldp     x7, x8,  [x1, #00]
    ldp     x9, x10, [x1, #16]

    mov     x11, #0xFFFFFFFFFFFFFFFF
    mov     x12, #0x00000000FFFFFFFF
    mov     x13, #0xFFFFFFFF00000001


    adds    x3, x3, x7
    adcs    x4, x4, x8
    adcs    x5, x5, x9
    adcs    x6, x6, x10

    adc     x14, xzr, xzr
    sub     x14, xzr, x14

    and     x16, x12, x14
    and     x17, x13, x14

    subs    x3, x3, x14
    sbcs    x4, x4, x16
    sbcs    x5, x5, xzr
    sbc     x6, x6, x17

    subs    x3, x3, x11
    sbcs    x4, x4, x12
    sbcs    x5, x5, xzr
    sbcs    x6, x6, x13

    sbc     x14, xzr, xzr

    and     x12, x12, x14
    and     x13, x13, x14

    adds    x3, x3, x14
    adcs    x4, x4, x12
    adcs    x5, x5, xzr
    adc     x6, x6, x13

    stp     x3, x4, [x2, #00]
    stp     x5, x6, [x2, #16]
    ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field negation
;  Input: a[x0] 4 words < p
;  Output: c[x1] 4 words
;  Operation: c [x1] =  -a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_neg_asm)
fmt(f_neg_asm):
    ldp     x2, x3, [x0, #00]
    ldp     x4, x5, [x0, #16]

    mov     x6, #0xFFFFFFFFFFFFFFFF
    mov     x7, #0x00000000FFFFFFFF
    mov     x8, #0xFFFFFFFF00000001

    subs    x2, x6,  x2
    sbcs    x3, x7,  x3
    sbcs    x4, xzr, x4
    sbc     x5, x8,  x5

    subs    x2, x2,  x6
    sbcs    x3, x3,  x7
    sbcs    x4, x4,  xzr
    sbcs    x5, x5,  x8

    sbc     x9, xzr, xzr

    and     x7, x7,  x9
    and     x8, x8,  x9

    adds    x2, x2,  x9
    adcs    x3, x3,  x7
    adcs    x4, x4,  xzr
    adc     x5, x5,  x8

    stp     x2, x3,  [x1, #00]
    stp     x4, x5,  [x1, #16]
    ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field subtraction
;  Input: a[x0] 4 words < p; b[x1] 4 words < p
;  Output: c[x2] 4 words
;  Operation: c [x2] = a [x0] - b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_sub_asm)
fmt(f_sub_asm):
    ldp     x3, x4,  [x0, #00]
    ldp     x5, x6,  [x0, #16]
    ldp     x7, x8,  [x1, #00]
    ldp     x9, x10, [x1, #16]

    mov     x11, #0x00000000FFFFFFFF
    mov     x13, #0xFFFFFFFF00000001

    subs    x3, x3, x7
    sbcs    x4, x4, x8
    sbcs    x5, x5, x9
    sbcs    x6, x6, x10

    sbc     x12, xzr, xzr

    and     x11, x11, x12
    and     x13, x13, x12

    adds    x3, x3, x12
    adcs    x4, x4, x11
    adcs    x5, x5, xzr
    adc     x6, x6, x13

    stp     x3, x4, [x2, #00]
    stp     x5, x6, [x2, #16]
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Multiprecision multiplication
;  Input: a[x0] 4 words; b[x1] 4 words
;  Output: c[x2] 8 words
;  Operation: c [x2] = a [x0] * b [x1]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mp_mul_asm)
fmt(mp_mul_asm):
    ldp     x3, x4, [x1]
    ldp     x5, x6, [x0]

    mul     x7, x5, x3
    umulh   x10, x6, x4
    mul     x8, x6, x3
    umulh   x9, x5, x4

    umulh   x11, x5, x3
    mul     x12, x6, x4

    adds    x8, x8, x11
    adcs    x9, x9, x12
    adc     x10, x10, xzr

    mul     x11, x5, x4
    umulh   x12, x6, x3

    adds    x8, x8, x11
    adcs    x9, x9, x12
    adc     x10, x10, xzr

    stp     x7, x8, [x2,#00]
    stp     x9, x10, [x2,#16]
    ret



.exit:
    mov x2, x11


.macro interleave_step, C0, C1, C2, C3, C4, C5\
                        B0, B1, B2, B3, \
                        T0, T1, T2, T3, \
                        AI

    mul     \T0, \AI, \B0
    umulh   \T1, \AI, \B0
    mul     \T2, \AI, \B2
    umulh   \T3, \AI, \B2

    adds    \C0, \C0, \T0
    adcs    \C1, \C1, \T1
    adcs    \C2, \C2, \T2
    adcs    \C3, \C3, \T3
    adc     \C4, \C4, xzr


    mul     \T0, \AI, \B1
    umulh   \T1, \AI, \B1
    mul     \T2, \AI, \B3
    umulh   \T3, \AI, \B3

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adcs    \C4, \C4, \T3
    adc     \C5, xzr, xzr

    // p =  2^256 - 2^224 + 2^192 + 2^96 - 1
    // p =  2^256         + 2^192 + 2^96
    //            - 2^224                - 1            
    // C  = C + \C0 * p

    lsr     \T1, \C0, #32

    adds    \C1, \C1, \C0, lsl #32
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \C0
    adcs    \C4, \C4, \C0
    adc     \C5, \C5, xzr

    subs    \C3, \C3, \C0, lsl #32
    sbcs    \C4, \C4, \T1
    sbc     \C5, \C5, xzr

    ; cbnz    \C5, .exit






.endm


//***********************************************************************
//  Field multiplication
//  Input: a[x0] 1 word < p; b[x1] 1 word < p
//  Output: c[x2] 1 word < p
//  Operation: c [x2] = a [x0] * b [x1] mod p
//*********************************************************************** 
.global fmt(f_mul_asm)
fmt(f_mul_asm):
    ldr     x3,      [x0, #00]
    ldp     x4, x5,  [x1, #00]
    ldp     x6, x7,  [x1, #16]

    // C = a0 * B
    mul     x9,  x3, x4
    umulh   x10, x3, x4
    mul     x11, x3, x6
    umulh   x12, x3, x6

    mul     x14, x3, x5
    umulh   x15, x3, x5
    mul     x16, x3, x7
    umulh   x13, x3, x7

    adds    x10, x10, x14
    adcs    x11, x11, x15
    adcs    x12, x12, x16
    adc     x13, x13, xzr

    // p =  2^256 - 2^224 + 2^192 + 2^96 - 1
    // p =  2^256         + 2^192 + 2^96
    //            - 2^224                - 1            
    // C  = C + x9 * p

    lsr     x15, x9, #32

    adds    x10, x10, x9, lsl #32
    adcs    x11, x11, x15
    adcs    x12, x12, x9
    adcs    x13, x13, x9
    adc     x14, xzr, xzr

    subs    x12, x12, x9, lsl #32
    sbcs    x13, x13, x15
    sbc     x14, x14, xzr

    ; cbnz    x14, .exit



    ldr     x3,  [x0, #8]

    interleave_step x10, x11, x12, x13, x14, x9, \
                    x4, x5, x6, x7, \
                    x8, x15, x16, x17, \
                    x3

    ldr     x3,  [x0, #16]

    interleave_step x11, x12, x13, x14, x9, x10, \
                    x4, x5, x6, x7, \
                    x8, x15, x16, x17, \
                    x3

    ldr     x3,  [x0, #24]

    interleave_step x12, x13, x14, x9, x10, x11, \
                    x4, x5, x6, x7, \
                    x8, x15, x16, x17, \
                    x3


    mov     x3, #0xFFFFFFFFFFFFFFFF
    mov     x4, #0x00000000FFFFFFFF
    mov     x6, #0xFFFFFFFF00000001

    sub     x11, xzr, x11

    and     x7, x4, x11
    and     x8, x6, x11

    subs    x13, x13, x11
    sbcs    x14, x14, x7
    sbcs    x9, x9, xzr
    sbc     x10, x10, x8


    subs    x13, x13, x3
    sbcs    x14, x14, x4
    sbcs    x9,  x9,  xzr
    sbcs    x10, x10, x6

    sbc     x11, xzr, xzr

    and     x7, x4, x11
    and     x8, x6, x11

    adds    x13, x13, x11
    adcs    x14, x14, x7
    adcs    x9,  x9,  xzr
    adc     x10, x10, x8

    stp     x13, x14,   [x2, #00]
    stp     x9,  x10,   [x2, #16]

    ret




#endif





