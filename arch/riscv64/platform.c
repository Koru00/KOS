#include <arch/general/platform.h>

#include <arch/riscv64/io/uart.h>

#define UART_TEXT_MODE

void arch_init()
{
	uart_init();
}

void put_char(char c)
{
#ifdef UART_TEXT_MODE
	uart_putc(c);
#elif VGA_TEXT_MODE

#endif
}
