/* linker/aarch64.ld */

OUTPUT_FORMAT(elf64-littleaarch64)
ENTRY(_start)

SECTIONS
{
    /* Load at physical address */
    . = 0x40080000;

    .text : ALIGN(4K)
    {
        *(.text.boot)
        *(.text*)
    }

    .rodata : ALIGN(4K)
    {
        *(.rodata*)
    }

    .data : ALIGN(4K)
    {
        *(.data*)
    }

    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss*)
    }

    .stack : ALIGN(16)
    {
        stack_bottom = .;
        . += 16K;
        stack_top = .;
    }
    
    _kernel_end = .;
}
