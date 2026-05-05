section .text
bits 64

global irq0_stub, irq1_stub, irq2_stub, irq3_stub
global irq4_stub, irq5_stub, irq6_stub, irq7_stub
global irq8_stub, irq9_stub, irq10_stub, irq11_stub
global irq12_stub, irq13_stub, irq14_stub, irq15_stub

extern irq_handler

%macro IRQ_STUB 1
irq%1_stub:
    ;cli
    push 0          ; Dummy error code (not all IRQs have one)
    push %1         ; IRQ number
    jmp irq_common_stub
%endmacro

IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15

; Common IRQ handler logic
irq_common_stub:
    ; Save general-purpose registers (System V AMD64 ABI)
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rdi
    push rsi
    push rbp
    push rdx
    push rcx
    push rbx
    push rax

    ; Set up first function argument for irq_handler (pointer to "registers_t")
    mov rdi, rsp
    call irq_handler
    mov rsp, rax

    ; Remove pushed irq number and dummy error code
    add rsp, 16

    ; Restore registers
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rbp
    pop rsi
    pop rdi
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    iretq
