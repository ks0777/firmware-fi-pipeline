.section .text, "ax"
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

.type _hang STT_FUNC
.global _hang
_hang:
    j _hang
    j _hang
    j _hang
    ret
.size _hang, . - _hang

.type _success STT_FUNC
.global _success
_success:
    j _hang
    j _hang
    j _hang
    ret
.size _success, . - _success

.end
