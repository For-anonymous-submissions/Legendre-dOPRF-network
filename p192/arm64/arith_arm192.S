// Format function and variable names for Mac OS X
#if defined(__APPLE__)
    #define fmt(f)    _##f
#else
    #define fmt(f)    f
#endif

.text
.align 3


#define ORIGINAL 0
#define ALT      1

#if (PRIMES == ORIGINAL)



// FIELD CONSTANTS
; static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFF13, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF}; // Field order p
; static const digit_t Mont_one[WORDS_FIELD]  = {0x00000000000000ED, 0x0000000000000000, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
; static const digit_t R2[WORDS_FIELD]        = {0x000000000000DB69, 0x0000000000000000, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
; static const digit_t iR[WORDS_FIELD]        = {0xDE83C7D4CB125C9E, 0xF1F532C497393FBA, 0x4CB125CE4FEEB7A0}; // iR =  R^(-1) (mod p)
; static const digit_t pp[WORDS_FIELD]        = {0xDE83C7D4CB125CE5, 0xF1F532C497393FBA, 0x4CB125CE4FEEB7A0}; // pp = -p^(-1) mod R
; static const digit_t ip[WORDS_FIELD]        = {0x217C382B34EDA31B, 0x0E0ACD3B68C6C045, 0xB34EDA31B011485F}; // ip =  p^(-1) mod R
; static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 0
; static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000}; // 1

// Field characterstics
p192:
.quad   0xFFFFFFFFFFFFFF13
.quad   0xFFFFFFFFFFFFFFFF
.quad   0xFFFFFFFFFFFFFFFF

// Montgomery one = R = 2^64 % p 
Rmp:
.quad   0x00000000000000ED
.quad   0x0000000000000000
.quad   0x0000000000000000

// R squared mod p
R2mp:
.quad   0x000000000000DB69
.quad   0x0000000000000000
.quad   0x0000000000000000

// Inverse of R mod p 
iRmp:
.quad   0xDE83C7D4CB125C9E
.quad   0xF1F532C497393FBA
.quad   0x4CB125CE4FEEB7A0

// Inverse of -p mod R
impmR:
.quad   0xDE83C7D4CB125CE5
.quad   0xF1F532C497393FBA
.quad   0x4CB125CE4FEEB7A0

// Inverse of p mod R
ipmR:
.quad   0x217C382B34EDA31B
.quad   0x0E0ACD3B68C6C045
.quad   0xB34EDA31B011485F

// Zero // Not actually used
Zero:
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000

// One
One:
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000000


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Modular reduction
;  Input: a[x0] 1 word < 2*p
;  Output: c[x1] 1 words < p
;  Operation: c [x1] =  a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.global fmt(f_red_asm)
fmt(f_red_asm):

    ldp     x2, x3, [x0, #00]
    ldr     x4,     [x0, #16]

    mov     x5, 0xFFFFFFFFFFFFFF13
    mov     x6, 0xFFFFFFFFFFFFFFFF

    subs    x2, x2, x5
    sbcs    x3, x3, x6
    sbcs    x4, x4, x6

    sbc     x7, xzr, xzr
    and     x5, x5, x7

    adds    x2, x2, x5
    adcs    x3, x3, x7
    adcs    x4, x4, x7

    stp     x2, x3, [x0, #00]
    str     x4,     [x0, #16]
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field addition
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] + b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.global fmt(f_add_asm)
fmt(f_add_asm):
    ldp     x3, x4, [x0, #00]
    ldr     x5,     [x0, #16]
    ldp     x6, x7, [x1, #00]
    ldr     x8,     [x1, #16]

    mov     x10, 0xFFFFFFFFFFFFFF13
    mov     x11, 0xFFFFFFFFFFFFFFFF

    adds    x3, x3, x6
    adcs    x4, x4, x7
    adcs    x5, x5, x8

    adc     x9, xzr, xzr
    sub     x9, xzr, x9

    and     x12, x10, x9

    subs    x3, x3, x12
    sbcs    x4, x4, x9
    sbc     x5, x5, x9

    subs    x3, x3, x10
    sbcs    x4, x4, x11
    sbcs    x5, x5, x11

    sbc     x9, xzr, xzr    

    and     x12, x10, x9

    adds    x3, x3, x12
    adcs    x4, x4, x9
    adc     x5, x5, x9

    stp     x3, x4, [x2, #00]
    str     x5,     [x2, #16]
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
    ldr     x4,     [x0, #16]

    mov     x6, #0xFFFFFFFFFFFFFF13
    mov     x7, #0xFFFFFFFFFFFFFFFF

    subs    x2, x6, x2
    sbcs    x3, x7, x3
    sbc     x4, x7, x4

    subs    x2, x2, x6
    sbcs    x3, x3, x7
    sbcs    x4, x4, x7

    sbc     x9, xzr, xzr

    and     x6, x6, x9

    adds    x2, x2, x6
    adcs    x3, x3, x9
    adc     x4, x4, x9

    stp     x2, x3, [x1, #00]
    str     x4,     [x1, #16]
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
    ldr     x5,      [x0, #16]
    ldp     x7, x8,  [x1, #00]
    ldr     x9,      [x1, #16]

    mov     x6, #0xFFFFFFFFFFFFFF13

    subs    x3, x3, x7
    sbcs    x4, x4, x8
    sbcs    x5, x5, x9

    sbc     x10, xzr, xzr

    and     x6, x6, x10

    adds    x3, x3, x6
    adcs    x4, x4, x10
    adc     x5, x5, x10

    stp     x3, x4, [x2, #00]
    str     x5,     [x2, #16]
    ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Multiprecision multiplication
;  Input: a[x0] 1 word; b[x1] 1 word
;  Output: c[x2] 2 words
;  Operation: c [x2] = a [x0] * b [x1]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mp_mul_asm)
fmt(mp_mul_asm):
    ldp     x3, x4,  [x0, #00]
    ldr     x5,      [x0, #16]
    ldp     x6, x7,  [x1, #00]
    ldr     x8,      [x1, #16]

    mul     x11, x3, x8
    umulh   x12, x3, x8

    mul     x13, x5, x8
    umulh   x14, x5, x8

    umulh   x9,  x3, x7
    mul     x10, x4, x8
    umulh   x15, x4, x8

    adds    x11, x11, x9
    adcs    x12, x12, x10
    adcs    x13, x13, x15
    adc     x14, x14, xzr

    mul     x9,  x4, x7
    umulh   x10, x4, x7
    umulh   x15, x5, x7

    adds    x11, x11, x9
    adcs    x12, x12, x10
    adcs    x13, x13, x15
    adc     x14, x14, xzr

    umulh   x10, x3, x6

    mul     x9,  x4, x6
    umulh   x15, x4, x6
    mul     x16, x5, x7

    adds    x10, x10, x9
    adcs    x11, x11, x15
    adcs    x12, x12, x16
    adcs    x13, x13, xzr
    adc     x14, x14, xzr

    mul     x9, x3, x6
    mul     x15, x3, x7
    mul     x16, x5, x6
    umulh   x17, x5, x6

    adds    x10, x10, x15
    adcs    x11, x11, x16
    adcs    x12, x12, x17
    adcs    x13, x13, xzr
    adc     x14, x14, xzr

    stp     x9,  x10, [x2,#00]
    stp     x11, x12, [x2,#16]
    stp     x13, x14, [x2,#32]
    ret





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Montgomery reduction
;  Input: a[x0] 2 words < p*R
;  Output: c[x1] 1 word < p
;  Operation: c[x1] = a [x0] * (R^(-1)) mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mont_redc_asm)
fmt(mont_redc_asm):

    ldp     x3, x4, [x0,#00]
    ldr     x5,     [x0,#16]


    ldr     x9,  ipmR
    ldr     x10, ipmR + 8
    ldr     x11, ipmR + 16

    mul     x6,  x3, x9
    mul     x7,  x3, x10
    umulh   x8,  x3, x10
    mul     x12, x4, x9
    umulh   x13, x4, x9

    adds    x7,  x7, x12
    adc     x8,  x8, x13

    umulh   x12, x3, x9
    mul     x13, x3, x11

    adds    x7,  x7, x12
    adc     x8,  x8, x13

    mul     x12, x4, x10
    mul     x13, x5, x9

    add     x8,  x8, x12
    add     x8,  x8, x13

    mov     x2,  #0x00000000000000ED     

    umulh   x3,  x6, x2
    mul     x4,  x8, x2
    umulh   x5,  x8, x2

    mul     x9,  x7, x2
    umulh   x10, x7, x2

    adds    x3,  x3, x9
    adcs    x4,  x4, x10
    adc     x5,  x5, xzr

    subs    x3,  xzr, x3
    sbcs    x4,  xzr, x4
    sbcs    x6,  x6,  x5
    sbcs    x7,  x7,  xzr
    sbc     x8,  x8,  xzr

    ldr     x9,         [x0, #24]
    ldp     x10, x11,   [x0, #32]

    mov     x2,  #0xFFFFFFFFFFFFFF13
    
    subs    x9,  x9,  x6
    sbcs    x10, x10, x7
    sbcs    x11, x11, x8

    sbc     x12, xzr, xzr

    and     x2,  x2,  x12

    adds    x9,  x9,  x2
    adcs    x10, x10, x12
    adc     x11, x11, x12

    stp     x9,  x10, [x1, #00]
    str     x11,      [x1, #16]
    ret






//***********************************************************************
//  Field multiplication
//  Input: a[x0] 1 word < p; b[x1] 1 word < p
//  Output: c[x2] 1 word < p
//  Operation: c [x2] = a [x0] * b [x1] mod p
//*********************************************************************** 
.global fmt(f_mul_asm)
fmt(f_mul_asm):
    ldp     x3, x4,  [x0, #00]
    ldr     x5,      [x0, #16]
    ldp     x6, x7,  [x1, #00]
    ldr     x8,      [x1, #16]

    mul     x11, x3, x8
    umulh   x12, x3, x8

    mul     x13, x5, x8
    umulh   x14, x5, x8

    umulh   x15,  x3, x7
    mul     x16, x4, x8
    umulh   x17, x4, x8

    adds    x11, x11, x15
    adcs    x12, x12, x16
    adcs    x13, x13, x17
    adc     x14, x14, xzr

    mul     x15,  x4, x7
    umulh   x16, x4, x7
    umulh   x17, x5, x7

    adds    x11, x11, x15
    adcs    x12, x12, x16
    adcs    x13, x13, x17
    adc     x14, x14, xzr

    mul     x9,  x3, x6
    umulh   x10, x3, x6

    mul     x15,  x4, x6
    umulh   x16, x4, x6
    mul     x17, x5, x7

    adds    x10, x10, x15
    adcs    x11, x11, x16
    adcs    x12, x12, x17
    adcs    x13, x13, xzr
    adc     x14, x14, xzr

    mul     x15, x3, x7
    mul     x16, x5, x6
    umulh   x17, x5, x6

    adds    x10, x10, x15
    adcs    x11, x11, x16
    adcs    x12, x12, x17
    adcs    x13, x13, xzr
    adc     x14, x14, xzr

    ldr     x3,  ipmR
    ldr     x4,  ipmR + 8
    ldr     x5,  ipmR + 16

    mul     x6,  x3, x9
    mul     x7,  x3, x10
    umulh   x8,  x3, x10

    mul     x15, x4, x9
    umulh   x16, x4, x9

    adds    x7,  x7, x15
    adc     x8,  x8, x16

    umulh   x15, x3, x9
    mul     x16, x3, x11

    adds    x7,  x7, x15
    adc     x8,  x8, x16

    mul     x15, x4, x10
    mul     x16, x5, x9

    add     x8,  x8, x15
    add     x8,  x8, x16

    mov     x17,  #0x00000000000000ED

    mul     x11, x6, x17
    umulh   x3,  x6, x17
    mul     x4,  x8, x17
    umulh   x5,  x8, x17

    mul     x9,  x7, x17
    umulh   x10, x7, x17

    adds    x3,  x3, x9
    adcs    x4,  x4, x10
    adc     x5,  x5, xzr

    subs    x11, xzr, x11
    sbcs    x3,  xzr, x3
    sbcs    x4,  xzr, x4
    sbcs    x6,  x6,  x5
    sbcs    x7,  x7,  xzr
    sbc     x8,  x8,  xzr

    mov     x17,  #0xFFFFFFFFFFFFFF13
    
    subs    x12, x12, x6
    sbcs    x13, x13, x7
    sbcs    x14, x14, x8

    sbc     x16, xzr, xzr

    and     x17, x17, x16

    adds    x12, x12, x17
    adcs    x13, x13, x16
    adc     x14, x14, x16

    stp     x12,  x13,  [x2, #00]
    str     x14,        [x2, #16]
    ret


    ; ldr     x17, impmR

    ; // a0 * B
    ; mul     x9,  x6, x3
    ; umulh   x10, x6, x3
    ; mul     x11, x8, x3
    ; umulh   x12, x8, x3

    ; mul     x13, x7, x3
    ; umulh   x14, x7, x3

    ; adds    x10, x10, x13
    ; adcs    x11, x11, x14
    ; adc     x12, x12, xzr

    ; // ((a0 * B)_R * (-p^-1))_R
    ; mul     x16, x17, x9

    ; // x14 * p
    ; mov     x3,  #0x00000000000000ED

    ; mul     x14, x16, x3
    ; umulh   x15, x16, x3

    ; subs    x9,  x9,  x14
    ; sbcs    x10, x10, x15
    ; sbcs    x11, x11, xzr
    ; sbcs    x12, x12, xzr
    ; sbc     x13, xzr, xzr

    ; adds    x12, x12, x16
    ; adc     x13, x13, xzr



    ; // a1 * B
    ; umulh   x15, x6,  x4
    ; mul     x14, x7,  x4
    ; umulh   x16, x7,  x4

    ; adds    x15, x15, x14
    ; adc     x16, x16, xzr

    ; mul     x14, x8,  x4
    ; umulh   x3,  x8,  x4

    ; adds    x16, x16, x14
    ; adc     x3,  x3, xzr

    ; mul     x14, x6, x4


    ; adds    x10, x10, x14
    ; adcs    x11, x11, x15
    ; adcs    x12, x12, x16
    ; adcs    x13, x13, x3
    ; adc     x9,  xzr, xzr

    ; mul     x16, x17, x10
    ; mov     x3,  #0x00000000000000ED

    ; mul     x14, x16, x3
    ; umulh   x15, x16, x3

    ; subs    x10, x10, x14
    ; sbcs    x11, x11, x15
    ; sbcs    x12, x12, xzr
    ; sbcs    x13, x13, xzr
    ; sbc     x9,  x9,  xzr

    ; adds    x13, x13, x16
    ; adc     x9,  x9,  xzr




    ; // a2 * B
    ; mul     x14, x6, x5
    ; umulh   x15, x6, x5
    ; mul     x16, x8, x5
    ; umulh   x3,  x8, x5

    ; mul     x4,  x7, x5
    ; umulh   x6,  x7, x5

    ; adds    x15, x15, x4
    ; adcs    x16, x16, x6
    ; adc     x3,  x3,  xzr

    ; adds    x11, x11, x14
    ; adcs    x12, x12, x15
    ; adcs    x13, x13, x16
    ; adcs    x9,  x9,  x3
    ; adc     x10, xzr, xzr

    ; mul     x16, x17, x11
    ; mov     x3,  #0x00000000000000ED

    ; mul     x14, x16, x3
    ; umulh   x15, x16, x3

    ; subs    x11, x11, x14
    ; sbcs    x12, x12, x15
    ; sbcs    x13, x13, xzr
    ; sbcs    x9,  x9,  xzr
    ; sbc     x10, x10, xzr

    ; adds    x9,  x9,  x16
    ; adc     x10, x10, xzr



    ; mov     x3,  #0xFFFFFFFFFFFFFF13
    ; sub     x10, xzr, x10

    ; and     x3,  x3,  x10

    ; subs    x12, x12, x3
    ; sbcs    x13, x13, x10
    ; sbc     x9,  x9,  x10

    ; mov     x3,  #0xFFFFFFFFFFFFFF13
    ; mov     x10, #0xFFFFFFFFFFFFFFFF

    ; subs    x12, x12, x3
    ; sbcs    x13, x13, x10
    ; sbcs    x9,  x9,  x10

    ; sbc     x11, xzr, xzr
    ; and     x3,  x3,  x11

    ; adds    x12, x12, x3
    ; adcs    x13, x13, x11
    ; adc     x9,  x9,  x11

    ; stp     x12, x13, [x2, #00]
    ; str     x9,       [x2, #16]
    
    ; ret

















#elif (PRIMES == ALT)











// FIELD CONSTANTS
; static const digit_t p[WORDS_FIELD]         = {0xFFFFFFFFFFFFFFED, 0xFFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF}; // Field order p
; static const digit_t Mont_one[WORDS_FIELD]  = {0x0000000000000026, 0x0000000000000000, 0x0000000000000000}; // R  =  2^{NBITS_PRIME} (mod p)
; static const digit_t R2[WORDS_FIELD]        = {0x00000000000005A4, 0x0000000000000000, 0x0000000000000000}; // R2 = (2^{NBITS_PRIME})^2 (mod p)
; static const digit_t iR[WORDS_FIELD]        = {0x435E50D79435E50B, 0x5E50D79435E50D79, 0x10D79435E50D7943}; // iR =  R^(-1) (mod p)
; static const digit_t pp[WORDS_FIELD]        = {0x86BCA1AF286BCA1B, 0xBCA1AF286BCA1AF2, 0x21AF286BCA1AF286}; // pp = -p^(-1) mod R
; static const digit_t ip[WORDS_FIELD]        = {0x79435E50D79435E5, 0x435E50D79435E50D, 0xDE50D79435E50D79}; // ip =  p^(-1) mod R
; static const digit_t Zero[WORDS_FIELD]      = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000}; // 0
; static const digit_t One[WORDS_FIELD]       = {0x0000000000000001, 0x0000000000000000, 0x0000000000000000}; // 1


// Field characterstics
p192:
.quad   0xFFFFFFFFFFFFFFED
.quad   0xFFFFFFFFFFFFFFFF
.quad   0x7FFFFFFFFFFFFFFF

// Montgomery one = R = 2^192 % p 
Rmp:
.quad   0x0000000000000026
.quad   0x0000000000000000
.quad   0x0000000000000000

// R squared mod p
R2mp:
.quad   0x00000000000005A4
.quad   0x0000000000000000
.quad   0x0000000000000000

// Inverse of R mod p 
iRmp:
.quad   0x435E50D79435E50B
.quad   0x5E50D79435E50D79
.quad   0x10D79435E50D7943

// Inverse of -p mod R
impmR:
.quad   0x86BCA1AF286BCA1B
.quad   0xBCA1AF286BCA1AF2
.quad   0x21AF286BCA1AF286

// Inverse of p mod R
ipmR:
.quad   0x79435E50D79435E5
.quad   0x435E50D79435E50D
.quad   0xDE50D79435E50D79

// Zero // Not actually used
Zero:
.quad   0x0000000000000000
.quad   0x0000000000000000
.quad   0x0000000000000000

// One
One:
.quad   0x0000000000000001
.quad   0x0000000000000000
.quad   0x0000000000000000





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Modular reduction
;  Input: a[x0] 1 word < 2*p
;  Output: c[x1] 1 words < p
;  Operation: c [x1] =  a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.global fmt(f_red_asm)
fmt(f_red_asm):

    ldp     x2,  x3, [x0, #00]
    ldr     x4,      [x0, #16]

    mov     x6,  #0xFFFFFFFFFFFFFFED
    mov     x7,  #0xFFFFFFFFFFFFFFFF
    mov     x8,  #0x7FFFFFFFFFFFFFFF
    mov     x11, #0x0000000000000013

    lsr     x10, x4,  #63
    sub     x10, xzr, x10
    and     x10, x10, x11

    and     x4,  x4,  x8

    adds    x2,  x2,  x10
    adcs    x3,  x3,  xzr
    adc     x4,  x4,  xzr


    subs    x2,  x2,  x6
    sbcs    x3,  x3,  x7
    sbcs    x4,  x4,  x8

    sbc     x7,  xzr, xzr

    and     x6,  x6,  x7
    and     x8,  x8,  x7

    adds    x2,  x2,  x6
    adcs    x3,  x3,  x7
    adcs    x4,  x4,  x8

    stp     x2,  x3,  [x0, #00]
    str     x4,       [x0, #16]
    ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field addition
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] + b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.global fmt(f_add_asm)
fmt(f_add_asm):
    ldp     x3, x4, [x0, #00]
    ldr     x5,     [x0, #16]
    ldp     x6, x7, [x1, #00]
    ldr     x8,     [x1, #16]

    mov     x10, #0xFFFFFFFFFFFFFFED
    mov     x11, #0xFFFFFFFFFFFFFFFF
    mov     x12, #0x7FFFFFFFFFFFFFFF

    adds    x3, x3, x6
    adcs    x4, x4, x7
    adc     x5, x5, x8

    subs    x3, x3, x10
    sbcs    x4, x4, x11
    sbcs    x5, x5, x12

    sbc     x11, xzr, xzr    

    and     x10, x10, x11
    and     x12, x12, x11

    adds    x3, x3, x10
    adcs    x4, x4, x11
    adc     x5, x5, x12

    stp     x3, x4, [x2, #00]
    str     x5,     [x2, #16]
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
    ldr     x4,     [x0, #16]

    mov     x5, #0xFFFFFFFFFFFFFFED
    mov     x6, #0xFFFFFFFFFFFFFFFF
    mov     x7, #0x7FFFFFFFFFFFFFFF

    subs    x2, x5, x2
    sbcs    x3, x6, x3
    sbc     x4, x7, x4

    subs    x2, x2, x5
    sbcs    x3, x3, x6
    sbcs    x4, x4, x7

    sbc     x6, xzr, xzr

    and     x5, x5, x6
    and     x7, x7, x6

    adds    x2, x2, x5
    adcs    x3, x3, x6
    adc     x4, x4, x7

    stp     x2, x3, [x1, #00]
    str     x4,     [x1, #16]
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
    ldr     x5,      [x0, #16]
    ldp     x7, x8,  [x1, #00]
    ldr     x9,      [x1, #16]

    mov     x10, #0xFFFFFFFFFFFFFFED
    mov     x12, #0x7FFFFFFFFFFFFFFF

    subs    x3, x3, x7
    sbcs    x4, x4, x8
    sbcs    x5, x5, x9

    sbc     x11, xzr, xzr

    and     x10, x10, x11
    and     x12, x12, x11

    adds    x3, x3, x10
    adcs    x4, x4, x11
    adc     x5, x5, x12

    stp     x3, x4, [x2, #00]
    str     x5,     [x2, #16]
    ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Multiprecision multiplication
;  Input: a[x0] 1 word; b[x1] 1 word
;  Output: c[x2] 2 words
;  Operation: c [x2] = a [x0] * b [x1]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mp_mul_asm)
fmt(mp_mul_asm):
    ldp     x3, x4,  [x0, #00]
    ldr     x5,      [x0, #16]
    ldp     x6, x7,  [x1, #00]
    ldr     x8,      [x1, #16]

    mul     x11, x3, x8
    umulh   x12, x3, x8

    mul     x13, x5, x8
    umulh   x14, x5, x8

    umulh   x9,  x3, x7
    mul     x10, x4, x8
    umulh   x15, x4, x8

    adds    x11, x11, x9
    adcs    x12, x12, x10
    adcs    x13, x13, x15
    adc     x14, x14, xzr

    mul     x9,  x4, x7
    umulh   x10, x4, x7
    umulh   x15, x5, x7

    adds    x11, x11, x9
    adcs    x12, x12, x10
    adcs    x13, x13, x15
    adc     x14, x14, xzr

    umulh   x10, x3, x6

    mul     x9,  x4, x6
    umulh   x15, x4, x6
    mul     x16, x5, x7

    adds    x10, x10, x9
    adcs    x11, x11, x15
    adcs    x12, x12, x16
    adcs    x13, x13, xzr
    adc     x14, x14, xzr

    mul     x9, x3, x6
    mul     x15, x3, x7
    mul     x16, x5, x6
    umulh   x17, x5, x6

    adds    x10, x10, x15
    adcs    x11, x11, x16
    adcs    x12, x12, x17
    adcs    x13, x13, xzr
    adc     x14, x14, xzr

    stp     x9,  x10, [x2,#00]
    stp     x11, x12, [x2,#16]
    stp     x13, x14, [x2,#32]
    ret





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Montgomery reduction
;  Input: a[x0] 2 words < p*R
;  Output: c[x1] 1 word < p
;  Operation: c[x1] = a [x0] * (R^(-1)) mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mont_redc_asm)
fmt(mont_redc_asm):

    ldp     x3,  x4,    [x0, #00]
    ldp     x5,  x15,   [x0, #16]
    ldp     x16, x17,   [x0, #32]


    ldr     x9,  ipmR
    ldr     x10, ipmR + 8
    ldr     x11, ipmR + 16

    mul     x6,  x3, x9
    mul     x7,  x3, x10
    umulh   x8,  x3, x10
    mul     x12, x4, x9
    umulh   x13, x4, x9

    adds    x7,  x7, x12
    adc     x8,  x8, x13

    umulh   x12, x3, x9
    mul     x13, x3, x11

    adds    x7,  x7, x12
    adc     x8,  x8, x13

    mul     x12, x4, x10
    mul     x13, x5, x9

    add     x8,  x8, x12
    add     x8,  x8, x13

    mov     x2,  #0x0000000000000013     

    umulh   x3,  x6, x2
    mul     x4,  x8, x2
    umulh   x5,  x8, x2

    mul     x9,  x7, x2
    umulh   x10, x7, x2

    adds    x3,  x3, x9
    adcs    x4,  x4, x10
    adc     x5,  x5, xzr

    lsl     x10,  x6, #63
    lsl     x11,  x7, #63
    lsl     x12,  x8, #63

    eor     x6, x11, x6, lsr #1
    eor     x7, x12, x7, lsr #1
    lsr     x8, x8,  #1

    subs    x3,  xzr, x3
    sbcs    x4,  x10, x4
    sbcs    x6,  x6,  x5
    sbcs    x7,  x7,  xzr
    sbc     x8,  x8,  xzr


    mov     x2,  #0xFFFFFFFFFFFFFFED
    mov     x4,  #0x7FFFFFFFFFFFFFFF

    subs    x15, x15, x6
    sbcs    x16, x16, x7
    sbcs    x17, x17, x8

    sbc     x3, xzr, xzr

    and     x2,  x2,  x3
    and     x4,  x4,  x3

    adds    x15, x15, x2
    adcs    x16, x16, x3
    adc     x17, x17, x4

    stp     x15, x16, [x1, #00]
    str     x17,      [x1, #16]
    ret






//***********************************************************************
//  Field multiplication
//  Input: a[x0] 1 word < p; b[x1] 1 word < p
//  Output: c[x2] 1 word < p
//  Operation: c [x2] = a [x0] * b [x1] mod p
//*********************************************************************** 
.global fmt(f_mul_asm)
fmt(f_mul_asm):
    ldp     x3, x4,  [x0, #00]
    ldr     x5,      [x0, #16]
    ldp     x6, x7,  [x1, #00]
    ldr     x8,      [x1, #16]


    mul     x11, x3, x8
    umulh   x12, x3, x8

    mul     x13, x5, x8
    umulh   x14, x5, x8

    umulh   x9,  x3, x7
    mul     x10, x4, x8
    umulh   x15, x4, x8

    adds    x11, x11, x9
    adcs    x12, x12, x10
    adcs    x13, x13, x15
    adc     x14, x14, xzr

    mul     x9,  x4, x7
    umulh   x10, x4, x7
    umulh   x15, x5, x7

    adds    x11, x11, x9
    adcs    x12, x12, x10
    adcs    x13, x13, x15
    adc     x14, x14, xzr

    umulh   x10, x3, x6

    mul     x9,  x4, x6
    umulh   x15, x4, x6
    mul     x16, x5, x7

    adds    x10, x10, x9
    adcs    x11, x11, x15
    adcs    x12, x12, x16
    adcs    x13, x13, xzr
    adc     x14, x14, xzr

    mul     x9, x3, x6
    mul     x15, x3, x7
    mul     x16, x5, x6
    umulh   x17, x5, x6

    adds    x10, x10, x15
    adcs    x11, x11, x16
    adcs    x12, x12, x17
    adcs    x13, x13, xzr
    adc     x14, x14, xzr

    // x9-x14 is a*b




    ldr     x3,  ipmR
    ldr     x4,  ipmR + 8
    ldr     x5,  ipmR + 16

    mul     x6,  x3, x9
    mul     x7,  x3, x10
    umulh   x8,  x3, x10
    mul     x15, x4, x9
    umulh   x16, x4, x9

    adds    x7,  x7, x15
    adc     x8,  x8, x16

    umulh   x15, x3, x9
    mul     x16, x3, x11

    adds    x7,  x7, x15
    adc     x8,  x8, x16

    mul     x15, x4, x10
    mul     x16, x5, x9

    add     x8,  x8, x15
    add     x8,  x8, x16

    mov     x17, #0x0000000000000013     

    mul     x11, x6, x17
    umulh   x3,  x6, x17
    mul     x4,  x8, x17
    umulh   x5,  x8, x17

    mul     x9,  x7, x17
    umulh   x10, x7, x17

    adds    x3,  x3, x9
    adcs    x4,  x4, x10
    adc     x5,  x5, xzr

    lsl     x15,  x6, #63
    lsl     x16,  x7, #63
    lsl     x17,  x8, #63

    eor     x6, x16, x6, lsr #1
    eor     x7, x17, x7, lsr #1
    lsr     x8, x8,  #1

    subs    x11, xzr, x11
    sbcs    x3,  xzr, x3
    sbcs    x4,  x15, x4
    sbcs    x6,  x6,  x5
    sbcs    x7,  x7,  xzr
    sbc     x8,  x8,  xzr


    mov     x15,  #0xFFFFFFFFFFFFFFED
    mov     x17,  #0x7FFFFFFFFFFFFFFF

    subs    x12, x12, x6
    sbcs    x13, x13, x7
    sbcs    x14, x14, x8

    sbc     x16, xzr, xzr

    and     x15,  x15,  x16
    and     x17,  x17,  x16

    adds    x12, x12, x15
    adcs    x13, x13, x16
    adc     x14, x14, x17

    stp     x12,  x13,  [x2, #00]
    str     x14,        [x2, #16]
    ret



    ; ldr     x17, impmR

    ; // a0 * B
    ; mul     x9,  x6, x3
    ; umulh   x10, x6, x3
    ; mul     x11, x8, x3
    ; umulh   x12, x8, x3

    ; mul     x13, x7, x3
    ; umulh   x14, x7, x3

    ; adds    x10, x10, x13
    ; adcs    x11, x11, x14
    ; adc     x12, x12, xzr


    ; mul     x14, x17, x9

    ; mov     x3,  #0x0000000000000013

    ; ; mul     x15, x14, x3  //cancels out, not needed
    ; umulh   x16, x14, x3

    ; ; subs    x9,  x9,  x15
    ; subs    x10, x10, x16
    ; sbcs    x11, x11, xzr
    ; sbc     x12, x12, xzr

    ; lsr     x15, x14, #1

    ; adds    x11, x11, x14, lsl #63
    ; adc     x12, x12, x15



    ; // a1 * B
    ; mul     x13, x6, x4
    ; umulh   x14, x6, x4
    ; mul     x15, x8, x4
    ; umulh   x16, x8, x4

    ; mul     x3,  x7, x4
    ; umulh   x4,  x7, x4

    ; adds    x14, x14, x3
    ; adcs    x15, x15, x4
    ; adc     x16, x16, xzr

    ; adds    x10, x10, x13
    ; adcs    x11, x11, x14
    ; adcs    x12, x12, x15
    ; adc     x9,  xzr, x16


    ; mul     x14, x17, x10

    ; mov     x3,  #0x0000000000000013

    ; ; mul     x15, x14, x3  //cancels out, not needed
    ; umulh   x16, x14, x3

    ; ; subs    x10, x10, x15
    ; subs    x11, x11, x16
    ; sbcs    x12, x12, xzr
    ; sbc     x9,  x9,  xzr

    ; lsr     x15, x14, #1

    ; adds    x12, x12, x14, lsl #63
    ; adc     x9,  x9,  x15




    ; // a2 * B
    ; mul     x13, x6, x5
    ; umulh   x14, x6, x5
    ; mul     x15, x8, x5
    ; umulh   x16, x8, x5

    ; mul     x3,  x7, x5
    ; umulh   x4,  x7, x5

    ; adds    x14, x14, x3
    ; adcs    x15, x15, x4
    ; adc     x16, x16, xzr

    ; adds    x11, x11, x13
    ; adcs    x12, x12, x14
    ; adcs    x9,  x9,  x15
    ; adc     x10, xzr, x16

    ; mul     x14, x17, x11

    ; mov     x3,  #0x0000000000000013

    ; ; mul     x15, x14, x3  //cancels out, not needed
    ; umulh   x16, x14, x3

    ; ; subs    x11, x11, x15
    ; subs    x12, x12, x16
    ; sbcs    x9,  x9,  xzr
    ; sbc     x10, x10, xzr

    ; lsr     x15, x14, #1

    ; adds    x9,  x9,  x14, lsl #63
    ; adc     x10, x10, x15





    ; // mod p reduction
    ; mov     x6,  #0xFFFFFFFFFFFFFFED
    ; mov     x7,  #0xFFFFFFFFFFFFFFFF
    ; mov     x8,  #0x7FFFFFFFFFFFFFFF
    ; mov     x11, #0x0000000000000013

    ; subs    x12,  x12,  x6
    ; sbcs    x9,   x9,   x7
    ; sbcs    x10,  x10,  x8

    ; sbc     x7,  xzr, xzr

    ; and     x6,  x6,  x7
    ; and     x8,  x8,  x7

    ; adds    x12,  x12,  x6
    ; adcs    x9,   x9,   x7
    ; adcs    x10,  x10,  x8


    ; stp     x12, x9,  [x2, #00]
    ; str     x10,      [x2, #16]
    
    ; ret









#endif