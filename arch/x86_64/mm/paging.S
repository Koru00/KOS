.section .data.boot
.align 4096
.global pml4_table
.global pdpt_identity
.global pdpt_higher
.global pd_identity
.global pd_higher

pml4_table:
    .quad pdpt_identity + 0x3     # index 0

    .fill 255, 8, 0               # entries 1–255

    .quad pdpt_higher + 0x3       # index 256

    .fill 255, 8, 0               # entries 257–511

.align 4096
pdpt_identity:
    .quad pd_identity + 0x3
    .fill 511, 8, 0

.align 4096
pd_identity:
    .quad 0x00000000 + 0x83
    .fill 511, 8, 0

.align 4096
pdpt_higher:
    .quad pd_higher + 0x3
    .fill 511, 8, 0

.align 4096
pd_higher:
    .quad 0x00000000 + 0x83
    .quad 0x00200000 + 0x83
    .quad 0x00400000 + 0x83
    .quad 0x00600000 + 0x83
    .fill 508, 8, 0
