/* arch/riscv64/io/uart.c */

#include "arch/riscv64/io/uart.h"

#define UART0 0x10000000

// Register offsets for 16550A
#define UART_REG_THR 0  // Transmit Holding Register (Write)
#define UART_REG_LSR 5  // Line Status Register (Read)

// Bit 5 of LSR is the "Transmit Holding Register Empty" flag
#define LSR_THRE (1 << 5)

void uart_init() {
    volatile unsigned char* base = (volatile unsigned char*)UART0;
    base[2] = 0xC7; // Enable FIFO, clear them, set interrupt trigger level
}

void uart_putc(char c) {
    volatile unsigned char* base = (volatile unsigned char*)UART0;

    // Wait until the UART says it's ready to receive a new character
    while ((base[UART_REG_LSR] & LSR_THRE) == 0) {
        // Do nothing, just loop (polling)
    }

    // Now it's safe to write the character
    base[UART_REG_THR] = c;
}
