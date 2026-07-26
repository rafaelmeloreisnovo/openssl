/* Copyright 2026 Rafael Melo Reis. See repository LICENSE.txt. */
.syntax unified
.text
.section .rmr.text,"ax",%progbits
.p2align 4
.global rmr_mix32_arch
.hidden rmr_mix32_arch
.type rmr_mix32_arch,%function
rmr_mix32_arch:
    ror r1, r0, #19      /* rol32(x, 13) */
    ror r2, r0, #7
    eor r1, r1, r0
    eor r1, r1, r2
    movw r2, #0x79b9
    movt r2, #0x9e37
    eor r0, r1, r2
    bx lr
.size rmr_mix32_arch, .-rmr_mix32_arch
