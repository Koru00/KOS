/* arch/x86_64/boot/long_mode.S */

.section .text
.code64
.global long_mode_entry
.extern kernel_main

long_mode_entry:
    /* Reload segments (flat) */
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es

    /* Setup stack (64-bit) */
    mov $stack_top, %rsp

    /* Call kernel */
    call kernel_main

.hang:
    hlt
    jmp .hang
