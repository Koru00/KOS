.section .text
.global lidt

# void lidt(void* base, uint16_t size)
lidt:
    mov %rdi, %rax        # base pointer
    lidt (%rax)           # load IDT
    ret
