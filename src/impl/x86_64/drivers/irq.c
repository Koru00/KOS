#include "ports.h"
#include "irq.h"

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

/* This array is actually an array of function pointers. We use
 *  this to handle custom IRQ handlers for a given IRQ */
void *irq_routines[16] =
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0};

/* This installs a custom IRQ handler for the given IRQ */
void irq_install_handler(int irq, void (*handler)(struct registers *r))
{
    irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */
void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
 *  is a problem in protected mode, because IDT entry 8 is a
 *  Double Fault! Without remapping, every time IRQ0 fires,
 *  you get a Double Fault Exception, which is NOT actually
 *  what's happening. We send commands to the Programmable
 *  Interrupt Controller (PICs - also called the 8259's) in
 *  order to make IRQ0 to 15 be remapped to IDT entries 32 to
 *  47 */
void irq_remap(void)
{
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);
}




/* We first remap the interrupt controllers, and then we install
 *  the appropriate ISRs to the correct entries in the IDT. This
 *  is just like installing the exception handlers */
void irq_install()
{
    irq_remap();

    set_idt_gate(32, (unsigned)irq0_stub, 0x08, 0x8E);
    set_idt_gate(33, (unsigned)irq1_stub, 0x08, 0x8E);
    set_idt_gate(34, (unsigned)irq2_stub, 0x08, 0x8E);
    set_idt_gate(35, (unsigned)irq3_stub, 0x08, 0x8E);
    set_idt_gate(36, (unsigned)irq4_stub, 0x08, 0x8E);
    set_idt_gate(37, (unsigned)irq5_stub, 0x08, 0x8E);
    set_idt_gate(38, (unsigned)irq6_stub, 0x08, 0x8E);
    set_idt_gate(39, (unsigned)irq7_stub, 0x08, 0x8E);
    set_idt_gate(40, (unsigned)irq8_stub, 0x08, 0x8E);
    set_idt_gate(41, (unsigned)irq9_stub, 0x08, 0x8E);
    set_idt_gate(42, (unsigned)irq10_stub, 0x08, 0x8E);
    set_idt_gate(43, (unsigned)irq11_stub, 0x08, 0x8E);
    set_idt_gate(44, (unsigned)irq12_stub, 0x08, 0x8E);
    set_idt_gate(45, (unsigned)irq13_stub, 0x08, 0x8E);
    set_idt_gate(46, (unsigned)irq14_stub, 0x08, 0x8E);
    set_idt_gate(47, (unsigned)irq15_stub, 0x08, 0x8E);
}

/* Each of the IRQ ISRs point to this function, rather than
 *  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
 *  to be told when you are done servicing them, so you need
 *  to send them an "End of Interrupt" command (0x20). There
 *  are two 8259 chips: The first exists at 0x20, the second
 *  exists at 0xA0. If the second controller (an IRQ from 8 to
 *  15) gets an interrupt, you need to acknowledge the
 *  interrupt at BOTH controllers, otherwise, you only send
 *  an EOI command to the first controller. If you don't send
 *  an EOI, you won't raise any more IRQs */
void irq_handler(struct registers *r)
{
    /* This is a blank function pointer */
    void (*handler)(struct registers *r);

    /* Find out if we have a custom handler to run for this
     *  IRQ, and then finally, run it */
    handler = irq_routines[r->int_no - 32];
    if (handler)
    {
        handler(r);
    }

    /* If the IDT entry that was invoked was greater than 40
     *  (meaning IRQ8 - 15), then we need to send an EOI to
     *  the slave controller */
    if (r->int_no >= 40)
    {
        port_byte_out(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
     *  interrupt controller too */
    port_byte_out(0x20, 0x20);
}
