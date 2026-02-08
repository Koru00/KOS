; main.asm - Boot entry, paging, GDT, and jump to long mode

global start
extern long_mode_start

section .rodata

gdt64:
    dq 0

.code_segment: equ gdt64 + 8
    dq (1 << 47) | (1 << 43) | (1 << 44) | (1 << 53)

.data_segment: equ gdt64 + 16
    dq (1 << 47) | (1 << 44) | (1 << 53)

.user_code_segment: equ gdt64 + 24
    dq (1 << 47) | (3 << 45) | (1 << 43) | (1 << 44) | (1 << 53)

.user_data_segment: equ gdt64 + 32
    dq (1 << 47) | (3 << 45) | (1 << 44) | (1 << 53)

.pointer:
    dw gdt64_end - gdt64 - 1
    dq gdt64

gdt64_end:

section .text
bits 32

start:
    mov esp, stack_top
    call check_multiboot
    call check_cpuid
    call check_long_mode
    call setup_page_tables
    call enable_paging
    lgdt [gdt64.pointer]
    jmp 0x08:long_mode_start
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
    mov eax, page_table_l3
    or eax, 0b11
    mov [page_table_l4], eax
    mov eax, page_table_l2
    or eax, 0b11
    mov [page_table_l3], eax
    xor ecx, ecx
.loop:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000111
    mov [page_table_l2 + ecx*8], eax
    inc ecx
    cmp ecx, 512
    jne .loop
    ret

enable_paging:
    mov eax, page_table_l4
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
    mov byte [0xb8000], 'E'
    mov byte [0xb8001], 'R'
    mov byte [0xb8002], 'R'
    mov byte [0xb8003], ':'
    mov byte [0xb8004], al
    mov byte [0xb8005], 0x0A
    hlt

section .bss
align 4096
page_table_l4: resb 4096
page_table_l3: resb 4096
page_table_l2: resb 4096
stack_bottom:  resb 4096*4
stack_top:
