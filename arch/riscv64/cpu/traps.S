/* arch/riscv64/cpu/traps.S */

.global trap_vector
trap_vector:
    call trap_handler
    sret
