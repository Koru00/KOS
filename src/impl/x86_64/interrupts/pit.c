#include "pit.h"
#include "ports.h"
#include "print.h"
#include "irq.h"
#include "logging.h"

#define PIT_FREQUENCY 1193180
#define TIMER_DEFAULT_HZ 1000

volatile unsigned long g_system_ticks = 0;

void timer_phase(int hz) {
    if (hz <= 0) hz = TIMER_DEFAULT_HZ;
    
    int divisor = PIT_FREQUENCY / hz;
    port_byte_out(0x43, 0x36);
    port_byte_out(0x40, divisor & 0xFF);
    port_byte_out(0x40, divisor >> 8);
    
    LOG_DEBUG("Timer configured for %d Hz", hz);
}

void timer_callback(void) {
    g_system_ticks++;
    
    // Log every second (assuming 1000 Hz)
    if (g_system_ticks % 1000 == 0) {
        LOG_INFO("System uptime: %lu seconds", g_system_ticks / 1000);
    }
}

void timer_handler(struct registers *r) {
    (void)r; // Unused parameter
    timer_callback();
}

void timer_install(void) {
    irq_install_handler(0, timer_handler);
    LOG_DEBUG("Timer interrupt handler installed");
}

void timer_wait(int ticks) {
    if (ticks <= 0) return;
    
    unsigned long target_ticks = g_system_ticks + ticks;
    while (g_system_ticks < target_ticks) {
        asm volatile("hlt");
    }
}

unsigned long timer_get_ticks(void) {
    return g_system_ticks;
}