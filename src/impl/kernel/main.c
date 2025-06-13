#include "print.h"
#include "idt.h"
#include "../../intf/drivers/keyboard.h"
#include "memset.h"
#include "pit.h"
#include "file.h"
#include "../../intf/debug/debug.h"
#include "power.h"
#include "string.h"
#include "../../intf/interrupts/irq.h"
#include "../../intf/drivers/vga.h"
#include "../../intf/drivers/cursor.h"
#include "input.h"
#include "ports.h"
#include "heap_alloc.h"
#include "strcat.h"
#include "output.h"

#define NUM_GDT_ENTRIES 5

const uint64_t num_gdt_entries = NUM_GDT_ENTRIES;
uint64_t gdt_entries[NUM_GDT_ENTRIES];

struct GDTR
{
    uint16_t limit;
    uint64_t address;
} __attribute__((packed));

struct GDTR example_gdtr;

void load_gdt()
{
    asm("lgdt %0" : : "m"(example_gdtr));
}

void flush_gdt()
{
    asm volatile("\
        mov $0x10, %ax \n\
        mov %ax, %ds \n\
        mov %ax, %es \n\
        mov %ax, %fs \n\
        mov %ax, %gs \n\
        mov %ax, %ss \n\
        \n\
        pop %rdi \n\
        push $0x8 \n\
        push %rdi \n\
        lretq \n\
    ");
}

extern void irq_remap(void);

char extstr[100];


int main_kbd_listener(const keycode_t Key)
{
    switch (Key)
    {
    case KEY_0: // Block cursor (start: 0, end: 15)
        port_byte_out(0x3D4, 0x0A); // Select Cursor Start Register
        port_byte_out(0x3D5, 0x00); // Start at scanline 0
        port_byte_out(0x3D4, 0x0B); // Select Cursor End Register
        port_byte_out(0x3D5, 0x0F); // End at scanline 15
        break;

    case KEY_1: // Underline cursor (start: 13, end: 15)
        port_byte_out(0x3D4, 0x0A);
        port_byte_out(0x3D5, 0x0D);
        port_byte_out(0x3D4, 0x0B);
        port_byte_out(0x3D5, 0x0F);
        break;

    case KEY_2: // Half-height cursor (start: 7, end: 15)
        port_byte_out(0x3D4, 0x0A);
        port_byte_out(0x3D5, 0x07);
        port_byte_out(0x3D4, 0x0B);
        port_byte_out(0x3D5, 0x0F);
        break;

    case KEY_3: // Invisible cursor (set start > end or bit 5 = 1)
        port_byte_out(0x3D4, 0x0A);
        port_byte_out(0x3D5, 0x20); // Bit 5 = 1 hides cursor
        break;

    default:
        break;
    }
}


void kernel_main()
{

    example_gdtr.limit = NUM_GDT_ENTRIES * sizeof(uint64_t) - 1;
    example_gdtr.address = (uint64_t)gdt_entries;

    // null descriptor, required to be here.
    gdt_entries[0] = 0;

    uint64_t kernel_code = 0;
    kernel_code |= 0b1011 << 8; // type of selector
    kernel_code |= 1 << 12;     // not a system descriptor
    kernel_code |= 0 << 13;     // DPL field = 0
    kernel_code |= 1 << 15;     // present
    kernel_code |= 1 << 21;     // long-mode segment

    gdt_entries[1] = kernel_code << 32;

    uint64_t kernel_data = 0;
    kernel_data |= 0b0011 << 8; // type of selector
    kernel_data |= 1 << 12;     // not a system descriptor
    kernel_data |= 0 << 13;     // DPL field = 0
    kernel_data |= 1 << 15;     // present
    kernel_data |= 1 << 21;     // long-mode segment
    gdt_entries[2] = kernel_data << 32;

    uint64_t user_code = kernel_code | (3 << 13);
    gdt_entries[3] = user_code;

    uint64_t user_data = kernel_data | (3 << 13);
    gdt_entries[4] = user_data;

    load_gdt();

    // flush_gdt( );
    asm volatile("cli");

    init_idt();

    irq_remap();

    pic_initialize();

    timer_phase(100);

    timer_install();

    init_cursor();

    // register the keyboards listeners
    int res;
    res = add_keyboard_listener(main_kbd_listener);
    

    asm volatile("sti");
	
    init_vga();

    init_input();
    kb_print(1);
    
    init_output();

    while (1)
    {
        asm("hlt");
    }
}

