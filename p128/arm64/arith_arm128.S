// Format function and variable names for Mac OS X
#if defined(__APPLE__)
    #define fmt(f)    _##f
#else
    #define fmt(f)    f
#endif

.text
.align 2

#define ORIGINAL 0
#define ALT      1







#if (PRIMES == ORIGINAL)


; static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF}; // Field order p
; static const digit_t Mont_one[WORDS_FIELD]  = {0x0000000000000002, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
; static const digit_t R2[WORDS_FIELD]        = {0x0000000000000004, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
; static const digit_t iR[WORDS_FIELD]        = {0x0000000000000000, 0x4000000000000000}; // iR =  R^(-1) (mod p)
; static const digit_t pp[WORDS_FIELD]        = {0x0000000000000001, 0x8000000000000000}; // pp = -p^(-1) mod R
; static const digit_t ip[WORDS_FIELD]        = {0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF}; // ip =  p^(-1) mod R
; static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000}; // 0
; static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000}; // 1

// Field characterstics
p128:
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x7FFFFFFFFFFFFFFF

// Montgomery one = R = 2^128 mod p = 2^128 - p ( = -p % R) (p + Mont_one = R)
Rmp:
.quad   0x0000000000000002
.quad   0x0000000000000000

// R squared mod p
R2mp:
.quad   0x0000000000000004
.quad   0x0000000000000000

// Inverse of R mod p 
iRmp:
.quad   0x0000000000000000
.quad   0x4000000000000000

// Inverse of -p mod R
impmR:
.quad   0x0000000000000001
.quad   0x8000000000000000

// Inverse of p mod R
ipmR:
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x7FFFFFFFFFFFFFFF

// Zero // Not actually used
Zero:
.quad   0x0000000000000000
.quad   0x0000000000000000

// One
One:
.quad   0x0000000000000001
.quad   0x0000000000000000



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Modular reduction
;  Input: a[x0] 1 word < 2*p
;  Output: c[x1] 1 words < p
;  Operation: c [x1] =  a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.global fmt(f_red_asm)
fmt(f_red_asm):

    ldp     x2, x3, [x0]

    lsr     x4, x3, #63
    and     x3, x3, #0x7FFFFFFFFFFFFFFF

    adds    x2, x2, x4
    adc     x3, x3, xzr

    adds    x2, x2, #1
    adc     x3, x3, xzr

    lsr     x4, x3, #63
    and     x3, x3, #0x7FFFFFFFFFFFFFFF

    adds    x2, x2, x4
    adc     x3, x3, xzr

    subs    x2, x2, #1
    sbc     x3, x3, xzr

    stp     x2, x3, [x0]
    ret


    ; ldp     x2, x3, [x0]
    ; mov     x5, 0x7FFFFFFFFFFFFFFF

    ; lsr     x6, x3, #63
    ; and     x3, x3, x5

    ; adds    x2, x2, x6
    ; adc     x3, x3, xzr

    ; subs    x2, x2, 0xFFFFFFFFFFFFFFFF
    ; sbcs    x3, x3, x5

    ; sbc     x4, xzr, xzr

    ; and     x5, x5, x4

    ; adds    x2, x2, x4
    ; adc     x3, x3, x5

    ; stp     x2, x3, [x0]
    ; ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field addition
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] + b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.global fmt(f_add_asm)
fmt(f_add_asm):
    ldp     x3, x4, [x0]
    ldp     x7, x8, [x1]

    mov     x6, 0x7FFFFFFFFFFFFFFF

    adds    x3, x3, x7
    adc     x4, x4, x8

    subs    x3, x3, 0xFFFFFFFFFFFFFFFF
    sbcs    x4, x4, x6

    sbc     x5, xzr, xzr

    and     x6, x6, x5

    adds    x3, x3, x5
    adc     x4, x4, x6

    stp     x3, x4, [x2]
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field negation
;  Input: a[x0] 1 word < p
;  Output: c[x1] 1 words
;  Operation: c [x1] =  -a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_neg_asm)
fmt(f_neg_asm):

    ldp     x2, x3, [x0]
    mov     x4, 0xFFFFFFFFFFFFFFFF
    mov     x5, 0x7FFFFFFFFFFFFFFF

    subs    x2, x4, x2
    sbc     x3, x5, x3

    subs    x2, x2, x4
    sbcs    x3, x3, x5

    sbc     x4, xzr, xzr
    and     x5, x5, x4

    adds    x2, x2, x4
    adc     x3, x3, x5

    stp     x2, x3, [x1]
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field subtraction
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] - b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_sub_asm)
fmt(f_sub_asm):
    ldp     x3, x4, [x0]
    ldp     x5, x6, [x1]

    mov     x8, 0x7FFFFFFFFFFFFFFF

    subs    x3, x3, x5
    sbcs    x4, x4, x6

    sbc     x7, xzr, xzr
    and     x8, x8, x7

    adds    x3, x3, x7
    adc     x4, x4, x8

    stp     x3, x4, [x2]
    ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Multiprecision multiplication
