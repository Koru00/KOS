#include "print.h"
#include "idt.h"      // Include IDT header
#include "keyboard.h" // Include keyboard header
#include "memset.h"
#include "timer.h"
#include "file.h"
#include "debug.h"

void kernel_main()
{
    asm volatile("cli");

    // initialize interrupt descriptor table
    init_idt();

    // initialize position-independent code
    pic_initialize();

    // initialize keyboard
    init_keyboard();

    // Timer initialized
    pit_install();
    
    serial_print("print in the serial log");

    while (1)
    {
        keyboard_callback();
        move_cursor();
    }
    
}

