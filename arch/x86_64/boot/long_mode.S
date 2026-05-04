/* arch/x86_64/boot/long_mode.S */

.section .text.boot
.code64
.global long_mode_entry
.extern kernel_main
.extern stack_top

long_mode_entry:
    mov $'L', %al
    out %al, $0xE9
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
