// Format function and variable names for Mac OS X
#if defined(__APPLE__)
    #define fmt(f)    _##f
#else
    #define fmt(f)    f
#endif

.text
.align 2



// FIELD CONSTANTS

// Field characterstics
p64:
.quad   0xFFFFFFFFFFFFFFC5

// Montgomery one = R = 2^64 % p 
Rmp:
.quad   0x000000000000003B

// R squared mod p
R2mp:
.quad   0x0000000000000D99

// Inverse of R mod p 
iRmp:
.quad   0xCBEEA4E1A08AD8C4

// Inverse of -p mod R
impmR:
.quad   0xCBEEA4E1A08AD8F3

// Inverse of p mod R
ipmR:
.quad   0x34115B1E5F75270D

// Zero // Not actually used
Zero:
.quad   0x0000000000000000

// One
One:
.quad   0x0000000000000001



//***********************************************************************
//  Multiprecision multiplication
//  Input: a[x0] 1 word; b[x1] 1 word
//  Output: c[x2] 2 words
//  Operation: c [x2] = a [x0] * b [x1]
//*********************************************************************** 
.global fmt(mp_mul_asm)
fmt(mp_mul_asm):
    ldr     x8, [x0]
    ldr     x9, [x1]
    mul     x10, x8, x9
    umulh   x8, x8, x9
    stp     x10, x8, [x2]
    ret


//***********************************************************************
//  Field multiplication
//  Input: a[x0] 1 word < p; b[x1] 1 word < p
//  Output: c[x2] 1 word < p
//  Operation: c [x2] = a [x0] * b [x1] mod p
//*********************************************************************** 
.global fmt(f_mul_asm)
fmt(f_mul_asm):
    ldr     x8, [x0]
    ldr     x9, [x1]
    ldr     x13, ipmR
    // Multiply a * b
    mul     x10, x8, x9
    umulh   x11, x8, x9

    ldr     x14, p64

    // Multiply ((a * b) % R)  *  (p^(-1) % R)   % R  =: N
    mul     x12, x10, x13

    // Multiply N * p
    umulh   x15, x12, x14


    // Subtract (a * b) - (N * p)
    subs    x15, x11, x15

    sbc     x16, xzr, xzr 

    // Set x16 = p & mask_carryflag
    and     x16, x16, x14


    add     x8, x15, x16
    str     x8, [x2]
    
    ret










