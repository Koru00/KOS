#include "print.h"
#include "commands.h"
#include "string.h"
#include "heap_alloc.h"
#include "cursor.h"
#include "vga.h"
#include "debug.h"
#include "stdarg.h"
#include "input.h"
#include "keyboard.h"


void printf(const char *str, ...)
{
    va_list args;
    va_start(args, str);

    for (size_t i = 0; str[i] != '\0'; i++)
    {
        char character = (uint8_t)str[i];

        switch (character)
        {
        case '\0':
            goto end;
            break;
        case '\n':
            vga_newline();
            break;
        case '\t':
            tab();
            break;
        case '%':
        {
            char type = (uint8_t)str[++i];
            switch (type)
            {
            case '%':
                vga_write('%');
                break;
            case 'c':
            {
                char c = (char)(va_arg(args, int));
                vga_write(c);
                break;
            }
            case 's':
            {
                char *s = va_arg(args, char *);
                if (s) {
                    // Write string directly to avoid recursion
                    for (char *p = s; *p; p++) {
                        vga_write(*p);
                    }
                }
                break;
            }
            case 'd':
            {
                int d = va_arg(args, int);
                char *num_str = int_to_str(d);
                for (char *p = num_str; *p; p++) {
                    vga_write(*p);
                }
                break;
            }
            case 'x':
            {
                unsigned int num = va_arg(args, unsigned int);
                // Simple hex conversion
                char hex_str[20];
                char *ptr = hex_str + 19;
                *ptr = '\0';
                ptr--;
                
                if (num == 0) {
                    *ptr = '0';
                    vga_write('0');
                } else {
                    while (num > 0) {
                        int digit = num % 16;
                        *ptr = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
                        num /= 16;
                        ptr--;
                    }
                    ptr++;
                    while (*ptr) {
                        vga_write(*ptr++);
                    }
                }
                break;
            }
            default:
                vga_write('%');
                vga_write(type);
                break;
            }
            break;
        }
        default:
            vga_write(character);
            break;
        }
    }

end:
    // asm volatile("sti");
    va_end(args);
}



void scanf(char *output)
{
    char scan[25];
    int length = vga_line_l();
    for (int i = 0; i < length; i++)
    {
        scan[i] = vga_read(i, 0);
    }
    // Fix: copy the string instead of trying to malloc with char*
    for (int i = 0; i < length && scan[i] != '\0'; i++) {
        output[i] = scan[i];
    }
    output[length] = '\0'; // Null terminate
}

// DEPRECATED
/*
#define SCREEN_SIZE (NUM_ROWS * NUM_COLS)

const static size_t NUM_COLS = 80;
const static size_t NUM_ROWS = 25;

struct Char
{
    uint8_t character;
    uint8_t color;
};

struct Char *buffer = (struct Char *)0xb8000;
size_t col = 0;
size_t row = 0;
uint8_t color = PRINT_COLOR_WHITE | PRINT_COLOR_BLACK << 4;

void clear_row(size_t row)
{
    col = 0;
    struct Char empty = (struct Char){
        character : ' ',
        color : color,
    };
    for (size_t col = 0; col < NUM_COLS; col++)
    {
        buffer[col + NUM_COLS * row] = empty;
    }
}

void print_clear()
{
    for (size_t i = 0; i < NUM_ROWS; i++)
    {
        clear_row(i);
    };
    col = 0;
    row = 0;
    update_screen();
}

void reload_screen()
{
    for (size_t row = 0; row < NUM_ROWS; row++)
    {
        for (size_t col = 0; col < NUM_COLS; col++)
        {

            buffer[row * NUM_COLS + col] = (struct Char){
        character : buffer[row * NUM_COLS + col].character,
        color : color,
    };
        }
    }
    update_screen();
}

void update_screen()
{
    update_cursor(row, col);
}

int get_line_length(int row)
{
    // geting how long is the line
    int last_char_col;
    for (int col = NUM_COLS; col >= 0; col--)
    {
        struct Char c = buffer[row * NUM_COLS + col];
        if (c.character != ' ')
        {
            return col + 1;
        }
    }
    return 0;
}

char *get_line_text(int row, int max_col)
{
    if (max_col == -1)
    {
        max_col = NUM_COLS;
    }
    int length = get_line_length(row);
    if (length > max_col)
    {
        length = max_col;
    }

    char *buffer_out = malloc(length + 1);
    for (int col = 0; col < length; col++)
    {
        struct Char line_character = buffer[row * NUM_COLS + col];
        buffer_out[col] = line_character.character;
    }
    buffer_out[length] = '\0'; // Null terminator
    return buffer_out;
}

void print_newline()
{
    col = 0;
    if (row < NUM_ROWS - 1)
    {
        row++;
        update_cursor(row, col);
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
    update_screen();
}

void print_backspace()
{
    struct Char empty = (struct Char){
        character : ' ',
        color : color,
    };
    if (row == 0 && col == 0)
    {
        return;
    }
    if (col == 0)
    {
        row -= 1;
        col = get_line_length(row);
    }
    else
    {
        col -= 1;
        buffer[col + NUM_COLS * row] = empty;
    }
    update_screen();
}

void print_char(char character)
{
    if (character == '\n')
    {
        if (get_line_text(row, 1)[0] == '$')
        {
            commands(get_line_text(row, -1));
        }
        else
        {
            print_newline();
        }
        return;
    }
    else if (character == '\b')
    {
        print_backspace();
        return;
    }

    if (col > NUM_COLS)
    {
        print_newline();
    }
    col = get_cursor_pos_col();
    row = get_cursor_pos_row();
    buffer[col + NUM_COLS * row] = (struct Char){
        character : (uint8_t)character,
        color : color,
    };
    col++;
    update_screen();
}

void print_str(char *str)
{
    for (size_t i = 0; 1; i++)
    {
        char character = (uint8_t)str[i];

        if (character == '\0')
        {
            return;
        }
        print_char(character);
    }
    update_screen();
}

void print_set_color(uint8_t foreground, uint8_t background)
{
    color = foreground + (background << 4);
}

void print_logo()
{
    print_newline();
    print_set_color(PRINT_COLOR_LIGHT_GREEN, PRINT_COLOR_BLACK);
    print_str(" __    __   ______    ______  ");
    print_newline();
    print_str("/  |  /  | /      \  /      \ ");
    print_newline();
    print_str("$$ | /$$/ /$$$$$$  |/$$$$$$  |");
    print_newline();
    print_str("$$ |/$$/  $$ |  $$ |$$ \__$$/ ");
    print_newline();
    print_str("$$  $$<   $$ |  $$ |$$      \ ");
    print_newline();
    print_str("$$$$$  \  $$ |  $$ | $$$$$$  |");
    print_newline();
    print_str("$$ |$$  \ $$ \__$$ |/  \__$$ |");
    print_newline();
    print_str("$$ | $$  |$$    $$/ $$    $$/ ");
    print_newline();
    print_str("$$/   $$/  $$$$$$/   $$$$$$/  ");
}
*/
