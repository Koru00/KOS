#include <stdint.h>
#include "ports.h"
#include "keyboard.h"
#include "print.h"
#include "cursor.h"
#include "debug.h"

int pos_row = 0;
int pos_col = 0;

// It say if the cursor can move
int cursor_move = 1;



void update_cursor(int row, int col)
{
    uint16_t position = row * 80 + col;

    pos_col = col;
    pos_row = row;

    port_byte_out(0x3D4, 0x0F);
    port_byte_out(0x3D5, (uint8_t)(position & 0xFF));

    port_byte_out(0x3D4, 0x0E);
    port_byte_out(0x3D5, (uint8_t)((position >> 8) & 0xFF));
    log_message(__PRETTY_FUNCTION__, "updated position of the cursor");
}
void move_cursor(Dir dir)
{
    if (cursor_move)
    {
        switch (dir)
        {
        case 0:
            update_cursor(pos_row - 1, pos_col);
            break;
        case 1:
            update_cursor(pos_row + 1, pos_col);
            break;
        case 2:
            update_cursor(pos_row, pos_col - 1);
            break;
        case 3:
            update_cursor(pos_row, pos_col + 1);
            break;
        default:
            break;
       }
    }
}

int get_cursor_pos_col()
{
    return pos_col;
}

int get_cursor_pos_row()
{
    return pos_row;
}

int get_cursor_pos()
{
    return pos_row * 80 + pos_col;
}

void able_cursor(int is_able)
{
    cursor_move = is_able;
}
