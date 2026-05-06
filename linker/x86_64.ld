OUTPUT_FORMAT(elf64-x86-64)
ENTRY(start)

PHDRS
{
    boot PT_LOAD FLAGS(5);   /* R + X */
    kernel PT_LOAD FLAGS(7); /* R + W + X */
}

SECTIONS
{
    /* --- BOOT SECTION FIRST --- */
    . = 0;

    .boot 0x100000 : AT(0)
    {
        KEEP(*(.multiboot_header))
        KEEP(*(.boot*))
    } :boot
    
    /* 3. Physical aliases for 32-bit assembly */
    p_page_table_l4 = page_table_l4 - 0xffffffff80000000;
    p_page_table_l3 = page_table_l3 - 0xffffffff80000000;
    p_page_table_l2 = page_table_l2 - 0xffffffff80000000;
    p_stack_top = stack_top - 0xffffffff80000000;
    p_gdt64_pointer = gdt64_pointer - 0xffffffff80000000;
    p_long_mode_ptr = long_mode_start - 0xffffffff80000000;

    /* --- KERNEL HIGH HALF --- */
    . = ALIGN(4K);
    . += 0xffffffff80000000;

    .text : AT(ADDR(.text) - 0xffffffff80000000) {
        *(.text*)
    } : kernel

    .rodata : AT(ADDR(.rodata) - 0xffffffff80000000) {
        *(.rodata*)
    } : kernel

    .data : AT(ADDR(.data) - 0xffffffff80000000) {
        *(.data*)
    } : kernel

    .bss : AT(ADDR(.bss) - 0xffffffff80000000) {
        *(COMMON)
        *(.bss*)
    } : kernel

    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}
