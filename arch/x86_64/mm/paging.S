/* paging.S */

.section .data.boot
.align 4096
.global pml4_table
.global pdpt_identity
.global pdpt_higher
.global pd_identity
.global pd_higher

.align 4096
pdpt_identity:
    .quad pd_identity + 0x3
    .fill 511, 8, 0

.align 4096
pd_identity:
    .quad 0x00000000 + 0x83
    .fill 511, 8, 0

.align 4096
pml4_table:
    .quad pdpt_identity + 0x3     # Index 0: Identity map
    .fill 510, 8, 0               # Indices 1–510: Empty
    .quad pdpt_higher + 0x3       # Index 511: Higher half (0xFFFFFFFF80000000)

.align 4096
pdpt_higher:
    # In the Top 2GB, the address bits point to the last entries of the PDPT
    .fill 510, 8, 0               # Indices 0–509: Empty
    .quad pd_higher + 0x3         # Index 510: Maps 0xFFFFFFFF80000000
    .fill 1, 8, 0                 # Index 511: Empty

.align 4096
pd_higher:
    # This maps the actual 2MB huge pages starting at 0xFFFFFFFF80000000
    .quad 0x00000000 + 0x83       # 0xFFFFFFFF80000000 -> 0x00000000
    .quad 0x00200000 + 0x83       # 0xFFFFFFFF80200000 -> 0x00200000
    .quad 0x00400000 + 0x83       # ...
    .quad 0x00600000 + 0x83
    .fill 508, 8, 0
