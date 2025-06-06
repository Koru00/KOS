#include "pic.h"
#include "ports.h" // You may need to define IO operations like outb, inb


void pic_initialize()
{
    // Send initialization command to PIC
    // Master and slave PIC remap
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20); // Master PIC vector offset
    port_byte_out(0xA1, 0x28); // Slave PIC vector offset
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);

    // Unmask IRQ1 (keyboard)
    port_byte_out(0xFD, 0x21); // 0xFD unmask IRQ1
}
