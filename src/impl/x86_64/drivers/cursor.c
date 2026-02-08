#include <stdint.h>
#include "cursor.h"
#include "ports.h"
#include "keyboard.h"
#include "print.h"
#include "debug.h"
#include "string.h"
#include "vga.h"


int pos_row = 0;
int pos_col = 0;

// It say if the cursor can move
int cursor_move = 1;

void cursor_kbd_listener(const keycode_t Key)
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

void init_cursor()
{
    add_keyboard_listener(cursor_kbd_listener);
}