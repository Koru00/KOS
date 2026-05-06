OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    /* QEMU virt machine RAM starts here */
    . = 0x80000000; 

    /* --- CODE SECTION (Read / Execute) --- */
    .text : ALIGN(4096) {
        _text_start = .;
        KEEP(*(.text.boot)) /* Ensure boot.S is FIRST */
        *(.text .text.*)
        . = ALIGN(4096);
        _text_end = .;
    }

    /* --- READ-ONLY DATA (Read Only) --- */
    .rodata : ALIGN(4096) {
        _rodata_start = .;
        *(.rodata .rodata.*)
        . = ALIGN(4096);
        _rodata_end = .;
    }

    /* --- READ/WRITE DATA (Read / Write) --- */
    .data : ALIGN(4096) {
        _data_start = .;
        
        /* Global Pointer for linker relaxation */
        __global_pointer$ = . + 0x800;
        
        *(.sdata .sdata.*)
        *(.data .data.*)
        . = ALIGN(4096);
        _data_end = .;
    }

    /* --- UNINITIALIZED DATA & STACK (Read / Write) --- */
    .bss : ALIGN(4096) {
        _bss_start = .;
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)

        /* Allocate stack safely INSIDE our managed footprint */
        . = ALIGN(16);
        _stack_bottom = .;
        . += 0x4000; /* 16 KB stack */
        _stack_top = .;

        . = ALIGN(4096);
        _bss_end = .;
    }

    /* The true end of the kernel footprint.
       The Physical Memory Manager starts allocating from here! */
    . = ALIGN(4096);
    _kernel_end = .;
}