;  Input: a[x0] 1 word; b[x1] 1 word
;  Output: c[x2] 2 words
;  Operation: c [x2] = a [x0] * b [x1]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mp_mul_asm)
fmt(mp_mul_asm):
    ldp     x3, x4, [x1]
    ldp     x5, x6, [x0]

    mul     x7, x3, x5
    umulh   x8, x3, x5

    mul     x9, x4, x6
    umulh   x10, x4, x6

    mul     x11, x4, x5
    umulh   x12, x4, x5

    adds    x8, x8, x11
    adcs    x9, x9, x12
    adc     x10, x10, xzr

    mul     x11, x3, x6
    umulh   x12, x3, x6

    adds    x8, x8, x11
    adcs    x9, x9, x12
    adc     x10, x10, xzr

    stp     x7, x8, [x2,#00]
    stp     x9, x10, [x2,#16]
    ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Montgomery reduction
;  Input: a[x0] 2 words < p*R
;  Output: c[x1] 1 word < p
;  Operation: c[x1] = a [x0] * (R^(-1)) mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mont_redc_asm)
fmt(mont_redc_asm):

    ldp     x2, x3, [x0,#00]
    ldp     x4, x5, [x0,#16]

    mov     x8, 0xFFFFFFFFFFFFFFFF
    mov     x9, 0x7FFFFFFFFFFFFFFF

    mul     x10, x2, x8
    umulh   x11, x2, x8
    mul     x13, x3, x8
    mul     x12, x2, x9

    add     x11, x11, x12
    add     x11, x11, x13

    mul     x12, x10, x8
    umulh   x13, x10, x8
    mul     x14, x11, x9
    umulh   x15, x11, x9

    mul     x16, x10, x9
    umulh   x17, x10, x9

    adds    x13, x13, x16
    adcs    x14, x14, x17
    adc     x15, x15, xzr

    mul     x16, x11, x8
    umulh   x17, x11, x8

    adds    x13, x13, x16
    adcs    x14, x14, x17
    adc     x15, x15, xzr

    subs    x4, x4, x14
    sbcs    x5, x5, x15

    sbc     x8, xzr, xzr
    and     x9, x9, x8

    adds    x4, x4, x8
    adc     x5, x5, x9

    stp     x4, x5, [x1]
    ret





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field multiplication
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 word < p
;  Operation: c [x2] = a [x0] * b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.global fmt(f_mul_asm)
fmt(f_mul_asm):
    ldp     x3, x4, [x0]
    ldp     x5, x6, [x1]

    mov     x15, 0x7FFFFFFFFFFFFFFF

    mul     x7, x3, x5
    umulh   x8, x3, x5
    mul     x10, x3, x6
    umulh   x9, x3, x6

    lsr     x16, x7, #1

    adds    x8, x8, x10
    adc     x9, x9, xzr

    adds    x8, x8, x7, lsl #63
    adc     x9, x9, x16

    mul     x10, x4, x5
    umulh   x11, x4, x5
    mul     x13, x4, x6
    umulh   x12, x4, x6

    adds    x10, x10, x8
    adcs    x11, x11, x9
    adc     x12, x12, xzr

    lsr     x16, x10, #1

    adds    x11, x11, x13
    adc     x12, x12, xzr

    adds    x11, x11, x10, lsl #63
    adc     x12, x12, x16


    subs    x11, x11, 0xFFFFFFFFFFFFFFFF
    sbcs    x12, x12, x15

    sbc     x14, xzr, xzr
    and     x15, x15, x14

    adds    x11, x11, x14
    adc     x12, x12, x15

    stp     x11, x12, [x2]
    ret




.macro f_mul    A0, A1, \
                B0, B1, \
                T0, T1, T2, T3, T4, T5

    mul     \T0, \A0, \B0
    umulh   \T1, \A0, \B0
    mul     \T3, \A0, \B1
    umulh   \T2, \A0, \B1
    
    lsr     \T4, \T0, #1

    adds    \T1, \T1, \T3
    adc     \T2, \T2, xzr


    adds    \T1, \T1, \T0, lsl #63
    adc     \T2, \T2, \T4


    mul     \T3, \A1, \B0
    umulh   \T4, \A1, \B0
    umulh   \T5, \A1, \B1
    mul     \A1, \A1, \B1

    adds    \T4, \T4, \A1
    adc     \T5, \T5, xzr

    adds    \T3, \T3, \T1
    adcs    \T4, \T4, \T2
    adc     \T5, \T5, xzr

    lsr     \T0, \T3, #1

    adds    \T4, \T4, \T3, lsl #63
    adc     \T5, \T5, \T0

    lsr     \T1, \T5, #63
    and     \T5, \T5, #0x7FFFFFFFFFFFFFFF

    adds    \A0, \T4, \T1
    adc     \A1, \T5, xzr


.endm


.macro f_square     A0, A1, \
                    T0, T1, T2, T3, T4, T5

    f_mul   \A0, \A1, \A0, \A1, \T0, \T1, \T2, \T3, \T4, \T5

.endm


.global fmt(f_leg_asm)
fmt(f_leg_asm):

    ldp     x2, x3, [x0]

    sub     sp, sp, #16
    stp     x19, x20, [sp]

    mov     x4, x2
    mov     x5, x3

    mov     x15, x2
    mov     x16, x3


    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3
    mov         x6, x2
    mov         x7, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3
    mov         x8, x2
    mov         x9, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3
    mov         x16, x2
    mov         x17, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3
    mov         x19, x2
    mov         x20, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15


    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15

    

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x19, x20, x10, x11, x12, x13, x14, x15

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x16, x17, x10, x11, x12, x13, x14, x15


    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x8, x9, x10, x11, x12, x13, x14, x15

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x6, x7, x10, x11, x12, x13, x14, x15


    adds        x2, x2, #1
    adc         x3, x3, xzr

    lsr         x4, x3, #63

    and         x3, x3, #0x7FFFFFFFFFFFFFFF

    adds        x2, x2, x4
    adc         x3, x3, xzr

    subs        x2, x2, #1
    sbc         x3, x3, xzr


    ldp         x19, x20, [sp]
    add         sp, sp, #16
    
    and         x2, x2, #0x01

    strb        w2, [x1]

    ret




.global fmt(f_inv_asm)
fmt(f_inv_asm):

    ldp     x2, x3, [x0]

    sub     sp, sp, #16
    stp     x19, x20, [sp]

    mov     x4, x2
    mov     x5, x3

    mov     x6, x2
    mov     x7, x3


    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3
    mov         x8, x2
    mov         x9, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3
    mov         x16, x2
    mov         x17, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3
    mov         x19, x2
    mov         x20, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15
    mov         x4, x2
    mov         x5, x3

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15


    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x4, x5, x10, x11, x12, x13, x14, x15

    

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x19, x20, x10, x11, x12, x13, x14, x15

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x16, x17, x10, x11, x12, x13, x14, x15


    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x8, x9, x10, x11, x12, x13, x14, x15

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x6, x7, x10, x11, x12, x13, x14, x15

    f_square    x2, x3, x10, x11, x12, x13, x14, x15

    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_mul       x2, x3, x6, x7, x10, x11, x12, x13, x14, x15



    adds        x2, x2, #1
    adc         x3, x3, xzr

    lsr         x4, x3, #63

    and         x3, x3, #0x7FFFFFFFFFFFFFFF

    adds        x2, x2, x4
    adc         x3, x3, xzr

    subs        x2, x2, #1
    sbc         x3, x3, xzr


    ldp         x19, x20, [sp]
    add         sp, sp, #16

    stp         x2, x3, [x1]
    ret





.global fmt(f_sqrt_asm)
fmt(f_sqrt_asm):

    ldp     x2, x3, [x0]


    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15
    f_square    x2, x3, x10, x11, x12, x13, x14, x15


    adds        x2, x2, #1
    adc         x3, x3, xzr

    lsr         x4, x3, #63

    and         x3, x3, #0x7FFFFFFFFFFFFFFF

    adds        x2, x2, x4
    adc         x3, x3, xzr

    subs        x2, x2, #1
    sbc         x3, x3, xzr

    stp         x2, x3, [x1]
    ret





#elif (PRIMES == ALT)
// FIELD CONSTANTS

; static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFF61, 0xFFFFFFFFFFFFFFFF}; // Field order p
; static const digit_t Mont_one[WORDS_FIELD]  = {0x000000000000009F, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
; static const digit_t R2[WORDS_FIELD]        = {0x00000000000062C1, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
; static const digit_t iR[WORDS_FIELD]        = {0xB5EFE63D2EB11AF1, 0xB11B5EFE63D2EB11}; // iR =  R^(-1) (mod p)
; static const digit_t pp[WORDS_FIELD]        = {0xB5EFE63D2EB11B5F, 0xB11B5EFE63D2EB11}; // pp = -p^(-1) mod R
; static const digit_t ip[WORDS_FIELD]        = {0x4A1019C2D14EE4A1, 0x4EE4A1019C2D14EE}; // ip =  p^(-1) mod R
; static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000}; // 0
; static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000}; // 1

// Field characterstics
p128:
.quad   0xFFFFFFFFFFFFFF53
.quad   0xFFFFFFFFFFFFFFFF

// Montgomery one = R = 2^128 mod p = 2^128 - p ( = -p % R) (p + Mont_one = R)
Rmp:
.quad   0x00000000000000AD
.quad   0x0000000000000000

// R squared mod p
R2mp:
.quad   0x00000000000074E9
.quad   0x0000000000000000

// Inverse of R mod p 
iRmp:
.quad   0x882383B30D516318
.quad   0x133CABA736C05EB4

// Inverse of -p mod R
impmR:
.quad   0x882383B30D516325
.quad   0x133CABA736C05EB4

// Inverse of p mod R
ipmR:
.quad   0x77DC7C4CF2AE9CDB
.quad   0xECC35458C93FA14B

// Zero // Not actually used
Zero:
.quad   0x0000000000000000
.quad   0x0000000000000000

// One
One:
.quad   0x0000000000000001
.quad   0x0000000000000000


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Modular reduction
;  Input: a[x0] 1 word < R
;  Output: c[x1] 1 words < p
;  Operation: c [x1] =  a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_red_asm)
fmt(f_red_asm):
    ldp     x2, x3, [x0]
    ldr     x4, Rmp

    adds    x2, x2, x4
    adcs    x3, x3, xzr

    sbc     x5, xzr, xzr
    and     x4, x4, x5

    subs    x2, x2, x4
    sbc     x3, x3, xzr

    stp     x2, x3, [x0]
    ret

    ; Alternative algorithm
    ; ldp     x2, x3, [x0]
    ; ldr     x4, p128
    ; ldr     x5, p128 + 8

    ; subs    x2, x2, x4
    ; sbcs    x3, x3, x5

    ; sbc     x6, xzr, xzr

    ; and     x4, x6, x4
    ; and     x5, x6, x5

    ; adds    x2, x2, x4
    ; adc     x3, x3, x5

    ; stp     x2, x3, [x0]
    ; ret
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field addition
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] + b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_add_asm)
fmt(f_add_asm):
    ldp     x3, x4, [x0]
    ldp     x7, x8, [x1]
    ldr     x5, Rmp

    adds    x3, x3, x7
    adcs    x4, x4, x8

    adc     x6, xzr, xzr
    sub     x6, xzr, x6
    and     x6, x6, x5

    adds    x3, x3, x6
    adc     x4, x4, xzr

    adds    x3, x3, x5
    adcs    x4, x4, xzr

    sbc     x6, xzr, xzr
    and     x6, x6, x5

    sub     x3, x3, x6
    sub     x4, x4, xzr

    stp     x3, x4, [x2]
    ret





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field negation
;  Input: a[x0] 1 word < p
;  Output: c[x1] 1 words
;  Operation: c [x1] =  -a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_neg_asm)
fmt(f_neg_asm):
    ; Doesn't work with a = p (which is never an input, but still, above works with a=p)
    ldp     x2, x3, [x0]
    ldr     x4, Rmp

    adds    x2, x2, x4
    adc     x3, x3, xzr

    subs    x2, x4, x2
    sbcs    x3, xzr, x3

    sbc     x5, xzr, xzr
    and     x4, x4, x5

    sub     x2, x2, x4
    sub     x3, x3, xzr

    stp     x2, x3, [x1]
    ret

    ; Alternative algorithm
    ; ldp     x2, x3, [x0]
    ; ldr     x4, p128
    ; ldr     x5, p128 + 8

    ; subs    x2, x4, x2
    ; sbc     x3, x5, x3

    ; subs    x2, x2, x4
    ; sbcs    x3, x3, x5

    ; sbc     x6, xzr, xzr
    ; and     x4, x6, x4
    ; and     x5, x6, x5

    ; adds     x2, x2, x4
    ; adc      x3, x3, x5

    ; stp     x2, x3, [x1]
    ; ret





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field subtraction
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] - b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_sub_asm)
fmt(f_sub_asm):
    ldp     x3, x4, [x0]
    ldp     x5, x6, [x1]
    ldr     x7, Rmp

    subs    x3, x3, x5
    sbcs    x4, x4, x6

    sbc     x8, xzr, xzr
    and     x8, x8, x7

    sub     x3, x3, x8
    sub     x4, x4, xzr

    stp     x3, x4, [x2]
    ret

    ; Alternative algorithm
    ; ldp     x3, x4, [x0]
    ; ldp     x5, x6, [x1]
    ; ldr     x7, p128
    ; ldr     x8, p128 + 8

    ; subs    x3, x3, x5
    ; sbcs    x4, x4, x6

    ; sbc     x10, xzr, xzr
    ; and     x7, x10, x7
    ; and     x8, x10, x8

    ; adds    x3, x3, x7
    ; adc     x4, x4, x8

    ; stp     x3, x4, [x2]
    ; ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Multiprecision multiplication
