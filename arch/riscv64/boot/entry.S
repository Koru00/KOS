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
	// Write the adress of trap_entry into the mtvec
	la t0, trap_entry
    	csrw mtvec, t0
	
	// Jump to the kernel entry point
	call kernel_main

.align 4    # mtvec requires 4-byte alignment
trap_entry:

    # 1. Allocate space on the stack for 32 registers (32 * 8 bytes = 256)
    addi sp, sp, -256

    # 2. Save all registers (x1 to x31)
    sd x1, 8(sp) 
    sd x2, 16(sp)
    sd x3, 24(sp)
    sd x4, 32(sp)
    sd x5, 40(sp)
    sd x6, 48(sp)
    sd x7, 56(sp)
    sd x8, 64(sp)
    sd x9, 72(sp)
    sd x10, 80(sp)
    sd x11, 88(sp)
    sd x12, 96(sp)
    sd x13, 104(sp)
    sd x14, 112(sp)
    sd x15, 120(sp)
    sd x16, 128(sp)
    sd x17, 136(sp)
    sd x18, 144(sp)
    sd x19, 152(sp)
    sd x20, 160(sp)
    sd x21, 168(sp)
    sd x22, 176(sp)
    sd x23, 184(sp)
    sd x24, 192(sp)
    sd x25, 200(sp)
    sd x26, 208(sp)
    sd x27, 216(sp)
    sd x28, 224(sp)
    sd x29, 232(sp)
    sd x30, 240(sp)
    sd x31, 248(sp)

    # 3. Pass the stack pointer to C as the first argument (a0)
    # This allows the C handler to inspect the saved registers
    mv a0, sp
    call trap_handler

    # 4. Restore all registers
    ld x1, 8(sp) 
    ld x2, 16(sp)
    ld x3, 24(sp)
    ld x4, 32(sp)
    ld x5, 40(sp)
    ld x6, 48(sp)
    ld x7, 56(sp)
    ld x8, 64(sp)
    ld x9, 72(sp)
    ld x10, 80(sp)
    ld x11, 88(sp)
    ld x12, 96(sp)
    ld x13, 104(sp)
    ld x14, 112(sp)
    ld x15, 120(sp)
    ld x16, 128(sp)
    ld x17, 136(sp)
    ld x18, 144(sp)
    ld x19, 152(sp)
    ld x20, 160(sp)
    ld x21, 168(sp)
    ld x22, 176(sp)
    ld x23, 184(sp)
    ld x24, 192(sp)
    ld x25, 200(sp)
    ld x26, 208(sp)
    ld x27, 216(sp)
    ld x28, 224(sp)
    ld x29, 232(sp)
    ld x30, 240(sp)
    ld x31, 248(sp)
    
    # 5. Free stack space and return from trap
    addi sp, sp, 256
    mret

    call trap_handler
    mret

halt:
    wfi
    j halt
