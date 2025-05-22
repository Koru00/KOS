#include "ports.h"
#include "string.h"

#define SERIAL_PORT 0x3F8

int log_id = 1;

void serial_write(char c) {
    while ((port_byte_in(SERIAL_PORT + 5) & 0x20) == 0);
    port_byte_out(SERIAL_PORT, c);
}

void serial_print(const char* str) {
    while (*str)
        serial_write(*str++);
    serial_write('\n');
}

void serial_log(char* str)
{
    serial_print(int_to_str(log_id++));
}