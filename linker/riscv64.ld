OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    . = 0x80000000;  /* QEMU default load address */

    .text : {
        KEEP(*(.text.boot)) /* Ensure boot.s is FIRST */
        *(.text .text.*)
    }
    
    .rodata : {
        *(.rodata*)
    }

    .data : {
    	. = ALIGN(16);
    	__global_pointer$ = . + 0x800;
   	*(.sdata .sdata.*)
    	*(.data .data.*)
    }

    .bss : {
    	. = ALIGN(16);
    	_bss_start = .;
    	*(.sbss .sbss.*)
    	*(.bss .bss.*)
    	*(COMMON)
    	_bss_end = .;
    }

    . = ALIGN(16);
    _stack_bottom = .;
    . += 0x4000; /* 16 KB stack */
    _stack_top = .;
}
