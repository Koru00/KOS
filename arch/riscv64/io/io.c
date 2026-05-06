#include "arch/riscv64/io/uart.h"
#include "lib/kprint.h"

void arch_putchar(char c)
{
#if defined(UART_OUTPUT)
	uart_putc(c);
#elif defined(SCREEN_OUTPUT)
	// Write char to the screen
#endif
}
