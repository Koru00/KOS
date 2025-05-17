#include "isr.h" // contains registers_t definition and isr_register_handler declaration
#include "ports.h"

#define MAX_INTERRUPTS 256

static isr_t interrupt_handlers[MAX_INTERRUPTS]; // typedef void (*isr_t)(registers_t *);

void isr_register_handler(int irq, isr_t handler) {
    interrupt_handlers[irq] = handler;
}

// Called by assembly ISR stubs on interrupt
void isr_handler(registers_t *regs) {
    if (interrupt_handlers[regs->int_no]) {
        interrupt_handlers[regs->int_no](regs);
    }

    // Send EOI to PIC if interrupt number >= 32 (hardware IRQs)
    if (regs->int_no >= 32 && regs->int_no < 48) {
        // PIC EOI ports
        if (regs->int_no >= 40) {
            port_byte_out(0xA0, 0x20); // slave PIC
        }
        port_byte_out(0x20, 0x20); // master PIC
    }
}
