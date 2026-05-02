/* arch/x86_64/cpu/gdt.S */

.section .data.boot
.align 8
gdt_start:
    .quad 0x0000000000000000          # Null descriptor
gdt_code: 
    .quad 0x00209a0000000000          # 64-bit code segment
gdt_data:
    .quad 0x0000920000000000          # 64-bit data segment
gdt_end:

.global gdt_descriptor
gdt_descriptor:
    .short gdt_end - gdt_start - 1    # Limit
    .long gdt_start                   # Base
