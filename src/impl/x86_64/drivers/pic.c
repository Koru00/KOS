#include "pic.h"
#include "io.h"  // You may need to define IO operations like outb, inb

void pic_initialize() {
    // Send initialization command to PIC
    outb(0x11, 0x20); // Start initialization (ICW1)
    outb(0x20, 0x21); // Remap IRQ0 to 0x20
    outb(0x04, 0x21); // IRQ2 to IRQ1 (cascade)
    outb(0x01, 0x21); // ICW4 - environment info

    // Unmask IRQ1 (keyboard)
    outb(0xFD, 0x21); // 0xFD unmask IRQ1
}
