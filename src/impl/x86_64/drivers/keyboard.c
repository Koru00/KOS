#include "print.h"
#include "ports.h"
#include "isr.h" // You need to implement IRQ handler registration
#include "string.h"
#include "debug.h"
#include "keyboard.h"

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

// Basic US QWERTY scancode map
char scancode_to_char[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', // 0x00 - 0x0F
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',  // 0x10 - 0x1D
    0,                                                                       // Control
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',             // 0x1E - 0x29
    0,                                                                       // Left Shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',                  // 0x2A - 0x35
    0, '*', 0, ' ',                                                          // 0x36 - 0x39 (Right Shift, KP *, Alt, Space)
    0,                                                                       // Caps Lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                            // F1–F10                                  // 0x3A - 0x43
    0,                                                                       // Num Lock
    0,                                                                       // Scroll Lock
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',         // Numpad keys 0x47 - 0x53
    0, 0, 0,                                                                 // Unused
    0, 0, 0, 0, 0, 0, 0,                                                     // F11–F12 and beyond (usually extended)
    0, 0, 0, 0, 0, 0, 0, 0                                                   // Fill rest as 0
};

char scancode_to_char_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', // 0x00 - 0x0F
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',  // 0x10 - 0x1D
    0,                                                                       // Control
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',             // 0x1E - 0x29
    0,                                                                       // Left Shift
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',                   // 0x2A - 0x35
    0, '*', 0, ' ',                                                          // 0x36 - 0x39
    0,                                                                       // Caps Lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                                            // F1–F10
    0,                                                                       // Num Lock
    0,                                                                       // Scroll Lock
    '7', '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.',         // Numpad (same as no shift)
    0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0};

typedef int bool;

bool key_states[128] = {0};
bool shift_held = 0; // globals
int continius = 0;
bool extended_code = 0;

int extended_key_callback(uint8_t key)
{
    // Handle extended keys (like arrow keys)
    switch (key)
    {
    case 0x48:
        return 1; // Up
        break;
    case 0x50:
        return 2; // Down
        break;
    case 0x4B:
        return 3; // Left
        break;
    case 0x4D:
        return 4; // Right
        break;
    default:
        // If not handled, ignore the scancode
        break;
    }

    // Clear extended code AFTER handling
    extended_code = 0;
    port_byte_out(0x20, 0x20);
}

// to improve
int keyboard_callback()
{
    uint8_t scancode = port_byte_in(0x60);
    bool is_release = scancode & 0x80;
    uint8_t key = scancode;

    if (scancode == 0xE0)
    {
        extended_code = 1;
        port_byte_out(0x20, 0x20);
        return;
    }

    if (key == 0x2A || key == 0x36) // Shift keys
    {
        shift_held = !is_release;
        key_states[key] = !is_release;
        port_byte_out(0x20, 0x20);
        return;
    }

    if (!is_release && !key_states[key])
    {
        key_states[key] = 1;

        if (scancode_map[scancode] == KEY_A)
        {
            serial_print("it work");
        }
        /*
        if (extended_code)
        {
            return extended_key_callback(key);
        }
        else if (key < 128 && scancode_to_char[key])
        {
            char c = shift_held ? scancode_to_char_shift[key] : scancode_to_char[key];
            if (c)
            {
                printf(c);
            }
        }*/
    }
    else if (!is_release && key_states[key])
    {

        if (!continius)
        {
            for (volatile int i = 0; i < 20000000; i++)
            {
                if (is_release)
                {
                    break;
                    continius = 0;
                }
            }
            continius = 1;
        }
        if (continius)
        {
            for (volatile int i = 0; i < 8000000; i++)
            {
                if (is_release)
                {
                    break;
                    continius = 0;
                }
            }
        }
        key_states[key] = 0;
    }
    if (is_release)
    {
        key_states[key] = 0;
        continius = 0;
    }

    port_byte_out(0x20, 0x20);
}

void init_keyboard()
{
    isr_register_handler(33, keyboard_callback); // IRQ1 is interrupt 33
}
