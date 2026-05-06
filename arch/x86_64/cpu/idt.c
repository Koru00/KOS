#include "arch/x86-64/idt.h"
#include "arch/x86-64/interrupts.h"

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idtp;

extern void lidt(void* base, uint16_t size);
extern void page_fault_stub();
extern void gp_fault_stub();
extern void double_fault_stub();
extern void default_interrupt_handler();

static void set_gate(int vec, void (*handler)(), uint8_t flags)
{
    uint64_t addr = (uint64_t)handler;

    idt[vec].offset_low  = addr & 0xFFFF;
    idt[vec].selector    = 0x08; // kernel code segment
    idt[vec].ist         = 0;
    idt[vec].type_attr   = flags;
    idt[vec].offset_mid  = (addr >> 16) & 0xFFFF;
    idt[vec].offset_high = (addr >> 32);
    idt[vec].zero        = 0;
}

void idt_init()
{
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint64_t)&idt;

    // clear
    for (int i = 0; i < IDT_SIZE; i++)
        set_gate(i, default_interrupt_handler, 0x8E);

    // real faults
    set_gate(14, page_fault_stub, 0x8E);
    set_gate(13, gp_fault_stub, 0x8E);
    set_gate(8,  double_fault_stub, 0x8E);

    lidt(&idt, sizeof(idt));
}
