OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    /* Physical Load Address */
    . = 1M;

    .boot : {
        *(.multiboot_header)
        *(.text.boot)    /* This now catches long_mode_entry */
        *(.data.boot)    /* This now catches pml4_table */
        *(.bss.boot)     /* This catches multiboot_info_ptr */
    }

    /* Move the location counter to the high half */
    . += 0xffffffff80000000;

    .text : AT(ADDR(.text) - 0xffffffff80000000) {
        *(.text*)
    }

    .rodata : AT(ADDR(.rodata) - 0xffffffff80000000) {
        *(.rodata*)
    }

    .data : AT(ADDR(.data) - 0xffffffff80000000) {
        *(.data*)
    }

    .bss : AT(ADDR(.bss) - 0xffffffff80000000) {
        *(COMMON)
        *(.bss*)
    }

    .stack : AT(ADDR(.stack) - 0xffffffff80000000) {
        stack_bottom = .;
        . += 16K;
        stack_top = .;
    }

    _kernel_end = .;
}
