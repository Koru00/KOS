/* arch/riscv64/boot/entry.S */

// Entry Setup before juping to the kernel

.section .text.boot
.global _start
.extern kernel_main

_start:
	// Setup GP
	.option push
	.option norelax
	la gp, __global_pointer$
	.option pop

    	# Set stack pointer (provided by linker)
    	la sp, _stack_top

bss_clear:
    	bge t0, t1, bss_done
    	sd zero, 0(t0)        # Use 'sd' for 64-bit double-word clear
    	addi t0, t0, 8
    	j bss_clear

bss_done:
	// Jump to the kernel entry point
	call kernel_main

halt:
    wfi
    j halt
