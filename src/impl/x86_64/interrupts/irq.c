#include "ports.h"
#include "irq.h"
#include "logging.h"
#include <stddef.h>

// IRQ stub declarations
extern void irq0_stub(), irq1_stub(), irq2_stub(), irq3_stub();
extern void irq4_stub(), irq5_stub(), irq6_stub(), irq7_stub();
extern void irq8_stub(), irq9_stub(), irq10_stub(), irq11_stub();
extern void irq12_stub(), irq13_stub(), irq14_stub(), irq15_stub();

// IRQ handler table
static void (*irq_routines[16])(struct registers*) = {NULL};

void irq_install_handler(int irq, void (*handler)(struct registers *r)) {
    if (irq >= 0 && irq < 16) {
        irq_routines[irq] = handler;
        LOG_DEBUG("IRQ handler installed for IRQ %d", irq);
    }
}

void irq_uninstall_handler(int irq) {
    if (irq >= 0 && irq < 16) {
        irq_routines[irq] = NULL;
        LOG_DEBUG("IRQ handler uninstalled for IRQ %d", irq);
    }
}

void irq_remap(void) {
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
    
    LOG_DEBUG("IRQ remapping completed");
}

void irq_install(void) {
    irq_remap();

    // Install IRQ gates in IDT
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
    
    LOG_DEBUG("IRQ gates installed in IDT");
}

struct cpu_status_s *irq_handler(struct cpu_status_s *css) {
    // Validate input pointer
    if (!css) return NULL;
    
    // Send EOI to slave PIC if needed (IRQ 8-15)
    if (css->vector_number >= 40) {
        port_byte_out(0xA0, 0x20);
    }

    // Handle specific IRQs
    switch (css->vector_number) {
        case 32:  // IRQ0 - Timer
            timer_callback();
            break;
        case 33:  // IRQ1 - Keyboard
            keyboard_callback();
            break;
        default: {
            // Call custom handler if registered
            int irq = css->vector_number - 32;
            if (irq >= 0 && irq < 16 && irq_routines[irq]) {
                // Create a compatible registers structure for legacy handlers
                struct registers regs = {0};
                // Map 64-bit registers to 32-bit structure
                regs.eax = css->rax & 0xFFFFFFFF;
                regs.ebx = css->rbx & 0xFFFFFFFF;
                regs.ecx = css->rcx & 0xFFFFFFFF;
                regs.edx = css->rdx & 0xFFFFFFFF;
                regs.int_no = css->vector_number;
                regs.err_code = css->error_code;
                
                irq_routines[irq](&regs);
            }
            break;
        }
    }

    // Send EOI to master PIC
    port_byte_out(0x20, 0x20);

    return css;
}
