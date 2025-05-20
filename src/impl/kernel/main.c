#include "print.h"
#include "idt.h"      // Include IDT header
#include "keyboard.h" // Include keyboard header
#include "memset.h"
#include "pit.h"
#include "file.h"
#include "debug.h"
#include "power.h"
#include "string.h"

void kernel_main()
{
    set_running(ON);

    asm volatile("cli");

    // initialize interrupt descriptor table
    init_idt();

    // initialize position-independent code
    pic_initialize();

  

    // initialize keyboard
    init_keyboard();


    while (get_running())
    {
    }
}
