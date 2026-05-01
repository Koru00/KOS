/* linker/x86_64.ld */

OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    /* Kernel virtual base (higher half) */
    . = 0xffffffff80000000;

    /* Text section (code) */
    .text : ALIGN(4K)
    {
        *(.text.boot)        /* early boot code */
        *(.text*)            /* all code */
    }

    /* Read-only data */
    .rodata : ALIGN(4K)
    {
        *(.rodata*)
    }

    /* Writable data */
    .data : ALIGN(4K)
    {
        *(.data*)
    }

    /* BSS (zeroed memory) */
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

    /* Kernel end symbol */
    _kernel_end = .;
}
