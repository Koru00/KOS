#include "../../intf/library/io/print.h"
#include "../../intf/interrupts/idt.h"      // Include IDT header
#include "../../intf/drivers/keyboard.h" // Include keyboard header
#include "memset.h"
#include "pit.h"
#include "file.h"
#include "../../intf/debug/debug.h"
#include "power.h"
#include "../../intf/library/str/string.h"
#include "../../intf/interrupts/irq.h"
#include "../../intf/drivers/vga.h"
#include "../../intf/drivers/cursor.h"

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

char extstr[100];


int main_kbd_listener(const keycode_t Key)
{
    printf("A");
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

    // register the keyboards listeners
    int res;
    res = add_keyboard_listener(main_kbd_listener);

    if (res != 0) {
        printf("help me");
    }

    asm volatile("sti");
	
    init_vga();
    


    while (1)
    {
        asm("hlt");
    }
}

