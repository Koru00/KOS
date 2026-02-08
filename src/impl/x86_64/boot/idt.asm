global load_idt
global keyboard_isr

extern keyboard_callback

[bits 64]
global load_idt

load_idt:
    lidt [rdi]
    ret

