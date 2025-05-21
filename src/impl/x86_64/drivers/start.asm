global irq0
global irq15

extern irq_handler

section .text

; IRQ0 handler (interrupt number 32)
irq0:
    cli
    push qword 0          ; Dummy error code
    push qword 32         ; IRQ number
    jmp irq_common_stub

; IRQ15 handler (interrupt number 47)
irq15:
    cli
    push qword 0
    push qword 47
    jmp irq_common_stub

irq_common_stub:
    ; Save registers manually
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Save segment registers (not usually needed in 64-bit, but keeping for compatibility)
    mov rax, ds
    push rax
    mov rax, es
    push rax
    mov rax, fs
    push rax
    mov rax, gs
    push rax

    ; Set up segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Pass pointer to struct regs (RSP)
    mov rdi, rsp
    call irq_handler

    ; Restore segment registers
    pop rax
    mov gs, ax
    pop rax
    mov fs, ax
    pop rax
    mov es, ax
    pop rax
    mov ds, ax

    ; Restore general-purpose registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16      ; Remove pushed error code and int number (2× 8 bytes)
    iretq            ; Return from interrupt (64-bit)
