.section .init, "ax"
.global _start

_start:
    .option norelax
    la t0, trap_vector
    csrw mtvec, t0

    la sp, __stack_top
    add s0, sp, zero
    jal zero, main

.type _hang STT_FUNC
.global _hang
trap_vector:
_hang:
    j _hang
    j _hang
    j _hang
.size _hang, . - _hang

.type _success STT_FUNC
.global _success
_success:
    j _success
    j _success
    j _success
.size _success, . - _success

.end
