#include "idt.h"
#include <stddef.h>
#include <string.h>

extern void load_idt(struct IDTPointer* idt_ptr); // Defined in Assembly
extern void keyboard_isr(); // Assembly stub for IRQ1

static struct IDTEntry idt[IDT_ENTRIES];
static struct IDTPointer idt_ptr;

/*
void set_idt_gate(int n, uint32_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // Kernel code segment
    idt[n].zero = 0;
    idt[n].type_attr = 0x8E; // 0b10001110: present, ring 0, 32-bit interrupt gate
    idt[n].offset_high = (handler >> 16) & 0xFFFF;
}
    */

void set_idt_gate(int n, uint64_t handler, uint16_t sel, uint8_t flags) {
    idt[n].offset_low  = handler & 0xFFFF;
    idt[n].selector    = sel;
    idt[n].ist         = 0;
    idt[n].type_attr   = flags;
    idt[n].offset_mid  = (handler >> 16) & 0xFFFF;
    idt[n].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[n].zero        = 0;
}


/*
void init_idt() {
    memset(&idt, 0, sizeof(idt));

    set_idt_gate(0x21, (uint32_t)keyboard_isr, 0x08, 0x8E); // IRQ1 (keyboard)

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;

    load_idt(&idt_ptr);
}
    */


    /* These are own ISRs that point to our special IRQ handler
 *  instead of the regular 'fault_handler' function */
extern void irq0_stub();
extern void irq1_stub();
extern void irq2_stub();
extern void irq3_stub();
extern void irq4_stub();
extern void irq5_stub();
extern void irq6_stub();
extern void irq7_stub();
extern void irq8_stub();
extern void irq9_stub();
extern void irq10_stub();
extern void irq11_stub();
extern void irq12_stub();
extern void irq13_stub();
extern void irq14_stub();
extern void irq15_stub();

void init_idt() {
    memset(&idt, 0, sizeof(idt));


    /*

   for (int i = 0; i < 32; i++) {
        set_idt_gate(i, (uint64_t)isr_stubs[i], 0x08, 0x8E);
    }
    for (int i = 32; i < 48; i++) {
        set_idt_gate(i, (uint64_t)irq_stubs[i - 32], 0x08, 0x8E);
}
        */
/*
    set_idt_gate(0, (uint64_t)irq0_stub, 0x08, 0x8E);
    set_idt_gate(1, (uint64_t)irq1_stub, 0x08, 0x8E);
    set_idt_gate(2, (uint64_t)irq2_stub, 0x08, 0x8E);
    set_idt_gate(3, (uint64_t)irq3_stub, 0x08, 0x8E);
    set_idt_gate(4, (uint64_t)irq4_stub, 0x08, 0x8E);
    set_idt_gate(5, (uint64_t)irq5_stub, 0x08, 0x8E);
    set_idt_gate(6, (uint64_t)irq6_stub, 0x08, 0x8E);
    set_idt_gate(7, (uint64_t)irq7_stub, 0x08, 0x8E);
    set_idt_gate(8, (uint64_t)irq8_stub, 0x08, 0x8E);
    set_idt_gate(9, (uint64_t)irq9_stub, 0x08, 0x8E);
    set_idt_gate(10, (uint64_t)irq10_stub, 0x08, 0x8E);
    set_idt_gate(11, (uint64_t)irq11_stub, 0x08, 0x8E);
    set_idt_gate(12, (uint64_t)irq12_stub, 0x08, 0x8E);
    set_idt_gate(13, (uint64_t)irq13_stub, 0x08, 0x8E);
    set_idt_gate(14, (uint64_t)irq14_stub, 0x08, 0x8E);
    set_idt_gate(15, (uint64_t)irq15_stub, 0x08, 0x8E);

    set_idt_gate(16, (uint64_t)irq0_stub, 0x08, 0x8E);
    set_idt_gate(17, (uint64_t)irq1_stub, 0x08, 0x8E);
    set_idt_gate(18, (uint64_t)irq2_stub, 0x08, 0x8E);
    set_idt_gate(19, (uint64_t)irq3_stub, 0x08, 0x8E);
    set_idt_gate(20, (uint64_t)irq4_stub, 0x08, 0x8E);
    set_idt_gate(21, (uint64_t)irq5_stub, 0x08, 0x8E);
    set_idt_gate(22, (uint64_t)irq6_stub, 0x08, 0x8E);
    set_idt_gate(23, (uint64_t)irq7_stub, 0x08, 0x8E);
    set_idt_gate(24, (uint64_t)irq8_stub, 0x08, 0x8E);
    set_idt_gate(25, (uint64_t)irq9_stub, 0x08, 0x8E);
    set_idt_gate(26, (uint64_t)irq10_stub, 0x08, 0x8E);
    set_idt_gate(27, (uint64_t)irq11_stub, 0x08, 0x8E);
    set_idt_gate(28, (uint64_t)irq12_stub, 0x08, 0x8E);
    set_idt_gate(29, (uint64_t)irq13_stub, 0x08, 0x8E);
    set_idt_gate(30, (uint64_t)irq14_stub, 0x08, 0x8E);
    set_idt_gate(31, (uint64_t)irq15_stub, 0x08, 0x8E);
*/
    set_idt_gate(32, (uint64_t)irq0_stub, 0x08, 0x8E);
    set_idt_gate(33, (uint64_t)irq1_stub, 0x08, 0x8E);
    set_idt_gate(34, (uint64_t)irq2_stub, 0x08, 0x8E);
    set_idt_gate(35, (uint64_t)irq3_stub, 0x08, 0x8E);
    set_idt_gate(36, (uint64_t)irq4_stub, 0x08, 0x8E);
    set_idt_gate(37, (uint64_t)irq5_stub, 0x08, 0x8E);
    set_idt_gate(38, (uint64_t)irq6_stub, 0x08, 0x8E);
    set_idt_gate(39, (uint64_t)irq7_stub, 0x08, 0x8E);
    set_idt_gate(40, (uint64_t)irq8_stub, 0x08, 0x8E);
    set_idt_gate(41, (uint64_t)irq9_stub, 0x08, 0x8E);
    set_idt_gate(42, (uint64_t)irq10_stub, 0x08, 0x8E);
    set_idt_gate(43, (uint64_t)irq11_stub, 0x08, 0x8E);
    set_idt_gate(44, (uint64_t)irq12_stub, 0x08, 0x8E);
    set_idt_gate(45, (uint64_t)irq13_stub, 0x08, 0x8E);
    set_idt_gate(46, (uint64_t)irq14_stub, 0x08, 0x8E);
    set_idt_gate(47, (uint64_t)irq15_stub, 0x08, 0x8E);

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;

    load_idt(&idt_ptr);
}



/*
keyboard_isr:
    pusha
    call keyboard_callback
    popa
    iretd
*/