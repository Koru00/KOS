/* arch/x86_64/boot/entry32.S */

.intel_syntax noprefix
.global start
.extern long_mode_start

.global page_table_l4
.global page_table_l3
.global page_table_l2
.global gdt64_pointer
.global stack_bottom
.global stack_top

.section .boot.rodata

gdt64:
    .quad 0                             /* dq -> .quad */

/* GAS uses .set or = for assignments, and labels must end with ':' */
.set code_segment, . - gdt64
    .quad (1 << 47) | (1 << 43) | (1 << 44) | (1 << 53)

.set data_segment, . - gdt64
    .quad (1 << 47) | (1 << 44) | (1 << 53)

.set user_code_segment, . - gdt64
    .quad (1 << 47) | (3 << 45) | (1 << 43) | (1 << 44) | (1 << 53)

.set user_data_segment, . - gdt64
    .quad (1 << 47) | (3 << 45) | (1 << 44) | (1 << 53)

gdt64_pointer:
    .word gdt64_end - gdt64 - 1
    .quad gdt64 # This is a 64-bit address, handled by p_gdt64_pointer math

gdt64_end:

.section .boot.text
.code32

start:
    mov esp, OFFSET p_stack_top
    call check_multiboot
    call check_cpuid
    call check_long_mode
    call setup_page_tables
    call enable_paging
    lgdt [p_gdt64_pointer]

    # Instead of: jmp 0x08:long_mode_start
    # Use an indirect far jump:
    push 0x08                        # Push the Code Segment selector
    mov eax, OFFSET p_long_mode_ptr  # Load physical address of our jump pointer
    push [eax]                       # Push the value at p_long_mode_ptr (the address of long_mode_start)
    retf                             # "Return Far" - this pops IP then CS, effectively jumping
    
    hlt

check_multiboot:
    cmp eax, 0x36d76289
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, 'M'
    jmp error

check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, 'C'
    jmp error

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, 'L'
    jmp error

setup_page_tables:
    mov eax, OFFSET p_page_table_l3
    or eax, 0b11
    mov [p_page_table_l4], eax
    mov eax, OFFSET p_page_table_l2
    or eax, 0b11
    mov [p_page_table_l3], eax
    xor ecx, ecx
.loop:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000111
    /* Explicitly size the destination for GAS */
    mov DWORD PTR [p_page_table_l2 + ecx*8], eax
    mov DWORD PTR [p_page_table_l2 + ecx*8 + 4], 0
    inc ecx
    cmp ecx, 512
    jne .loop
    ret

enable_paging:
    mov eax, OFFSET p_page_table_l4
    mov cr3, eax
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    ret

error:
    mov BYTE PTR [0xb8000], 'E'
    mov BYTE PTR [0xb8001], 'R'
    mov BYTE PTR [0xb8002], 'R'
    mov BYTE PTR [0xb8003], ':'
    mov BYTE PTR [0xb8004], al
    mov BYTE PTR [0xb8005], 0x0A
    hlt

.section .bss
.align 4096
page_table_l4: .skip 4096              /* resb -> .skip */
page_table_l3: .skip 4096
page_table_l2: .skip 4096
stack_bottom:  .skip 4096*4
stack_top:
