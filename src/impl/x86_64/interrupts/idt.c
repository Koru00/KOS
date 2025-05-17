#include "idt.h"
#include <stddef.h>

extern void load_idt(struct IDTPointer* idt_ptr); // Defined in Assembly
extern void keyboard_isr(); // Assembly stub for IRQ1

static struct IDTEntry idt[IDT_ENTRIES];
static struct IDTPointer idt_ptr;

void set_idt_gate(int n, uint32_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // Kernel code segment
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E; // 0b10001110: present, ring 0, 32-bit interrupt gate
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}

void init_idt() {
    memset(&idt, 0, sizeof(idt));

    set_idt_gate(0x21, (uint32_t)keyboard_isr); // IRQ1 (keyboard)

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;

    load_idt(&idt_ptr);
}
