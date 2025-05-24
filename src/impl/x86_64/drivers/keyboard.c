#include "print.h"
#include "ports.h"
#include "isr.h" // You need to implement IRQ handler registration
#include "string.h"
#include "debug.h"
#include "keyboard.h"
#include "vga.h"

static keycode_t scancode_map[128] = {
    [0x1E] = KEY_A,
    [0x30] = KEY_B,
    [0x2E] = KEY_C,
    [0x20] = KEY_D,
    [0x12] = KEY_E,
    [0x21] = KEY_F,
    [0x22] = KEY_G,
    [0x23] = KEY_H,
    [0x17] = KEY_I,
    [0x24] = KEY_J,
    [0x25] = KEY_K,
    [0x26] = KEY_L,
    [0x32] = KEY_M,
    [0x31] = KEY_N,
    [0x18] = KEY_O,
    [0x19] = KEY_P,
    [0x10] = KEY_Q,
    [0x13] = KEY_R,
    [0x1F] = KEY_S,
    [0x14] = KEY_T,
    [0x16] = KEY_U,
    [0x2F] = KEY_V,
    [0x11] = KEY_W,
    [0x2D] = KEY_X,
    [0x15] = KEY_Y,
    [0x2C] = KEY_Z,

    [0x02] = KEY_1,
    [0x03] = KEY_2,
    [0x04] = KEY_3,
    [0x05] = KEY_4,
    [0x06] = KEY_5,
    [0x07] = KEY_6,
    [0x08] = KEY_7,
    [0x09] = KEY_8,
    [0x0A] = KEY_9,
    [0x0B] = KEY_0,

    [0x1C] = KEY_ENTER,
    [0x01] = KEY_ESC,
    [0x0E] = KEY_BACKSPACE,
    [0x0F] = KEY_TAB,
    [0x39] = KEY_SPACE,
    [0x0C] = KEY_MINUS,
    [0x0D] = KEY_EQUAL,
    [0x1A] = KEY_LEFT_BRACKET,
    [0x1B] = KEY_RIGHT_BRACKET,
    [0x2B] = KEY_BACKSLASH,
    [0x27] = KEY_SEMICOLON,
    [0x28] = KEY_APOSTROPHE,
    [0x29] = KEY_GRAVE,
    [0x33] = KEY_COMMA,
    [0x34] = KEY_PERIOD,
    [0x35] = KEY_SLASH,

    [0x3A] = KEY_CAPS_LOCK,
    [0x2A] = KEY_LEFT_SHIFT,
    [0x36] = KEY_RIGHT_SHIFT,
    [0x1D] = KEY_LEFT_CTRL,
    [0x38] = KEY_LEFT_ALT,

    [0x3B] = KEY_F1,
    [0x3C] = KEY_F2,
    [0x3D] = KEY_F3,
    [0x3E] = KEY_F4,
    [0x3F] = KEY_F5,
    [0x40] = KEY_F6,
    [0x41] = KEY_F7,
    [0x42] = KEY_F8,
    [0x43] = KEY_F9,
    [0x44] = KEY_F10,
    [0x57] = KEY_F11,
    [0x58] = KEY_F12,
};

static char* key_map[128] = {
    [0x1E] = "a",
    [0x30] = "b",
    [0x2E] = "c",
    [0x20] = "d",
    [0x12] = "e",
    [0x21] = "f",
    [0x22] = "g",
    [0x23] = "h",
    [0x17] = "i",
    [0x24] = "j",
    [0x25] = "k",
    [0x26] = "l",
    [0x32] = "m",
    [0x31] = "n",
    [0x18] = "o",
    [0x19] = "p",
    [0x10] = "q",
    [0x13] = "r",
    [0x1F] = "s",
    [0x14] = "t",
    [0x16] = "u",
    [0x2F] = "v",
    [0x11] = "w",
    [0x2D] = "x",
    [0x15] = "y",
    [0x2C] = "z",

    [0x02] = "1",
    [0x03] = "2",
    [0x04] = "3",
    [0x05] = "4",
    [0x06] = "5",
    [0x07] = "6",
    [0x08] = "7",
    [0x09] = "8",
    [0x0A] = "9",
    [0x0B] = "0",

    [0x1C] = "\n",
    [0x01] = KEY_ESC,
    [0x0E] = KEY_BACKSPACE,
    [0x0F] = "\t",
    [0x39] = " ",
    [0x0C] = "-",
    [0x0D] = "=",
    [0x1A] = "<",
    [0x1B] = ">",
    [0x2B] = "\\",
    [0x27] = ";",
    [0x28] = "\'",
    [0x29] = KEY_GRAVE,
    [0x33] = ",",
    [0x34] = KEY_PERIOD,
    [0x35] = "/",

    [0x3A] = KEY_CAPS_LOCK,
    [0x2A] = KEY_LEFT_SHIFT,
    [0x36] = KEY_RIGHT_SHIFT,
    [0x1D] = KEY_LEFT_CTRL,
    [0x38] = KEY_LEFT_ALT,

    [0x3B] = KEY_F1,
    [0x3C] = KEY_F2,
    [0x3D] = KEY_F3,
    [0x3E] = KEY_F4,
    [0x3F] = KEY_F5,
    [0x40] = KEY_F6,
    [0x41] = KEY_F7,
    [0x42] = KEY_F8,
    [0x43] = KEY_F9,
    [0x44] = KEY_F10,
    [0x57] = KEY_F11,
    [0x58] = KEY_F12,
};

