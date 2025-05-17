global load_idt
global keyboard_isr

extern keyboard_callback

[bits 32]

load_idt:
    mov eax, [esp+4]
    lidt [eax]
    ret

keyboard_isr:
    pusha
    call keyboard_callback
    popa
    iretd
