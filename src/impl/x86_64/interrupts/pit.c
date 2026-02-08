#include "pit.h"
#include "ports.h"
#include "print.h"
#include "irq.h"
#include "debug.h"

volatile unsigned long tick = 0; // Global tick counter

void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    port_byte_out(0x43, 0x36);             /* Set our command byte 0x36 */
    port_byte_out(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    port_byte_out(0x40, divisor >> 8);     /* Set high byte of divisor */
}




void timer_handler(struct registers *r)
{
    // Increment our 'tick count' 
    tick++;
    serial_print("zz");
    // Every 18 clocks (approximately 1 second), we will
    // display a message on the screen 
    if (tick % 18 == 0)
    {
        printf("One second has passed\n");
    }
}


/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void timer_install()
{
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
}

/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = tick + ticks;
    while(tick < eticks);
}