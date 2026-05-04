OUTPUT_FORMAT(elf64-x86-64)
ENTRY(start)

PHDRS
{
    /* Define a single loadable segment for GRUB */
    boot PT_LOAD FLAGS(7); /* Read, Write, Execute */
}

SECTIONS
{
    /* 1. Physical address starts at 1MB */
    . = 1M;

    _boot_start = .;

    /* 2. Boot section: VMA == LMA (Low memory) */
    .boot : {
        KEEP(*(.multiboot_header))
        *(.boot.text)
        *(.boot.rodata)
    } : boot

    . = ALIGN(4K);
    _boot_end = .;

    /* 3. Physical aliases for 32-bit assembly */
    p_page_table_l4 = page_table_l4 - 0xffffffff80000000;
    p_page_table_l3 = page_table_l3 - 0xffffffff80000000;
    p_page_table_l2 = page_table_l2 - 0xffffffff80000000;
    p_stack_top     = stack_top     - 0xffffffff80000000;
    p_gdt64_pointer = gdt64_pointer - 0xffffffff80000000;
    p_long_mode_ptr = long_mode_start - 0xffffffff80000000;

    /* 4. Move Virtual Address to High-Half, but keep Physical Address at ~1MB */
    . = ALIGN(4K);
    . += 0xffffffff80000000;

    .text : AT(ADDR(.text) - 0xffffffff80000000) {
        *(.text*)
    } : boot

    .rodata : AT(ADDR(.rodata) - 0xffffffff80000000) {
        *(.rodata*)
    } : boot

    .data : AT(ADDR(.data) - 0xffffffff80000000) {
        *(.data*)
    } : boot

    .bss : AT(ADDR(.bss) - 0xffffffff80000000) {
        *(COMMON)
        *(.bss*)
    } : boot

    _kernel_end = .;
}
