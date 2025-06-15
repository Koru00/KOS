#include <stdint.h>
#include "ports.h"
#include "keyboard.h"
#include "print.h"
#include "cursor.h"
#include "debug.h"
#include "string.h"
#include "vga.h"

int pos_row = 0;
int pos_col = 0;

// It say if the cursor can move
int cursor_move = 1;

int cursor_kbd_listener(const keycode_t Key)
{
    switch (Key)
    {
    case KEY_UP:
        move_cursor(UP);
        break;
    case KEY_DOWN:
        move_cursor(DOWN);
        break;
    case KEY_LEFT:
        move_cursor(LEFT);
        break;
    case KEY_RIGHT:
        move_cursor(RIGHT);
        break;
    
    default:
        break;
    }
}

void init_cursor()
{
    int res;
    res = add_keyboard_listener(cursor_kbd_listener);
}

void update_cursor(int row, int col)
{
    if (vga_bounds(col, row))
    {
        return;
    }
    uint16_t position = row * 80 + col;

    pos_col = col;
    pos_row = row;

    port_byte_out(0x3D4, 0x0F);
    port_byte_out(0x3D5, (uint8_t)(position & 0xFF));

    port_byte_out(0x3D4, 0x0E);
    port_byte_out(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}
void move_cursor(Dir dir)
{
    if (cursor_move)
    {
        switch (dir)
        {
        case UP:
            update_cursor(pos_row - 1, pos_col);
            break;
        case DOWN:
            update_cursor(pos_row + 1, pos_col);
            break;
        case LEFT:
            update_cursor(pos_row, pos_col - 1);
            break;
        case RIGHT:
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

void cursor_shape(Shape shape)
{
  switch (shape) {
    case BLOCK: // Block cursor (start: 0, end: 15)
        port_byte_out(0x3D4, 0x0A); // Select Cursor Start Register
        port_byte_out(0x3D5, 0x00); // Start at scanline 0
        port_byte_out(0x3D4, 0x0B); // Select Cursor End Register
        port_byte_out(0x3D5, 0x0F); // End at scanline 15
        break;

    case UNDERLINE: // Underline cursor (start: 13, end: 15)
        port_byte_out(0x3D4, 0x0A);
        port_byte_out(0x3D5, 0x0D);
        port_byte_out(0x3D4, 0x0B);
        port_byte_out(0x3D5, 0x0F);
        break;

    case HALF: // Half-height cursor (start: 7, end: 15)
        port_byte_out(0x3D4, 0x0A);
        port_byte_out(0x3D5, 0x07);
        port_byte_out(0x3D4, 0x0B);
        port_byte_out(0x3D5, 0x0F);
        break;

    case INVISIBLE: // Invisible cursor (set start > end or bit 5 = 1)
        port_byte_out(0x3D4, 0x0A);
        port_byte_out(0x3D5, 0x20); // Bit 5 = 1 hides cursor
     log_message(__PRETTY_FUNCTION__, "cursor set to Invisible",LOG_INFO ); 
        break; 
    default:
      break;
  }
}
