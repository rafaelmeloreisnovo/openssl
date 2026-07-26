/* Copyright 2026 Rafael Melo Reis. See repository LICENSE.txt. */
.text
.section .rmr.text,"ax",@progbits
.p2align 5
.globl rmr_mix32_arch
.hidden rmr_mix32_arch
.type rmr_mix32_arch,@function
rmr_mix32_arch:
    movl %edi, %eax
    movl %edi, %ecx
    roll $13, %eax
    rorl $7, %ecx
    xorl %edi, %eax
    xorl %ecx, %eax
    xorl $0x9e3779b9, %eax
    ret
.size rmr_mix32_arch, .-rmr_mix32_arch
