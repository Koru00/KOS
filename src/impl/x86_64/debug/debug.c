#include "ports.h"
#include "string.h"
#include "debug.h"

#define SERIAL_PORT 0x3F8

const char* const LOG_TYPE_STR[] = {
	[LOG_INFO] = "INFO",
	[LOG_WARNING] = "WARNING",
	[LOG_ERROR] = "ERROR",
	[LOG_DEBUG] = "DEBUG",
	[LOG_FATAL] = "FATAL"
};

int log_id = 1;

void log_message(char* func, const char* message)
{
	serial_log();
	serial_square(func);
	serial_print(" -> ");
	serial_print(message);
	serial_print("\n");
}

void serial_write(char c) {
    while ((port_byte_in(SERIAL_PORT + 5) & 0x20) == 0);
    port_byte_out(SERIAL_PORT, c);
}

void serial_print(const char* str) {
	for (int i = 0; str[i] != '\0'; i++)
	{
		serial_write(str[i]);
	}
}

void serial_square(char* str)
{
	serial_write('[');
	serial_print(str);
	serial_write(']');
}

void serial_log()
{
    serial_square(int_to_str(log_id++));
}
