#define UART0 0x10000000

void uart_putc(char c) {
    *(volatile char*)UART0 = c;
}
