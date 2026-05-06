OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80200000;  /* QEMU default load address */

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
    }

    . = ALIGN(16);
    _stack_bottom = .;
    . += 0x4000; /* 16 KB stack */
    _stack_top = .;
}
