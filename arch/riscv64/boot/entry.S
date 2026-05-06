.section .text
.global _start
.extern kernel_main

_start:
    # Set stack pointer (provided by linker)
    la sp, _stack_top

    # Call C kernel
    call kernel_main

1:  j 1b   # infinite loop