static char* key_map_caps[128] = {
    [0x1E] = 'A',
    [0x30] = 'B',
    [0x2E] = 'C',
    [0x20] = 'D',
    [0x12] = 'E',
    [0x21] = 'F',
    [0x22] = 'G',
    [0x23] = 'H',
    [0x17] = 'I',
    [0x24] = 'J',
    [0x25] = 'K',
    [0x26] = 'L',
    [0x32] = 'M',
    [0x31] = 'N',
    [0x18] = 'O',
    [0x19] = 'P',
    [0x10] = 'Q',
    [0x13] = 'R',
    [0x1F] = 'S',
    [0x14] = 'T',
    [0x16] = 'U',
    [0x2F] = 'V',
    [0x11] = 'W',
    [0x2D] = 'X',
    [0x15] = 'Y',
    [0x2C] = 'Z',

    [0x02] = '!',
    [0x03] = '@',
    [0x04] = '#',
    [0x05] = '$',
    [0x06] = '%',
    [0x07] = '^',
    [0x08] = '&',
    [0x09] = '*',
    [0x0A] = '(',
    [0x0B] = ')',

    [0x1C] = '\n',
    [0x01] = KEY_ESC,
    [0x0E] = KEY_BACKSPACE,
    [0x0F] = '\t',
    [0x39] = ' ',
    [0x0C] = '-',
    [0x0D] = '=',
    [0x1A] = '<',
    [0x1B] = '>',
    [0x2B] = '\\',
    [0x27] = ';',
    [0x28] = '\'',
    [0x29] = KEY_GRAVE,
    [0x33] = ',',
    [0x34] = KEY_PERIOD,
    [0x35] = '/',

    [0x3A] = KEY_CAPS_LOCK,
    [0x2A] = KEY_LEFT_SHIFT,
    [0x36] = KEY_RIGHT_SHIFT,
    [0x1D] = KEY_LEFT_CTRL,
    [0x38] = KEY_LEFT_ALT,

    [0x3B] = KEY_F1,
    [0x3C] = KEY_F2,
    [0x3D] = KEY_F3,
    [0x3E] = KEY_F4,
    [0x3F] = KEY_F5,
    [0x40] = KEY_F6,
    [0x41] = KEY_F7,
    [0x42] = KEY_F8,
    [0x43] = KEY_F9,
    [0x44] = KEY_F10,
    [0x57] = KEY_F11,
    [0x58] = KEY_F12,
};

int write = 0;
int caps = 0;

void keyboard_callback()
{
    uint8_t scancode = port_byte_in(0x60);
    int is_release = scancode & 0x80;

    keycode_t key = scancode_map[scancode];
    keycode_t key_released = scancode_map[scancode & 0x7F];

    if (!is_release)
    {
        switch (key)
        {
        case KEY_I:
            if (!write)
            {
                write = 1;
                return;
            }
            break;
        case KEY_ESC:
            if (write)
            {
                write = 0;
                return;
            }
            break;
        case KEY_LEFT_SHIFT:
            if (!caps)
            {
                caps = 1;
                return;
            }
            break;
        }
        if (write && !caps)
        {
            printf(key_map[scancode]);
        }
        else if (write && caps)
        {
            printf(key_map_caps[scancode]);
        }
    }
    else
    {
        switch (key_released)
        {
        case KEY_LEFT_SHIFT:
            if (caps)
            {
                caps = 0;
                return;
            }
            break;
        }
    }
}
