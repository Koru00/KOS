/* kernel/main.c */

#include "arch/riscv64/io/uart.h"

void kernel_main(void) {

    // No check, just fire a byte at the memory address
    *(volatile char*)0x10000000 = 'H';
    *(volatile char*)0x10000000 = 'E';
    *(volatile char*)0x10000000 = 'L';
    *(volatile char*)0x10000000 = 'L';
    *(volatile char*)0x10000000 = 'O';
    
    uart_init();
    uart_putc('H');
    uart_putc('i');
    uart_putc('\n');

    while (1);

}
