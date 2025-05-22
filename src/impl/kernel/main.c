#include "print.h"
#include "idt.h"      // Include IDT header
#include "keyboard.h" // Include keyboard header
#include "memset.h"
#include "pit.h"
#include "file.h"
#include "debug.h"
#include "power.h"
#include "string.h"
#include "irq.h"

// #define PIT_ABLE 1

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
    //    serial_print("after flush.");
    asm volatile("cli");

    init_idt();

    irq_remap();

    pic_initialize();

    timer_phase(100);

    timer_install();

    asm volatile("sti");

    // this is what you want
    while (1)
    {
        asm("hlt");
    }
}

void kernel_main_KORU()
{
    serial_print("1.");
    set_running(ON);
    serial_print("2.");

    asm volatile("cli");
    serial_print("3.");

    // initialize interrupt descriptor table
    init_idt();
    serial_print("4.");

    // initialize position-independent code
    pic_initialize();
    serial_print("5.");

    // irq_install();
    serial_print("6.");
    timer_phase(100);
    serial_print("7.");

    timer_install();
    serial_print("v.");

    asm volatile("sti");

    serial_print("9.");

    // initialize keyboard
    // init_keyboard();

    while (get_running())
    {
    }
}
