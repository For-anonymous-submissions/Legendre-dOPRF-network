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

; static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};// Field order p
; static const digit_t Mont_one[WORDS_FIELD]  = {0x0000000000000001, 0x0000000000000000, 0x0000000000000001, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000};// R  =  2^{NBITS_PRIME} (mod p)
; static const digit_t R2[WORDS_FIELD]        = {0x0000000000000001, 0x0000000000000000, 0x0000000000000002, 0xFFFFFFFFFFFFFFFE, 0x0000000000000002, 0xFFFFFFFFFFFFFFFE, 0x0000000000000002, 0xFFFFFFFFFFFFFFFE};// R2 = (2^{NBITS_PRIME})^2 (mod p)
; static const digit_t iR[WORDS_FIELD]        = {0xFFFFFFFFFFFFFFFD, 0x0000000000000002, 0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFD, 0x0000000000000001, 0x0000000000000001};// iR =  R^(-1) (mod p)
; static const digit_t pp[WORDS_FIELD]        = {0x0000000000000001, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 0x0000000000000000, 0xFFFFFFFFFFFFFFFE, 0x0000000000000001, 0x0000000000000001};// pp = -p^(-1) mod R
; static const digit_t ip[WORDS_FIELD]        = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0000000000000001, 0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFE};// ip =  p^(-1) mod R
; static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000};// 0
; static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000};// 1

// Field characterstics
p512:
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFE
.quad   0x0000000000000000
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF

// Montgomery one = R % p
Rmp:
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000001
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000

// R squared mod p
R2mp:
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000002
.quad   0xFFFFFFFFFFFFFFFE
.quad   0x0000000000000002
.quad   0xFFFFFFFFFFFFFFFE
.quad   0x0000000000000002
.quad   0xFFFFFFFFFFFFFFFE

// Inverse of R mod p 
iRmp:
.quad   0xFFFFFFFFFFFFFFFD
.quad   0x0000000000000002
.quad   0xFFFFFFFFFFFFFFFE
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFD
.quad   0x0000000000000001
.quad   0x0000000000000001

// Inverse of -p mod R
impmR:
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0xFFFFFFFFFFFFFFFE
.quad   0x0000000000000001
.quad   0x0000000000000001

// Inverse of p mod r
ipmR:
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x0000000000000000
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x0000000000000001
.quad   0xFFFFFFFFFFFFFFFE
.quad   0xFFFFFFFFFFFFFFFE

