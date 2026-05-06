.intel_syntax noprefix
.section .text

.global page_fault_stub
.global gp_fault_stub
.global double_fault_stub

.extern page_fault_handler
.extern general_protection_fault
.extern double_fault

# -------------------------
# Page fault stub
# -------------------------
page_fault_stub:
    cli

/* error code is at [rsp] */
    mov rdi, QWORD PTR [rsp]

    # RIP is at [rsp + 8]
    mov rsi, QWORD PTR [rsp + 8]    # rip

    mov rax, cr2
    mov rdx, rax                    # cr2

    call page_fault_handler

.hang:
    hlt
    jmp .hang

# -------------------------
# GP fault stub
# -------------------------
gp_fault_stub:
    cli

    mov rdi, QWORD PTR [rsp]
    call general_protection_fault

.hang2:
    hlt
    jmp .hang2

# -------------------------
# Double fault stub
# -------------------------
double_fault_stub:
    cli

    mov rdi, QWORD PTR [rsp]
    call double_fault

.hang3:
    hlt
    jmp .hang3
