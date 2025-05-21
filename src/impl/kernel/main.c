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

//#define PIT_ABLE 1

void kernel_main()
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
    
    //irq_install();
    serial_print("6.");
    timer_phase(100);
    serial_print("7.");
    
    timer_install();
    serial_print("v.");
    
    #ifdef PIT_ABLE
    asm volatile("sti");
    #endif
    serial_print("9.");
  

    // initialize keyboard
    //init_keyboard();


    while (get_running())
    {
    }
}
