.section .init, "ax"
.global _start

_start:
    .option norelax
    la t0, trap_vector
    csrw mtvec, t0

    la sp, __stack_top
    add s0, sp, zero
    jal zero, main

trap_vector:
    j _hang

_hang:
    j _hang
    j _hang
    j _hang

.end
