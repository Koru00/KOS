#include "ports.h"

#define SERIAL_PORT 0x3F8

void serial_write(char c) {
    while ((port_byte_in(SERIAL_PORT + 5) & 0x20) == 0);
    port_byte_out(SERIAL_PORT, c);
}

void serial_print(const char* str) {
    while (*str)
        serial_write(*str++);
}
