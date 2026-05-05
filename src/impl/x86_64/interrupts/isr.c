#include "isr.h"
#include "ports.h"

#define MAX_INTERRUPTS 256

typedef void (*isr_t)(registers_t *);
static isr_t interrupt_handlers[MAX_INTERRUPTS];

void isr_register_handler(int int_no, isr_t handler) {
    interrupt_handlers[int_no] = handler;
}

// Called by assembly ISR stubs on interrupt
void isr_handler(registers_t *regs) {
    if (interrupt_handlers[regs->int_no]) {
        interrupt_handlers[regs->int_no](regs);
    }

    // Send EOI to PIC if it's a hardware interrupt (IRQ0 to IRQ15)
    if (regs->int_no >= 32 && regs->int_no < 48) {
        if (regs->int_no >= 40) {
            port_byte_out(0xA0, 0x20); // slave PIC
        }
        port_byte_out(0x20, 0x20); // master PIC
    }
}
