#include <stdint.h>
#include "ports.h"

void update_cursor(int row, int col) 
{
    uint16_t position = row * 80 + col;

    // Invia offset basso
    port_byte_out(0x3D4, 0x0F);
    port_byte_out(0x3D5, (uint8_t)(position & 0xFF));

    // Invia offset alto
    port_byte_out(0x3D4, 0x0E);
    port_byte_out(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}
