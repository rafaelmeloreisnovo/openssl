/* Copyright 2026 Rafael Melo Reis. See repository LICENSE.txt. */
.text
.section .rmr.text,"ax",%progbits
.p2align 5
.global rmr_mix32_arch
.hidden rmr_mix32_arch
.type rmr_mix32_arch,%function
rmr_mix32_arch:
    ror w1, w0, #19      /* rol32(x, 13) */
    ror w2, w0, #7
    eor w1, w1, w0
    eor w1, w1, w2
    movz w2, #0x79b9
    movk w2, #0x9e37, lsl #16
    eor w0, w1, w2
    ret
.size rmr_mix32_arch, .-rmr_mix32_arch
