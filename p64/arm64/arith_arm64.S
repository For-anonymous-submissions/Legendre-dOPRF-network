; Format function and variable names for Mac OS X
#if defined(__APPLE__)
    #define fmt(f)    _##f
#else
    #define fmt(f)    f
#endif

.text
.align 2



; FIELD CONSTANTS

; Field characterstics
p64:
.quad   0xFFFFFFFFFFFFFFC5

; Montgomery one = R = 2^64 % p 
Rmp:
.quad   0x000000000000003B

; R squared mod p
R2mp:
.quad   0x0000000000000D99

; Inverse of R mod p 
iRmp:
.quad   0xCBEEA4E1A08AD8C4

; Inverse of -p mod R
impmR:
.quad   0xCBEEA4E1A08AD8F3

; Inverse of p mod R
ipmR:
.quad   0x34115B1E5F75270D

; Zero ; Not actually used
Zero:
.quad   0x0000000000000000

; One
One:
.quad   0x0000000000000001



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Modular reduction
;  Input: a[x0] 1 word < R
;  Output: c[x1] 1 words < p
;  Operation: c [x1] =  a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_red_asm)
fmt(f_red_asm):
    ldr     x2, [x0]
    mov     x4, #-0x3B      // p

    subs    x2, x2, x4

    sbc     x3, xzr, xzr
    and     x3, x3, x4

    add     x2, x2, x3

    str     x2, [x0]
    ret

    ; Alternative algorithm
    ; ldr     x2, [x0]
    ; mov     x4, 0x3B        // Rmp

    ; adds    x2, x2, x4

    ; sbc     x3, xzr, xzr
    ; and     x3, x3, x4

    ; sub     x2, x2, x3

    ; str     x2, [x0]
    ; ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field addition
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] + b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_add_asm)
fmt(f_add_asm):
    ldr     x3, [x0]
    ldr     x4, [x1]
    mov     x5, 0x3B        // Rmp

    adds    x3, x3, x4

    adc     x6, xzr, xzr
    sub     x6, xzr, x6
    and     x6, x6, x5

    add     x3, x3, x6

    adds    x3, x3, x5

    sbc     x6, xzr, xzr
    and     x6, x6, x5
    sub     x3, x3, x6

    str     x3, [x2]
    ret

    ; Alternative algorithm
    ; ldr     x3, [x0]
    ; ldr     x4, [x1]
    ; mov     x5, #-0x3B        // p

    ; adds    x3, x3, x4

    ; adc     x6, xzr, xzr
    ; sub     x6, xzr, x6
    ; and     x6, x6, x5

    ; sub     x3, x3, x6

    ; subs    x3, x3, x5

    ; sbc     x6, xzr, xzr
    ; and     x6, x6, x5
    ; add     x3, x3, x6

    ; str     x3, [x2]
    ; ret






;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field negation
;  Input: a[x0] 1 word < p
;  Output: c[x1] 1 words
;  Operation: c [x1] =  -a [x0] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_neg_asm)
fmt(f_neg_asm):

    ldr     x8, [x0]
    mov     x11, #-0x3B         // p

    sub     x8, x11, x8
    subs    x8, x8, x11

    sbc     x9, xzr, xzr
    and     x9, x9, x11

    add     x8, x8, x9

    str     x8, [x1]
    ret

    ; Alternative algorithm
    ; Doesn't work with a = p (which is never an input, but still, above works with a=p)
    ; ldr     x2, [x0]
    ; mov     x3, #0x3B         // Rmp

    ; add     x2, x2, x3

    ; subs    x2, x3, x2

    ; sbc     x4, xzr, xzr
    ; and     x3, x3, x4

    ; sub     x2, x2, x3

    ; str     x2, [x1]
    ; ret




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field subtraction
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 words
;  Operation: c [x2] = a [x0] - b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_sub_asm)
fmt(f_sub_asm):
    ldr     x3, [x0]
    ldr     x4, [x1]
    mov     x5, #0x3B       // Rmp

    subs    x3, x3, x4

    sbc     x6, xzr, xzr
    and     x6, x6, x5

    sub     x3, x3, x6

    str     x3, [x2]
    ret

    ; Alternative algorithm
    ; ldr     x8, [x0]
    ; ldr     x9, [x1]
    ; mov     x11, #-0x3B       // p

    ; subs    x8, x8, x9

    ; sbc     x10, xzr, xzr
    ; and     x10, x10, x11

    ; add     x8, x8, x10

    ; str     x8, [x2]
    ; ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Multiprecision multiplication
;  Input: a[x0] 1 word; b[x1] 1 word
;  Output: c[x2] 2 words
;  Operation: c [x2] = a [x0] * b [x1]
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mp_mul_asm)
fmt(mp_mul_asm):
    ldr     x3, [x0]
    ldr     x4, [x1]
    mul     x5, x3, x4
    umulh   x6, x3, x4
    stp     x5, x6, [x2]
    ret





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Montgomery reduction
;  Input: a[x0] 2 words < p*R
;  Output: c[x1] 1 word < p
;  Operation: c[x1] = a [x0] * (R^(-1)) mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(mont_redc_asm)
fmt(mont_redc_asm):

    ldp     x3, x4, [x0]
    ldr     x8, ipmR

    mov     x5, #-0x3B        // p

    mul     x6, x3, x8
    umulh   x7, x6, x5

    subs    x4, x4, x7

    sbc     x7, xzr, xzr 
    and     x7, x7, x5

    add     x4, x4, x7
    str     x4, [x1]

    ret

    ; ldp     x3, x4, [x0]
    ; ldr     x8, ipmR

    ; mov     x5, #0x3B         // Rmp

    ; mul     x6, x3, x8
    ; umulh   x7, x6, x5

    ; subs    x4, x4, x7

    ; sbc     x7, xzr, xzr 
    ; and     x7, x7, x5

    ; sub     x4, x4, x7
    ; str     x4, [x1]

    ; ret




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;  Field multiplication
;  Input: a[x0] 1 word < p; b[x1] 1 word < p
;  Output: c[x2] 1 word < p
;  Operation: c [x2] = a [x0] * b [x1] mod p
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; 
.global fmt(f_mul_asm)
fmt(f_mul_asm):
    ldr     x0, [x0]
    ldr     x1, [x1]

    mul     x3, x0, x1
    umulh   x4, x0, x1

    ldr     x8, ipmR
    mov     x5, #-0x3B        // p

    mul     x6, x3, x8
    umulh   x7, x6, x5

    subs    x4, x4, x7

    sbc     x7, xzr, xzr 
    and     x7, x7, x5

    add     x4, x4, x7
    str     x4, [x2]

    ret


    ; ldr     x8, [x0]
    ; ldr     x9, [x1]

    ; umulh   x3, x9, x8
    ; mul     x4, x9, x8

    ; mov     x9, #9997
    ; movk    x9, #24437, lsl #16
    ; movk    x9, #23326, lsl #32
    ; movk    x9, #13329, lsl #48

    ; mul     x4, x4, x9

    ; mov     x9, #-59

    ; umulh   x4, x4, x9

    ; subs    x4, x3, x4
    ; sbc     x3, xzr, xzr

    ; and     x9, x9, x3, asr #63

    ; add     x4, x9, x4

    ; str     x4, [x2]
    ; ret

    


    ; ldr     x0, [x0]
    ; ldr     x1, [x1]
    ; ldr     x8, ipmR

    ; mul     x3, x0, x1
    ; umulh   x4, x0, x1

    ; mov     x5, 0x3B      // Rmp

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







