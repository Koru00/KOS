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

#define INITIAL_COLOR (VGA_COLOR_WHITE | VGA_COLOR_BLACK << 4)

// Color of the VGA
uint8_t color = INITIAL_COLOR;

struct Char empty;

void vga_refresh()
{
    update_cursor(row, col);
    init_empty(&empty);
}

void init_empty(struct Char *_char)
{
    _char->character = ' ';
    _char->color = color;
}

void clear_row(size_t row)
{
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
    vga_refresh();
}

void vga_write(char character)
{
    if (character == NULL)
    {
        return;
    }
    buffer[col + NUM_COLS * row] = (struct Char){
        character : (uint8_t)character,
        color : color,
    };
    col++;
    vga_refresh();
}

void vga_set_pos(int _col, int _row)
{
    if (_col > NUM_COLS || _col < 0 || _row > NUM_ROWS || _row < 0)
    {
        log_message(__PRETTY_FUNCTION__, "col or row out of bouds", LOG_WARNING);
        return;
    }
    col = _col;
    row = _row;
    vga_refresh();
}

void vga_color(uint8_t foreground, uint8_t background)
{
    color = foreground + (background << 4);
    vga_refresh();
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
    }
    else
    {
        // Scroll all rows up
        for (size_t r = 1; r < NUM_ROWS; r++)
        {
            for (size_t c = 0; c < NUM_COLS; c++)
            {
                struct Char character = buffer[c + NUM_COLS * r];
                buffer[c + NUM_COLS * (r - 1)] = character;
            }
        }

        // Clear the last row (now row NUM_ROWS - 1)
        clear_row(NUM_ROWS - 1);
    }

    vga_refresh();
}

void vga_backspace()
{
    if (col == 0 && row == 0)
    {
        log_message(__PRETTY_FUNCTION__, "out of the VGA bounds", LOG_INFO);
        return;
    }
    if (col == 0)
    {
        row--;
        col = vga_line_l();
    }
    else
    {
        col--;
    }
    buffer[col + NUM_COLS * row] = empty;
    vga_refresh();
}

int vga_line_l()
{
    int last_char_col;
    for (int col = NUM_COLS - 1; col >= 0; col--)
    {
        char c = buffer[row * NUM_COLS + col].character;
        if (c != ' ')
        {
            return col + 1;
        }
    }
    return 0;
}

int bounds[4];

int vga_bounds(int x, int y)
{
    if (x < bounds[0] || y < bounds[1] || x > bounds[2] || y > bounds[3])
    {
        return 1;
    }
    return 0;
}

void vga_set_bounds(int x, int y, int w, int h)
{
    bounds[0] = x;
    bounds[1] = y;
    bounds[2] = w;
    bounds[3] = h;
    log_message(__PRETTY_FUNCTION__, "updated bouds of the VGA", LOG_INFO);
}

void init_vga()
{
    memset(buffer, 0, SCREEN_SIZE * sizeof(struct Char));
    vga_refresh();
    vga_clear();
    vga_set_bounds(0, 0, NUM_COLS-1, NUM_ROWS-1);
//    log_message(__PRETTY_FUNCTION__, "vga initialized%d", LOG_INFO, NUM_COLS);
}
