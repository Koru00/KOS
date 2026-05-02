/* arch/x86_64/boot/entry32.S */

.section .text.boot
.global _start
.code32

.extern pml4_table
.extern long_mode_entry

_start:
    cli

    /* -------------------------
       Safe temporary stack (must be low memory)
       DO NOT use linker symbols here
    -------------------------- */
    mov $0x90000, %esp

    /* Save multiboot info */
    mov %ebx, multiboot_info_ptr

    /* -------------------------
       CPUID check
    -------------------------- */
    pushf
    pop %eax
    mov %eax, %ecx
    xor $0x200000, %eax
    push %eax
    popf
    pushf
    pop %eax
    xor %ecx, %eax
    jz .no_cpuid

    /* -------------------------
       Long mode support check
    -------------------------- */
    mov $0x80000000, %eax
    cpuid
    cmp $0x80000001, %eax
    jb .no_long_mode

    mov $0x80000001, %eax
    cpuid
    test $0x20000000, %edx
    jz .no_long_mode

    lgdt (gdt_descriptor)
   

    /* -------------------------
       Enable PAE
    -------------------------- */
    mov %cr4, %eax
    or $0x20, %eax
    mov %eax, %cr4

    /* -------------------------
       Load page tables (identity address!)
    -------------------------- */
    mov $pml4_table, %eax
    mov %eax, %cr3

    /* -------------------------
       Enable long mode (EFER)
    -------------------------- */
    mov $0xC0000080, %ecx
    rdmsr
    or $0x100, %eax
    wrmsr

    /* -------------------------
       Enable paging
    -------------------------- */
    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0

    /* -------------------------
       FAR JUMP to long mode entry
       (THIS IS REQUIRED — not a normal jmp)
    -------------------------- */
    ljmp $0x08, $long_mode_entry

.no_cpuid:
.no_long_mode:
.hang:
    hlt
    jmp .hang

.section .text.boot
.align 4
multiboot_info_ptr:
    .long 0
