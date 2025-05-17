#include "timer.h"
#include "ports.h"
#include "isr.h"

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIT_FREQ     1193182

static volatile uint32_t timer_ticks = 0;

volatile uint32_t delay_target_tick = 0;
volatile int delay_done = 0;

static void pit_set_frequency(uint32_t freq)
{
    uint16_t divisor = (uint16_t)(PIT_FREQ / freq);

    // Command byte:
    // Channel 0, Access mode: lobyte/hibyte, Mode 3 (square wave), binary
    port_byte_out(PIT_COMMAND, 0x36);

    // Send divisor low byte
    port_byte_out(PIT_CHANNEL0, divisor & 0xFF);

    // Send divisor high byte
    port_byte_out(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
}

static void pit_handler(registers_t *regs)
{
    timer_ticks++;
    // You can add periodic tasks here

    // Send End of Interrupt (EOI) to PIC
    port_byte_out(0x20, 0x20);
}

void pit_install()
{
    pit_set_frequency(100); // 100 Hz = 10 ms per tick
    isr_register_handler(32, pit_handler); // IRQ0 is interrupt 32
}

uint32_t get_timer_ticks()
{
    return timer_ticks;
}


// Wait for 'ms' milliseconds using timer ticks (assuming 100 ticks per second)
void delay_ms(uint32_t ms)
{
    uint32_t start = get_timer_ticks();
    uint32_t wait_ticks = (ms + 9) / 10; // rounds up

    while ((get_timer_ticks() - start) < wait_ticks) {
        // wait here
        // optionally add `__asm__("hlt");` if you want to be CPU-friendly
    }
}



