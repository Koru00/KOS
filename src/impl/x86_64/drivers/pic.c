#include "pic.h"
#include "io.h" // You may need to define IO operations like outb, inb


void pic_initialize()
{
    // Send initialization command to PIC
    // Master and slave PIC remap
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20); // Master PIC vector offset
    outb(0xA1, 0x28); // Slave PIC vector offset
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    // Unmask IRQ1 (keyboard)
    outb(0xFD, 0x21); // 0xFD unmask IRQ1
}
