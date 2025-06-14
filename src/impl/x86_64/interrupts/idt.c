#include <stdint.h>
#include <string.h>
#include "idt.h"

extern void load_idt(struct IDTPointer* idt_ptr);
extern void irq0_stub(); extern void irq1_stub(); extern void irq2_stub(); extern void irq3_stub();
extern void irq4_stub(); extern void irq5_stub(); extern void irq6_stub(); extern void irq7_stub();
extern void irq8_stub(); extern void irq9_stub(); extern void irq10_stub(); extern void irq11_stub();
extern void irq12_stub(); extern void irq13_stub(); extern void irq14_stub(); extern void irq15_stub();
extern void isr128();

static struct IDTEntry idt[IDT_ENTRIES];
static struct IDTPointer idt_ptr;

void set_idt_gate(int n, uint64_t handler, uint16_t sel, uint8_t flags) {
    idt[n].offset_low  = (uint16_t)(handler & 0xFFFF);
    idt[n].selector    = sel;
    idt[n].ist         = 0;
    idt[n].type_attr   = flags;
    idt[n].offset_mid  = (uint16_t)((handler >> 16) & 0xFFFF);
    idt[n].offset_high = (uint32_t)((handler >> 32) & 0xFFFFFFFF);
    idt[n].zero        = 0;
}

void init_idt(void) {
    memset(idt, 0, sizeof(idt));
    set_idt_gate(32, (uint64_t)irq0_stub,  0x08, 0x8E);
    set_idt_gate(33, (uint64_t)irq1_stub,  0x08, 0x8E);
    set_idt_gate(34, (uint64_t)irq2_stub,  0x08, 0x8E);
    set_idt_gate(35, (uint64_t)irq3_stub,  0x08, 0x8E);
    set_idt_gate(36, (uint64_t)irq4_stub,  0x08, 0x8E);
    set_idt_gate(37, (uint64_t)irq5_stub,  0x08, 0x8E);
    set_idt_gate(38, (uint64_t)irq6_stub,  0x08, 0x8E);
    set_idt_gate(39, (uint64_t)irq7_stub,  0x08, 0x8E);
    set_idt_gate(40, (uint64_t)irq8_stub,  0x08, 0x8E);
    set_idt_gate(41, (uint64_t)irq9_stub,  0x08, 0x8E);
    set_idt_gate(42, (uint64_t)irq10_stub, 0x08, 0x8E);
    set_idt_gate(43, (uint64_t)irq11_stub, 0x08, 0x8E);
    set_idt_gate(44, (uint64_t)irq12_stub, 0x08, 0x8E);
    set_idt_gate(45, (uint64_t)irq13_stub, 0x08, 0x8E);
    set_idt_gate(46, (uint64_t)irq14_stub, 0x08, 0x8E);
    set_idt_gate(47, (uint64_t)irq15_stub, 0x08, 0x8E);
    set_idt_gate(0x80, (uint64_t)isr128,    0x08, 0xEE);
    idt_ptr.limit = (uint16_t)(sizeof(idt) - 1);
    idt_ptr.base  = (uint64_t)&idt;
    load_idt(&idt_ptr);
}