// Zero // Not actually used
Zero:
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
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
.quad   0x0000000000000000
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
    ldp     x2, x3, [x0, #0x00]
    ldp     x4, x5, [x0, #0x10]
    ldp     x6, x7, [x0, #0x20]
    ldp     x8, x9, [x0, #0x30]


    mov     x10, #0xFFFFFFFFFFFFFFFF
    mov     x11, #0xFFFFFFFFFFFFFFFE


    subs    x2, x2, x10
    sbcs    x3, x3, x10
    sbcs    x4, x4, x11
    sbcs    x5, x5, xzr
    sbcs    x6, x6, x10
    sbcs    x7, x7, x10
    sbcs    x8, x8, x10
    sbcs    x9, x9, x10

    sbc     x10, xzr, xzr
    and     x11, x11, x10

    adds    x2, x2, x10
    adcs    x3, x3, x10
    adcs    x4, x4, x11
    adcs    x5, x5, xzr
    adcs    x6, x6, x10
    adcs    x7, x7, x10
    adcs    x8, x8, x10
    adc     x9, x9, x10

    stp     x2, x3, [x0, #0x00]
    stp     x4, x5, [x0, #0x10]
    stp     x6, x7, [x0, #0x20]
    stp     x8, x9, [x0, #0x30]
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field addition
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] + b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_add_asm)
fmt(f_add_asm):
    ldp     x3, x4,  [x1, #0x00]
    ldp     x5, x6,  [x1, #0x10]
    ldp     x7, x8,  [x1, #0x20]
    ldp     x9, x10, [x1, #0x30]
    
    /* save variables on stack */
    sub     sp,  sp,  #0x20
    stp     x19, x20, [sp, #0x00]
    stp     x21, x22, [sp, #0x10]

    ldp     x14, x15, [x0, #0x00]
    ldp     x16, x17, [x0, #0x10]
    ldp     x19, x20, [x0, #0x20]
    ldp     x21, x22, [x0, #0x30]

    mov     x11, #0xFFFFFFFFFFFFFFFF
    mov     x12, #0xFFFFFFFFFFFFFFFE

    subs    x3,  x11, x3
    sbcs    x4,  x11, x4
    sbcs    x5,  x12, x5
    sbcs    x6,  xzr, x6
    sbcs    x7,  x11, x7
    sbcs    x8,  x11, x8
    sbcs    x9,  x11, x9
    sbc     x10, x11, x10

    subs    x14, x14, x3
    sbcs    x15, x15, x4
    sbcs    x16, x16, x5
    sbcs    x17, x17, x6
    sbcs    x19, x19, x7
    sbcs    x20, x20, x8
    sbcs    x21, x21, x9
    sbcs    x22, x22, x10

    sbc     x11, xzr, xzr
    and     x12, x12, x11

    adds    x14, x14, x11
    adcs    x15, x15, x11
    adcs    x16, x16, x12
    adcs    x17, x17, xzr
    adcs    x19, x19, x11
    adcs    x20, x20, x11
    adcs    x21, x21, x11
    adc     x22, x22, x11

    stp     x14, x15, [x2, #0x00]
    stp     x16, x17, [x2, #0x10]
    stp     x19, x20, [x2, #0x20]
    stp     x21, x22, [x2, #0x30]

    /* restore stack */
    ldp     x19, x20, [sp, #0x00]
    ldp     x21, x22, [sp, #0x10]
    add     sp,  sp,  #0x20

    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field negation
;  Input: a[x0] 1 word < p
;  Output: c[x1] 1 words
;  Operation: c [x1] =  -a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_neg_asm)
fmt(f_neg_asm):
    ldp     x3, x4,  [x0, #0x00]
    ldp     x5, x6,  [x0, #0x10]
    ldp     x7, x8,  [x0, #0x20]
    ldp     x9, x10, [x0, #0x30]
    
    mov     x11, #0xFFFFFFFFFFFFFFFF
    mov     x12, #0xFFFFFFFFFFFFFFFE

    subs    x3,  x11, x3
    sbcs    x4,  x11, x4
    sbcs    x5,  x12, x5
    sbcs    x6,  xzr, x6
    sbcs    x7,  x11, x7
    sbcs    x8,  x11, x8
    sbcs    x9,  x11, x9
    sbc     x10, x11, x10

    subs    x3,  x3,  x11
    sbcs    x4,  x4,  x11
    sbcs    x5,  x5,  x12
    sbcs    x6,  x6,  xzr
    sbcs    x7,  x7,  x11
    sbcs    x8,  x8,  x11
    sbcs    x9,  x9,  x11
    sbcs    x10, x10, x11

    sbc     x11, xzr, xzr
    and     x12, x12, x11

    adds    x3,  x3,  x11
    adcs    x4,  x4,  x11
    adcs    x5,  x5,  x12
    adcs    x6,  x6,  xzr
    adcs    x7,  x7,  x11
    adcs    x8,  x8,  x11
    adcs    x9,  x9,  x11
    adc     x10, x10, x11

    stp     x3, x4,  [x1, #0x00]
    stp     x5, x6,  [x1, #0x10]
    stp     x7, x8,  [x1, #0x20]
    stp     x9, x10, [x1, #0x30]

    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field subtraction
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] - b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_sub_asm)
fmt(f_sub_asm):
    /* save variables on stack */
    sub     sp,  sp,  #0x20
    stp     x19, x20, [sp, #0x00]
    stp     x21, x22, [sp, #0x10]


    ldp     x3, x4,  [x1, #0x00]
    ldp     x5, x6,  [x1, #0x10]
    ldp     x7, x8,  [x1, #0x20]
    ldp     x9, x10, [x1, #0x30]
    

    ldp     x14, x15, [x0, #0x00]
    ldp     x16, x17, [x0, #0x10]
    ldp     x19, x20, [x0, #0x20]
    ldp     x21, x22, [x0, #0x30]

    mov     x12, #0xFFFFFFFFFFFFFFFE

    subs    x14, x14, x3
    sbcs    x15, x15, x4
    sbcs    x16, x16, x5
    sbcs    x17, x17, x6
    sbcs    x19, x19, x7
    sbcs    x20, x20, x8
    sbcs    x21, x21, x9
    sbcs    x22, x22, x10

    sbc     x11, xzr, xzr
    and     x12, x12, x11

    adds    x14, x14, x11
    adcs    x15, x15, x11
    adcs    x16, x16, x12
    adcs    x17, x17, xzr
    adcs    x19, x19, x11
    adcs    x20, x20, x11
    adcs    x21, x21, x11
    adc     x22, x22, x11

    stp     x14, x15, [x2, #0x00]
    stp     x16, x17, [x2, #0x10]
    stp     x19, x20, [x2, #0x20]
    stp     x21, x22, [x2, #0x30]

    /* restore stack */
    ldp     x19, x20, [sp, #0x00]
    ldp     x21, x22, [sp, #0x10]
    add     sp,  sp,  #0x20

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
    ldp     x3,  x4,  [x1, #0x00]
    ldp     x5,  x6,  [x0, #0x00]

    mul     x7,  x5,  x3
    umulh   x10, x6,  x4
    mul     x8,  x6,  x3
    umulh   x9,  x5,  x4

    umulh   x11, x5,  x3
    mul     x12, x6,  x4

    adds    x8,  x8,  x11
    adcs    x9,  x9,  x12
    adc     x10, x10, xzr

    mul     x11, x5,  x4
    umulh   x12, x6,  x3

    adds    x8,  x8,  x11
    adcs    x9,  x9,  x12
    adc     x10, x10, xzr

    stp     x7,  x8,  [x2, #0x00]
    stp     x9,  x10, [x2, #0x10]
    ret




// In first step C = 0, so no need to add, just directly compute into C

.macro mul_2x8_and_add_0        C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10,    \
                                B0, B1, B2, B3, B4, B5, B6, B7,                 \
                                A0, A1,                                         \
                                T0, T1, T2, T3, T4, T5, T6, T7


    mul     \C0, \A0, \B0
    umulh   \C1, \A0, \B0
    mul     \C2, \A0, \B2
    umulh   \C3, \A0, \B2
    mul     \C4, \A0, \B4
    umulh   \C5, \A0, \B4
    mul     \C6, \A0, \B6
    umulh   \C7, \A0, \B6

    ; adds    \C0, \C0, \T0
    ; adcs    \C1, \C1, \T1
    ; adcs    \C2, \C2, \T2
    ; adcs    \C3, \C3, \T3
    ; adcs    \C4, \C4, \T4
    ; adcs    \C5, \C5, \T5
    ; adcs    \C6, \C6, \T6
    ; adcs    \C7, \C7, \T7
    ; adcs    \C8, \C8, xzr
    ; adcs    \C9, \C9, xzr

    mul     \T0, \A0, \B1
    umulh   \T1, \A0, \B1
    mul     \T2, \A0, \B3
    umulh   \T3, \A0, \B3
    mul     \T4, \A0, \B5
    umulh   \T5, \A0, \B5
    mul     \T6, \A0, \B7
    umulh   \C8, \A0, \B7

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adcs    \C4, \C4, \T3
    adcs    \C5, \C5, \T4
    adcs    \C6, \C6, \T5
    adcs    \C7, \C7, \T6
    adc     \C8, \C8, xzr


    mul     \T0, \A1, \B0
    umulh   \T1, \A1, \B0
    mul     \T2, \A1, \B2
    umulh   \T3, \A1, \B2
    mul     \T4, \A1, \B4
    umulh   \T5, \A1, \B4
    mul     \T6, \A1, \B6
    umulh   \T7, \A1, \B6

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adcs    \C4, \C4, \T3
    adcs    \C5, \C5, \T4
    adcs    \C6, \C6, \T5
    adcs    \C7, \C7, \T6
    adcs    \C8, \C8, \T7
    adc     \C9, xzr, xzr

    mul     \T0, \A1, \B1
    umulh   \T1, \A1, \B1
    mul     \T2, \A1, \B3
    umulh   \T3, \A1, \B3
    mul     \T4, \A1, \B5
    umulh   \T5, \A1, \B5
    mul     \T6, \A1, \B7
    umulh   \T7, \A1, \B7

    adds    \C2, \C2, \T0
    adcs    \C3, \C3, \T1
    adcs    \C4, \C4, \T2
    adcs    \C5, \C5, \T3
    adcs    \C6, \C6, \T4
    adcs    \C7, \C7, \T5
    adcs    \C8, \C8, \T6
    adcs    \C9, \C9, \T7
    adc     \C10,xzr, xzr


.endm



.macro mul_2x8_and_add  C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10,    \
                        B0, B1, B2, B3, B4, B5, B6, B7,                 \
                        A0, A1,                                         \
                        T0, T1, T2, T3, T4, T5, T6, T7


    mul     \T0, \A0, \B0
    umulh   \T1, \A0, \B0
    mul     \T2, \A0, \B2
    umulh   \T3, \A0, \B2
    mul     \T4, \A0, \B4
    umulh   \T5, \A0, \B4
    mul     \T6, \A0, \B6
    umulh   \T7, \A0, \B6

    adds    \C0, \C0, \T0
    adcs    \C1, \C1, \T1
    adcs    \C2, \C2, \T2
    adcs    \C3, \C3, \T3
    adcs    \C4, \C4, \T4
    adcs    \C5, \C5, \T5
    adcs    \C6, \C6, \T6
    adcs    \C7, \C7, \T7
    adcs    \C8, \C8, xzr
    adc     \C9, xzr, xzr

    mul     \T0, \A0, \B1
    umulh   \T1, \A0, \B1
    mul     \T2, \A0, \B3
    umulh   \T3, \A0, \B3
    mul     \T4, \A0, \B5
    umulh   \T5, \A0, \B5
    mul     \T6, \A0, \B7
    umulh   \T7, \A0, \B7

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adcs    \C4, \C4, \T3
    adcs    \C5, \C5, \T4
    adcs    \C6, \C6, \T5
    adcs    \C7, \C7, \T6
    adcs    \C8, \C8, \T7
    adc     \C9, \C9, xzr


    mul     \T0, \A1, \B0
    umulh   \T1, \A1, \B0
    mul     \T2, \A1, \B2
    umulh   \T3, \A1, \B2
    mul     \T4, \A1, \B4
    umulh   \T5, \A1, \B4
    mul     \T6, \A1, \B6
    umulh   \T7, \A1, \B6

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adcs    \C4, \C4, \T3
    adcs    \C5, \C5, \T4
    adcs    \C6, \C6, \T5
    adcs    \C7, \C7, \T6
    adcs    \C8, \C8, \T7
    adc     \C9, \C9, xzr


    mul     \T0, \A1, \B1
    umulh   \T1, \A1, \B1
    mul     \T2, \A1, \B3
    umulh   \T3, \A1, \B3
    mul     \T4, \A1, \B5
    umulh   \T5, \A1, \B5
    mul     \T6, \A1, \B7
    umulh   \T7, \A1, \B7

    adds    \C2, \C2, \T0
    adcs    \C3, \C3, \T1
    adcs    \C4, \C4, \T2
    adcs    \C5, \C5, \T3
    adcs    \C6, \C6, \T4
    adcs    \C7, \C7, \T5
    adcs    \C8, \C8, \T6
    adcs    \C9, \C9, \T7
    adc     \C10,xzr, xzr

.endm





.macro add_c0_mu_p      C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10

;   adds    \C0, \C0, xzr
;   adcs    \C1, \C1, xzr
;   adcs    \C2, \C2, xzr
    adds    \C3, \C3, \C0
    adcs    \C4, \C4, \C1
    adcs    \C5, \C5, xzr
    adcs    \C6, \C6, xzr
    adcs    \C7, \C7, xzr
    adcs    \C8, \C8, \C0
    adcs    \C9, \C9, \C1
    adc     \C10,\C10,xzr

;   subs    \C0, \C0, \C0   // subtract with C0
;   sbcs    \C1, \C1, \C1
    subs    \C2, \C2, \C0
    sbcs    \C3, \C3, \C1
    sbcs    \C4, \C4, \C0
    sbcs    \C5, \C5, \C1
    sbcs    \C6, \C6, xzr
    sbcs    \C7, \C7, xzr
    sbcs    \C8, \C8, xzr
    sbcs    \C9, \C9, xzr
    sbc     \C10,\C10,xzr

.endm



.macro interleave_step_0    C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10,    \
                            B0, B1, B2, B3, B4, B5, B6, B7,                 \
                            A0, A1,                                         \
                            T0, T1, T2, T3, T4, T5, T6, T7


    mul_2x8_and_add_0   \C0, \C1, \C2, \C3, \C4, \C5, \C6, \C7, \C8, \C9, \C10, \
                        \B0, \B1, \B2, \B3, \B4, \B5, \B6, \B7,                 \
                        \A0, \A1,                                               \
                        \T0, \T1, \T2, \T3, \T4, \T5, \T6, \T7

    add_c0_mu_p         \C0, \C1, \C2, \C3, \C4, \C5, \C6, \C7, \C8, \C9, \C10


.endm



.macro interleave_step  C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, C10,        \
                        B0, B1, B2, B3, B4, B5, B6, B7,                     \
                        A0, A1,                                             \
                        T0, T1, T2, T3, T4, T5, T6, T7


    mul_2x8_and_add     \C0, \C1, \C2, \C3, \C4, \C5, \C6, \C7, \C8, \C9, \C10,     \
                        \B0, \B1, \B2, \B3, \B4, \B5, \B6, \B7,                     \
                        \A0, \A1,                                                   \
                        \T0, \T1, \T2, \T3, \T4, \T5, \T6, \T7

    add_c0_mu_p         \C0, \C1, \C2, \C3, \C4, \C5, \C6, \C7, \C8, \C9, \C10


.endm


//***********************************************************************
//  Field multiplication
//  Input: a[x0] 1 word < p; b[x1] 1 word < p
//  Output: c[x2] 1 word < p
//  Operation: c [x2] = a [x0] * b [x1] mod p
//*********************************************************************** 
.global fmt(f_mul_asm)
fmt(f_mul_asm):


    /* save variables on stack */
    sub     sp,  sp,  #0x70
    stp     x19, x20, [sp, #0x00]
    stp     x21, x22, [sp, #0x10]
    stp     x23, x24, [sp, #0x20]
    stp     x25, x26, [sp, #0x30]
    stp     x27, x28, [sp, #0x40]
    stp     x29, x30, [sp, #0x50]
    stp     x1,  x2,  [sp, #0x60]


    ldp     x3,  x4,  [x1, #0x00]
    ldp     x5,  x6,  [x1, #0x10]
    ldp     x7,  x8,  [x1, #0x20]
    ldp     x9,  x10, [x1, #0x30]
    


    ldp     x1,  x2,  [x0, #0x00]

    interleave_step_0   x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30,  \
                        x3, x4, x5, x6, x7, x8, x9, x10,                        \
                        x1, x2,                                                 \
                        x11, x12, x13, x14, x15, x16, x17, x19 


    ldp     x1, x2,   [x0, #0x10]

    interleave_step     x22, x23, x24, x25, x26, x27, x28, x29, x30, x20, x21,  \
                        x3, x4, x5, x6, x7, x8, x9, x10,                        \
                        x1, x2,                                                 \
                        x11, x12, x13, x14, x15, x16, x17, x19



    ldp     x1, x2,   [x0, #0x20]

    interleave_step     x24, x25, x26, x27, x28, x29, x30, x20, x21, x22, x23,  \
                        x3, x4, x5, x6, x7, x8, x9, x10,                        \
                        x1, x2,                                                 \
                        x11, x12, x13, x14, x15, x16, x17, x19



    ldp     x1, x2,   [x0, #0x30]

    interleave_step     x26, x27, x28, x29, x30, x20, x21, x22, x23, x24, x25,  \
                        x3, x4, x5, x6, x7, x8, x9, x10,                        \
                        x1, x2,                                                 \
                        x11, x12, x13, x14, x15, x16, x17, x19



    sub     x11, xzr, x25
    and     x12, x11, #0xFFFFFFFFFFFFFFFE

    subs    x3,  x28, x11
    sbcs    x4,  x29, x11
    sbcs    x5,  x30, x12
    sbcs    x6,  x20, xzr
    sbcs    x7,  x21, x11
    sbcs    x8,  x22, x11
    sbcs    x9,  x23, x11
    sbc     x10, x24, x11

    mov     x11, #0xFFFFFFFFFFFFFFFF
    mov     x12, #0xFFFFFFFFFFFFFFFE

    subs    x3,  x3,  x11
    sbcs    x4,  x4,  x11
    sbcs    x5,  x5,  x12
    sbcs    x6,  x6,  xzr
    sbcs    x7,  x7,  x11
    sbcs    x8,  x8,  x11
    sbcs    x9,  x9,  x11
    sbcs    x10, x10, x11

    sbc     x11, xzr, xzr
    and     x12, x12, x11

    adds    x3,  x3,  x11
    adcs    x4,  x4,  x11
    adcs    x5,  x5,  x12
    adcs    x6,  x6,  xzr
    adcs    x7,  x7,  x11
    adcs    x8,  x8,  x11
    adcs    x9,  x9,  x11
    adc     x10, x10, x11



    /* restore stack */
    ldp     x19, x20, [sp, #0x00]
    ldp     x21, x22, [sp, #0x10]
    ldp     x23, x24, [sp, #0x20]
    ldp     x25, x26, [sp, #0x30]
    ldp     x27, x28, [sp, #0x40]
    ldp     x29, x30, [sp, #0x50]
    ldp     x1,  x2,  [sp, #0x60]
    add     sp,  sp,  #0x70



    stp     x3,  x4,  [x2, #0x00]
    stp     x5,  x6,  [x2, #0x10]
    stp     x7,  x8,  [x2, #0x20]
    stp     x9,  x10, [x2, #0x30]

    ret




#elif (PRIMES == ALT)




// FIELD CONSTANTS

; static const digit_t p[WORDS_FIELD]         = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF}; // Field order p
; static const digit_t Mont_one[WORDS_FIELD]  = {0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0000000000000001, 0x0000000000000000, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
; static const digit_t R2[WORDS_FIELD]        = {0x0000000000000004, 0x0000000000000000, 0xFFFFFFFFFFFFFFF8, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF7, 0xFFFFFFFFFFFFFFFF, 0x0000000000000007}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
; static const digit_t iR[WORDS_FIELD]        = {0x0000000000000000, 0x0000000000000000, 0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFFF}; // iR =  R^(-1) (mod p)
; static const digit_t pp[WORDS_FIELD]        = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF}; // pp = -p^(-1) mod R
; static const digit_t ip[WORDS_FIELD]        = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000001, 0x0000000000000000, 0x8000000000000000}; // ip =  p^(-1) mod R
; static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 0
; static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 1

// Field characterstics
p512:
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x7FFFFFFFFFFFFFFF

// Montgomery one = R = 2^128 mod p = 2^128 - p ( = -p % R) (p + Mont_one = R)
Rmp:
.quad   0xFFFFFFFFFFFFFFFE
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000000

// R squared mod p
R2mp:
.quad   0x0000000000000004
.quad   0x0000000000000000
.quad   0xFFFFFFFFFFFFFFF8
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFF7
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x0000000000000007

// Inverse of R mod p 
iRmp:
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x3FFFFFFFFFFFFFFF

// Inverse of -p mod R
impmR:
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFE
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x7FFFFFFFFFFFFFFF

// Inverse of p mod r
ipmR:
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x8000000000000000


// Zero // Not actually used
Zero:
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000
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
.quad   0x0000000000000000
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
    ldp     x2, x3, [x0, #0x00]
    ldp     x4, x5, [x0, #0x10]
    ldp     x6, x7, [x0, #0x20]
    ldp     x8, x9, [x0, #0x30]


    mov     x10, #0x0000000000000001
    mov     x11, #0xFFFFFFFFFFFFFFFF
    mov     x12, #0x7FFFFFFFFFFFFFFF


    lsr     x14, x9, #63
    sub     x14, xzr, x14
    and     x9, x9, x12

    adds    x2, x2, x14
    adcs    x3, x3, x14
    adcs    x4, x4, x14
    adcs    x5, x5, x14
    adcs    x6, x6, x14
    adcs    x7, x7, xzr
    adcs    x8, x8, xzr
    adc     x9, x9, xzr

    subs    x2, x2, x10
    sbcs    x3, x3, xzr
    sbcs    x4, x4, xzr
    sbcs    x5, x5, xzr
    sbcs    x6, x6, xzr
    sbcs    x7, x7, x11
    sbcs    x8, x8, x11
    sbcs    x9, x9, x12

    sbc     x11, xzr, xzr
    and     x10, x10, x11
    and     x12, x12, x11

    adds    x2, x2, x10
    adcs    x3, x3, xzr
    adcs    x4, x4, xzr
    adcs    x5, x5, xzr
    adcs    x6, x6, xzr
    adcs    x7, x7, x11
    adcs    x8, x8, x11
    adc     x9, x9, x12

    stp     x2, x3, [x0, #0x00]
    stp     x4, x5, [x0, #0x10]
    stp     x6, x7, [x0, #0x20]
    stp     x8, x9, [x0, #0x30]
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field addition
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] + b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_add_asm)
fmt(f_add_asm):
    ldp     x3, x4,  [x1, #0x00]
    ldp     x5, x6,  [x1, #0x10]
    ldp     x7, x8,  [x1, #0x20]
    ldp     x9, x10, [x1, #0x30]
    
    /* save variables on stack */
    sub     sp,  sp,  #0x20
    stp     x19, x20, [sp, #0x00]
    stp     x21, x22, [sp, #0x10]

    ldp     x14, x15, [x0, #0x00]
    ldp     x16, x17, [x0, #0x10]
    ldp     x19, x20, [x0, #0x20]
    ldp     x21, x22, [x0, #0x30]

    mov     x11, #0x0000000000000001
    mov     x12, #0xFFFFFFFFFFFFFFFF
    mov     x13, #0x7FFFFFFFFFFFFFFF


    subs    x3,  x11, x3
    sbcs    x4,  xzr, x4
    sbcs    x5,  xzr, x5
    sbcs    x6,  xzr, x6
    sbcs    x7,  xzr, x7
    sbcs    x8,  x12, x8
    sbcs    x9,  x12, x9
    sbc     x10, x13, x10

    subs    x14, x14, x3
    sbcs    x15, x15, x4
    sbcs    x16, x16, x5
    sbcs    x17, x17, x6
    sbcs    x19, x19, x7
    sbcs    x20, x20, x8
    sbcs    x21, x21, x9
    sbcs    x22, x22, x10

    sbc     x12, xzr, xzr
    and     x11, x11, x12
    and     x13, x13, x12

    adds    x14, x14, x11
    adcs    x15, x15, xzr
    adcs    x16, x16, xzr
    adcs    x17, x17, xzr
    adcs    x19, x19, xzr
    adcs    x20, x20, x12
    adcs    x21, x21, x12
    adc     x22, x22, x13

    stp     x14, x15, [x2, #0x00]
    stp     x16, x17, [x2, #0x10]
    stp     x19, x20, [x2, #0x20]
    stp     x21, x22, [x2, #0x30]

    /* restore stack */
    ldp     x19, x20, [sp, #0x00]
    ldp     x21, x22, [sp, #0x10]
    add     sp,  sp,  #0x20

    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field negation
;  Input: a[x0] 1 word < p
;  Output: c[x1] 1 words
;  Operation: c [x1] =  -a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_neg_asm)
fmt(f_neg_asm):
    ldp     x3, x4,  [x0, #0x00]
    ldp     x5, x6,  [x0, #0x10]
    ldp     x7, x8,  [x0, #0x20]
    ldp     x9, x10, [x0, #0x30]
    
    mov     x11, #0x0000000000000001
    mov     x12, #0xFFFFFFFFFFFFFFFF
    mov     x13, #0x7FFFFFFFFFFFFFFF

    subs    x3,  x11, x3
    sbcs    x4,  xzr, x4
    sbcs    x5,  xzr, x5
    sbcs    x6,  xzr, x6
    sbcs    x7,  xzr, x7
    sbcs    x8,  x12, x8
    sbcs    x9,  x12, x9
    sbc     x10, x13, x10

    subs    x3,  x3,  x11
    sbcs    x4,  x4,  xzr
    sbcs    x5,  x5,  xzr
    sbcs    x6,  x6,  xzr
    sbcs    x7,  x7,  xzr
    sbcs    x8,  x8,  x12
    sbcs    x9,  x9,  x12
    sbcs    x10, x10, x13

    sbc     x12, xzr, xzr
    and     x11, x11, x12
    and     x13, x13, x12

    adds    x3,  x3,  x11
    adcs    x4,  x4,  xzr
    adcs    x5,  x5,  xzr
    adcs    x6,  x6,  xzr
    adcs    x7,  x7,  xzr
    adcs    x8,  x8,  x12
    adcs    x9,  x9,  x12
    adc     x10, x10, x13

    stp     x3, x4,  [x1, #0x00]
    stp     x5, x6,  [x1, #0x10]
    stp     x7, x8,  [x1, #0x20]
    stp     x9, x10, [x1, #0x30]

    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field subtraction
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] - b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_sub_asm)
fmt(f_sub_asm):
    /* save variables on stack */
    sub     sp,  sp,  #0x20
    stp     x19, x20, [sp, #0x00]
    stp     x21, x22, [sp, #0x10]


    ldp     x3, x4,  [x1, #0x00]
    ldp     x5, x6,  [x1, #0x10]
    ldp     x7, x8,  [x1, #0x20]
    ldp     x9, x10, [x1, #0x30]
    

    ldp     x14, x15, [x0, #0x00]
    ldp     x16, x17, [x0, #0x10]
    ldp     x19, x20, [x0, #0x20]
    ldp     x21, x22, [x0, #0x30]

    mov     x11, #0x0000000000000001
    mov     x12, #0xFFFFFFFFFFFFFFFF
    mov     x13, #0x7FFFFFFFFFFFFFFF

    subs    x14, x14, x3
    sbcs    x15, x15, x4
    sbcs    x16, x16, x5
    sbcs    x17, x17, x6
    sbcs    x19, x19, x7
    sbcs    x20, x20, x8
    sbcs    x21, x21, x9
    sbcs    x22, x22, x10

    sbc     x12, xzr, xzr
    and     x11, x11, x12
    and     x13, x13, x12

    adds    x14, x14, x11
    adcs    x15, x15, xzr
    adcs    x16, x16, xzr
    adcs    x17, x17, xzr
    adcs    x19, x19, xzr
    adcs    x20, x20, x12
    adcs    x21, x21, x12
    adc     x22, x22, x13

    stp     x14, x15, [x2, #0x00]
    stp     x16, x17, [x2, #0x10]
    stp     x19, x20, [x2, #0x20]
    stp     x21, x22, [x2, #0x30]

    /* restore stack */
    ldp     x19, x20, [sp, #0x00]
    ldp     x21, x22, [sp, #0x10]
    add     sp,  sp,  #0x20

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
    ldp     x3,  x4,  [x1, #0x00]
    ldp     x5,  x6,  [x0, #0x00]

    mul     x7,  x5,  x3
    umulh   x10, x6,  x4
    mul     x8,  x6,  x3
    umulh   x9,  x5,  x4

    umulh   x11, x5,  x3
    mul     x12, x6,  x4

    adds    x8,  x8,  x11
    adcs    x9,  x9,  x12
    adc     x10, x10, xzr

    mul     x11, x5,  x4
    umulh   x12, x6,  x3

    adds    x8,  x8,  x11
    adcs    x9,  x9,  x12
    adc     x10, x10, xzr

    stp     x7,  x8,  [x2, #0x00]
    stp     x9,  x10, [x2, #0x10]
    ret




// In first step C = 0, so no need to add, just directly compute into C

.macro mul_2x8_and_add_0        C0, C1, C2, C3, C4, C5, C6, C7, C8, C9,\
                                B0, B1, B2, B3, B4, B5, B6, B7, \
                                A0, A1, \
                                T0, T1, T2, T3, T4, T5, T6, T7, T8


    mul     \C0, \A0, \B0
    umulh   \C1, \A0, \B0
    mul     \C2, \A0, \B2
    umulh   \C3, \A0, \B2
    mul     \C4, \A0, \B4
    umulh   \C5, \A0, \B4
    mul     \C6, \A0, \B6
    umulh   \C7, \A0, \B6

    ; adds    \C0, \C0, \T0
    ; adcs    \C1, \C1, \T1
    ; adcs    \C2, \C2, \T2
    ; adcs    \C3, \C3, \T3
    ; adcs    \C4, \C4, \T4
    ; adcs    \C5, \C5, \T5
    ; adcs    \C6, \C6, \T6
    ; adcs    \C7, \C7, \T7
    ; adcs    \C8, \C8, xzr
    ; adcs    \C9, \C9, xzr

    mul     \T0, \A0, \B1
    umulh   \T1, \A0, \B1
    mul     \T2, \A0, \B3
    umulh   \T3, \A0, \B3
    mul     \T4, \A0, \B5
    umulh   \T5, \A0, \B5
    mul     \T6, \A0, \B7
    umulh   \C8, \A0, \B7

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adcs    \C4, \C4, \T3
    adcs    \C5, \C5, \T4
    adcs    \C6, \C6, \T5
    adcs    \C7, \C7, \T6
    adc     \C8, \C8, xzr


    mul     \T0, \A1, \B0
    umulh   \T1, \A1, \B0
    mul     \T2, \A1, \B2
    umulh   \T3, \A1, \B2
    mul     \T4, \A1, \B4
    umulh   \T5, \A1, \B4
    mul     \T6, \A1, \B6
    umulh   \T7, \A1, \B6

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adcs    \C4, \C4, \T3
    adcs    \C5, \C5, \T4
    adcs    \C6, \C6, \T5
    adcs    \C7, \C7, \T6
    adcs    \C8, \C8, \T7
    adc     \C9, xzr, xzr

    mul     \T0, \A1, \B1
    umulh   \T1, \A1, \B1
    mul     \T2, \A1, \B3
    umulh   \T3, \A1, \B3
    mul     \T4, \A1, \B5
    umulh   \T5, \A1, \B5
    mul     \T6, \A1, \B7
    umulh   \T7, \A1, \B7

    adds    \C2, \C2, \T0
    adcs    \C3, \C3, \T1
    adcs    \C4, \C4, \T2
    adcs    \C5, \C5, \T3
    adcs    \C6, \C6, \T4
    adcs    \C7, \C7, \T5
    adcs    \C8, \C8, \T6
    adc     \C9, \C9, \T7


.endm



.macro mul_2x8_and_add  C0, C1, C2, C3, C4, C5, C6, C7, C8, C9,\
                        B0, B1, B2, B3, B4, B5, B6, B7, \
                        A0, A1, \
                        T0, T1, T2, T3, T4, T5, T6, T7, T8


    mul     \T0, \A0, \B0
    umulh   \T1, \A0, \B0
    mul     \T2, \A0, \B2
    umulh   \T3, \A0, \B2
    mul     \T4, \A0, \B4
    umulh   \T5, \A0, \B4
    mul     \T6, \A0, \B6
    umulh   \T7, \A0, \B6

    adds    \C0, \C0, \T0
    adcs    \C1, \C1, \T1
    adcs    \C2, \C2, \T2
    adcs    \C3, \C3, \T3
    adcs    \C4, \C4, \T4
    adcs    \C5, \C5, \T5
    adcs    \C6, \C6, \T6
    adcs    \C7, \C7, \T7
    adcs    \C8, \C8, xzr
    adc     \C9, \C9, xzr

    mul     \T0, \A0, \B1
    umulh   \T1, \A0, \B1
    mul     \T2, \A0, \B3
    umulh   \T3, \A0, \B3
    mul     \T4, \A0, \B5
    umulh   \T5, \A0, \B5
    mul     \T6, \A0, \B7
    umulh   \T7, \A0, \B7

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adcs    \C4, \C4, \T3
    adcs    \C5, \C5, \T4
    adcs    \C6, \C6, \T5
    adcs    \C7, \C7, \T6
    adcs    \C8, \C8, \T7
    adc     \C9, \C9, xzr


    mul     \T0, \A1, \B0
    umulh   \T1, \A1, \B0
    mul     \T2, \A1, \B2
    umulh   \T3, \A1, \B2
    mul     \T4, \A1, \B4
    umulh   \T5, \A1, \B4
    mul     \T6, \A1, \B6
    umulh   \T7, \A1, \B6

    adds    \C1, \C1, \T0
    adcs    \C2, \C2, \T1
    adcs    \C3, \C3, \T2
    adcs    \C4, \C4, \T3
    adcs    \C5, \C5, \T4
    adcs    \C6, \C6, \T5
    adcs    \C7, \C7, \T6
    adcs    \C8, \C8, \T7
    adc     \C9, \C9, xzr


    mul     \T0, \A1, \B1
    umulh   \T1, \A1, \B1
    mul     \T2, \A1, \B3
    umulh   \T3, \A1, \B3
    mul     \T4, \A1, \B5
    umulh   \T5, \A1, \B5
    mul     \T6, \A1, \B7
    umulh   \T7, \A1, \B7

    adds    \C2, \C2, \T0
    adcs    \C3, \C3, \T1
    adcs    \C4, \C4, \T2
    adcs    \C5, \C5, \T3
    adcs    \C6, \C6, \T4
    adcs    \C7, \C7, \T5
    adcs    \C8, \C8, \T6
    adc     \C9, \C9, \T7


.endm





.macro add_c0_mu_p      C0, C1, C2, C3, C4, C5, C6, C7, C8, C9, \
                        T0, T1, T2, T3, T4

    subs    \T0, xzr, \C0
    sbc     \T1, xzr, \C1

    lsl     \T3, \T1, #63
    lsl     \T2, \T0, #63
    lsr     \T4, \T1, #1
    eor     \T3, \T3, \T0, lsr #1

    adds    \C0, \C0, \T0
    adcs    \C1, \C1, \T1
    adcs    \C2, \C2, xzr
    adcs    \C3, \C3, xzr
    adcs    \C4, \C4, xzr
    adcs    \C5, \C5, xzr
    adcs    \C6, \C6, xzr
    adcs    \C7, \C7, \T2
    adcs    \C8, \C8, \T3
    adc     \C9, \C9, \T4

;   subs    \C0, \C0, xzr   // subtract with 0
;   sbcs    \C1, \C1, xzr
;   sbcs    \C2, \C2, xzr
;   sbcs    \C3, \C3, xzr
;   sbcs    \C4, \C4, xzr
    subs    \C5, \C5, \T0
    sbcs    \C6, \C6, \T1
    sbcs    \C7, \C7, xzr
    sbcs    \C8, \C8, xzr
    sbc     \C9, \C9, xzr

.endm



.macro interleave_step_0    C0, C1, C2, C3, C4, C5, C6, C7, C8, C9,\
                            B0, B1, B2, B3, B4, B5, B6, B7, \
                            A0, A1, \
                            T0, T1, T2, T3, T4, T5, T6, T7, T8


    mul_2x8_and_add_0   \C0, \C1, \C2, \C3, \C4, \C5, \C6, \C7, \C8, \C9,\
                        \B0, \B1, \B2, \B3, \B4, \B5, \B6, \B7, \
                        \A0, \A1, \
                        \T0, \T1, \T2, \T3, \T4, \T5, \T6, \T7, \T8

    add_c0_mu_p         \C0, \C1, \C2, \C3, \C4, \C5, \C6, \C7, \C8, \C9, \
                        \T0, \T1, \T2, \T3, \T4


.endm



.macro interleave_step  C0, C1, C2, C3, C4, C5, C6, C7, C8, C9,\
                        B0, B1, B2, B3, B4, B5, B6, B7, \
                        A0, A1, \
                        T0, T1, T2, T3, T4, T5, T6, T7, T8


    mul_2x8_and_add     \C0, \C1, \C2, \C3, \C4, \C5, \C6, \C7, \C8, \C9,\
                        \B0, \B1, \B2, \B3, \B4, \B5, \B6, \B7, \
                        \A0, \A1, \
                        \T0, \T1, \T2, \T3, \T4, \T5, \T6, \T7, \T8

    add_c0_mu_p         \C0, \C1, \C2, \C3, \C4, \C5, \C6, \C7, \C8, \C9, \
                        \T0, \T1, \T2, \T3, \T4


.endm


//***********************************************************************
//  Field multiplication
//  Input: a[x0] 1 word < p; b[x1] 1 word < p
//  Output: c[x2] 1 word < p
//  Operation: c [x2] = a [x0] * b [x1] mod p
//*********************************************************************** 
.global fmt(f_mul_asm)
fmt(f_mul_asm):


    /* save variables on stack */
    sub     sp,  sp,  #0x70
    stp     x19, x20, [sp, #0x00]
    stp     x21, x22, [sp, #0x10]
    stp     x23, x24, [sp, #0x20]
    stp     x25, x26, [sp, #0x30]
    stp     x27, x28, [sp, #0x40]
    stp     x29, x30, [sp, #0x50]
    stp     x1,  x2,  [sp, #0x60]


    ldp     x3,  x4,  [x1, #0x00]
    ldp     x5,  x6,  [x1, #0x10]
    ldp     x7,  x8,  [x1, #0x20]
    ldp     x9,  x10, [x1, #0x30]
    


    ldp     x1,  x2,  [x0, #0x00]

    interleave_step_0   x20, x21, x22, x23, x24, x25, x26, x27, x28, x29,   \
                        x3, x4, x5, x6, x7, x8, x9, x10,                    \
                        x1, x2,                                             \
                        x11, x12, x13, x14, x15, x16, x17, x19, x30


    ldp     x1, x2,   [x0, #0x10]

    interleave_step     x22, x23, x24, x25, x26, x27, x28, x29, x20, x21,   \
                        x3, x4, x5, x6, x7, x8, x9, x10,                    \
                        x1, x2,                                             \
                        x11, x12, x13, x14, x15, x16, x17, x19, x30



    ldp     x1, x2,   [x0, #0x20]

    interleave_step     x24, x25, x26, x27, x28, x29, x20, x21, x22, x23,   \
                        x3, x4, x5, x6, x7, x8, x9, x10,                    \
                        x1, x2,                                             \
                        x11, x12, x13, x14, x15, x16, x17, x19, x30



    ldp     x1, x2,   [x0, #0x30]

    interleave_step     x26, x27, x28, x29, x20, x21, x22, x23, x24, x25,   \
                        x3, x4, x5, x6, x7, x8, x9, x10,                    \
                        x1, x2,                                             \
                        x11, x12, x13, x14, x15, x16, x17, x19, x30



    mov     x11, #0x0000000000000001
    mov     x12, #0xFFFFFFFFFFFFFFFF
    mov     x13, #0x7FFFFFFFFFFFFFFF

    subs    x3,  x28, x11
    sbcs    x4,  x29, xzr
    sbcs    x5,  x20, xzr
    sbcs    x6,  x21, xzr
    sbcs    x7,  x22, xzr
    sbcs    x8,  x23, x12
    sbcs    x9,  x24, x12
    sbcs    x10, x25, x13

    sbc     x12, xzr, xzr
    and     x11, x11, x12
    and     x13, x13, x12

    adds    x3,  x3,  x11
    adcs    x4,  x4,  xzr
    adcs    x5,  x5,  xzr
    adcs    x6,  x6,  xzr
    adcs    x7,  x7,  xzr
    adcs    x8,  x8,  x12
    adcs    x9,  x9,  x12
    adc     x10, x10, x13



    /* restore stack */
    ldp     x19, x20, [sp, #0x00]
    ldp     x21, x22, [sp, #0x10]
    ldp     x23, x24, [sp, #0x20]
    ldp     x25, x26, [sp, #0x30]
    ldp     x27, x28, [sp, #0x40]
    ldp     x29, x30, [sp, #0x50]
    ldp     x1,  x2,  [sp, #0x60]
    add     sp,  sp,  #0x70



    stp     x3,  x4,  [x2, #0x00]
    stp     x5,  x6,  [x2, #0x10]
    stp     x7,  x8,  [x2, #0x20]
    stp     x9,  x10, [x2, #0x30]

    ret







#endif





