#include "vga.h"
#include "debug.h"
#include "memset.h"
#include "cursor.h"

// Declaring the max nunber of col and row of the VGA
const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;
// Screen area
#define SCREEN_SIZE (NUM_ROWS * NUM_COLS)


// Structure for the characters on the VGA
struct Char
{
    uint8_t character;
    uint8_t color;
};

// Pointer to the VGA space on memory
struct Char *buffer = (struct Char *)0xb8000;

// Curent col and row (they are unsigned)
size_t col = 0;
size_t row = 0;

// Color of the VGA
uint8_t color = VGA_COLOR_WHITE | VGA_COLOR_BLACK << 4;



void clear_row(size_t row)
{
    struct Char empty = (struct Char){
        character : ' ',
        color : color,
    };
    for (size_t col = 0; col < NUM_COLS; col++)
    {
        buffer[col + NUM_COLS * row] = empty;
    }
}

void vga_clear()
{
    for (size_t i = 0; i < NUM_ROWS; i++)
    {
        clear_row(i);
    };
    row = 0;
    col = 0;
}

void vga_write(char character)
{
    buffer[col + NUM_COLS * row] = (struct Char){
        character : (uint8_t)character,
        color : color,
    };
    col++;
}

void vga_set_pos(int _col, int _row)
{
	if (_col > NUM_COLS || _col < 0 || _row > NUM_ROWS || _row < 0)
	{
		log_message(__PRETTY_FUNCTION__, "col or row out of bouds");
		return;
	}
	col = _col;
	row = _row;
}

void vga_color(uint8_t foreground, uint8_t background)
{
    color = foreground + (background << 4);
}

char vga_read(int col, int row)
{
    return buffer[col + NUM_COLS * row].character;
}

void vga_newline()
{
    col = 0;
    if (row < NUM_ROWS - 1)
    {
        row++;
        return;
    }
    for (size_t row = 1; row < NUM_ROWS; row++)
    {
        for (size_t col = 0; col < NUM_COLS; col++)
        {
            struct Char character = buffer[col + NUM_COLS * row];
            buffer[col + NUM_COLS * (row - 1)] = character;
        }
    }
    clear_row(NUM_COLS - 1);
}

int vga_line_l()
{
    int last_char_col;
    for (int col = NUM_COLS-1; col >= 0; col--)
    {
        char c = buffer[row * NUM_COLS + col].character;
        if (c != ' ')
        {
            return col + 1;
        }
    }
    return 0;
}

void init_vga() {
    memset(buffer, 0, SCREEN_SIZE * sizeof(struct Char));
    vga_clear();
    update_cursor(0,0);
}