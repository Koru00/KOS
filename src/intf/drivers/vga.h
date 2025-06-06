#pragma once

#include <stdint.h>
#include <stddef.h>

enum
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GRAY = 7,
    VGA_COLOR_DARK_GRAY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_PINK = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
};

void init_vga();
void clear_row(size_t row);
void vga_clear();
void vga_color(uint8_t foreground, uint8_t background);
void vga_write(char character);
char vga_read(int col, int row);
void vga_newline();
int vga_line_l();
void vga_set_pos(int _col, int _row);
void vga_backspace();
void vga_set_bounds(int x, int y, int w, int h);
int vga_bounds(int x, int y);