;  Input: a[x0] 1 word; b[x1] 1 word
;  Output: c[x2] 2 words
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

    stp     x7, x8,  [x2,#00]
    stp     x9, x10, [x2,#16]
    ret




    ; ldp     x3, x4, [x0]
    ; ldp     x5, x6, [x1]

    ; mul     x7, x3, x5
    ; umulh   x8, x3, x5

    ; mul     x10, x3, x6
    ; umulh   x9, x3, x6

    ; adds    x8, x8, x10
    ; adc     x9, x9, xzr

    ; mul     x11, x4, x5

    ; adds    x8, x8, x11   // x7, x8 finished
    ; adc     x9, x9, xzr

    ; umulh   x11, x4, x5
    ; umulh   x10, x4, x6

    ; adds    x9, x9, x11
    ; adc     x10, x10, xzr

    ; mul     x11, x4, x6
    ; adds    x9, x9, x11

    ; adc     x10, x10, xzr

    ; stp     x7, x8, [x2,#00]
    ; stp     x9, x10, [x2,#16]
    ; ret



    ; ldp     x3, x4, [x0]
    ; ldp     x5, x6, [x1]

    ; mul     x7, x3, x5
    ; umulh   x8, x3, x5

    ; mul     x9, x3, x6
    ; umulh   x10, x3, x6

    ; mul     x11, x4, x5
    ; umulh   x12, x4, x5

    ; mul     x13, x4, x6
    ; umulh   x14, x4, x6

    ; adds    x8, x9, x8
    ; adc     x3, x10, xzr
    ; adds    x8, x8, x11
    ; adc     x3, x3, xzr

    ; adds    x3, x3, x12
    ; adc     x4, x14, xzr
    ; adds    x3, x3, x13
    ; adc     x4, x4, xzr

    ; stp     x7, x8, [x2,#00]
    ; stp     x3, x4, [x2,#16]
    ; ret





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Montgomery reduction
;  Input: a[x0] 2 words < p*R
;  Output: c[x1] 1 word < p
;  Operation: c[x1] = a [x0] * (R^(-1)) mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mont_redc_asm)
fmt(mont_redc_asm):

    ldp     x2, x3, [x0,#00]
    ldp     x4, x5, [x0,#16]

    ldr     x6, ipmR
    ldr     x7, ipmR + 8

    ldr     x8, p128
    ldr     x9, p128 + 8

    mul     x10, x2, x6
    umulh   x11, x2, x6
    mul     x12, x2, x7
    mul     x13, x3, x6

    add     x11, x11, x12
    add     x11, x11, x13

    umulh   x12, x10, x8
    mul     x13, x11, x8
    mul     x14, x10, x9

    umulh   x2, x10, x9

    adds    x14, x14, x13
    adc     x2, x2, xzr
    adds    x14, x14, x12
    adc     x2, x2, xzr

    umulh   x13, x11, x8
    mul     x14, x11, x9

    umulh   x3, x11, x9

    adds    x2, x2, x13
    adc     x3, x3, xzr
    adds    x2, x2, x14
    adc     x3, x3, xzr

    subs    x4, x4, x2
    sbcs    x5, x5, x3

    sbc     x10, xzr, xzr
    and     x8, x8, x10
    and     x9, x9, x10

    adds    x4, x4, x8
    adc     x5, x5, x9

    stp     x4, x5, [x1]
    ret





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field multiplication
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 word < p
;  Operation: c [x2] = a [x0] * b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_mul_asm)
fmt(f_mul_asm):
    ldp     x4, x5, [x0,#00]
    mov     x0, x2
    ldp     x2, x3, [x1,#00]


    umulh   x10, x2, x4
    mul     x11, x3, x4
    mul     x12, x2, x5

    ldr     x6, ipmR
    ldr     x7, ipmR + 8

    umulh   x13, x3, x4
    umulh   x14, x2, x5
    mul     x15, x3, x5

    mul     x2, x2, x4
    umulh   x5, x3, x5

    ldr     x8, p128
    ldr     x9, p128 + 8

    adds    x3, x10, x11
    adcs    x4, x13, x14
    adc     x5, x5, xzr

    adds    x3, x3, x12
    adcs    x4, x4, x15
    adc     x5, x5, xzr


    mul     x10, x2, x6
    umulh   x11, x2, x6
    mul     x12, x2, x7
    mul     x13, x3, x6

    add     x11, x11, x12
    add     x11, x11, x13

    umulh   x12, x10, x8
    mul     x13, x11, x8
    mul     x14, x10, x9

    umulh   x2, x10, x9

    adds    x14, x14, x13
    adc     x2, x2, xzr
    adds    x14, x14, x12
    adc     x2, x2, xzr

    umulh   x13, x11, x8
    mul     x14, x11, x9

    umulh   x3, x11, x9

    adds    x2, x2, x13
    adc     x3, x3, xzr
    adds    x2, x2, x14
    adc     x3, x3, xzr

    subs    x4, x4, x2
    sbcs    x5, x5, x3

    sbc     x10, xzr, xzr
    and     x8, x8, x10
    and     x9, x9, x10

    adds    x4, x4, x8
    adc     x5, x5, x9

    stp     x4, x5, [x0]
    ret


    ; ldp     x3, x4, [x0]
    ; ldp     x5, x6, [x1]

    ; ldr     x0, Rmp

    ; mul     x7, x5, x3
    ; umulh   x10, x6, x4
    ; mul     x8, x6, x3
    ; umulh   x9, x5, x4

    ; umulh   x11, x5, x3
    ; mul     x12, x6, x4
    ; mul     x13, x5, x4
    ; umulh   x14, x6, x3

    ; adds    x8, x8, x11
    ; adcs    x9, x9, x12
    ; adc     x10, x10, xzr

    ; adds    x8, x8, x13
    ; adcs    x9, x9, x14
    ; adc     x10, x10, xzr


    ; mul     x3, x9, x0
    ; umulh   x6, x9, x0
    ; mul     x4, x10, x0
    ; umulh   x5, x10, x0

    ; adds    x4, x4, x6
    ; adc     x5, x5, xzr

    ; adds    x3, x3, x7
    ; adcs    x4, x4, x8
    ; adc     x5, x5, xzr

    ; mul     x7, x5, x0
    ; umulh   x8, x5, x0

    ; adds    x3, x3, x7
    ; adcs    x4, x4, x8

    ; adc     x5, xzr, xzr
    ; sub     x5, xzr, x5
    ; and     x5, x5, x0

    ; adds    x3, x3, x5
    ; adc     x4, x4, xzr

    ; adds    x3, x3, x0
    ; adcs    x4, x4, xzr

    ; sbc     x5, xzr, xzr
    ; and     x5, x5, x0

    ; subs    x3, x3, x5
    ; sbcs    x4, x4, xzr

    ; stp     x3, x4, [x2]

    ; ret

    
    ; ldr     x0, [x0]
    ; ldr     x1, [x1]
    ; ldr     x8, ipmR

    ; mul     x3, x0, x1
    ; umulh   x4, x0, x1

    ; ldr     x5, Rmp

    ; mul     x6, x3, x8

    ; mul     x11, x6, x5
    ; umulh   x7, x6, x5

    ; adds    x3, x3, x11
    ; adc     x4, x4, x7

    ; subs    x4, x4, x6

    ; sbc     x7, xzr, xzr 
    ; and     x7, x7, x5

    ; subs    x4, x4, x7

    ; str     x4, [x2]

    ; ret



#endif
