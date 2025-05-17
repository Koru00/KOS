#include "print.h"
#include "ports.h"
#include "isr.h" // You need to implement IRQ handler registration
#include "timer.h"
#include "string.h"

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
void keyboard_callback()
{
    uint8_t scancode = port_byte_in(0x60);
    bool is_release = scancode & 0x80;
    uint8_t key = scancode & 0x7F;

    if (scancode == 0xE0)
    {
        extended_code = 1;
        port_byte_out(0x20, 0x20);
        return;
    }

    if (extended_code)
    {
        // Handle extended keys (like arrow keys)
        switch (key)
        {
        case 0x48:
            print_str("[UP]");
            break;
        case 0x50:
            print_str("[DOWN]");
            break;
        case 0x4B:
            print_str("[LEFT]");
            break;
        case 0x4D:
            print_str("[RIGHT]");
            break;
        default:
            // If not handled, ignore the scancode
            break;
        }

        // Clear extended code AFTER handling
        extended_code = 0;
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

    if (!is_release && !key_states[key] && !extended_code)
    {
        key_states[key] = 1;

        if (key < 128 && scancode_to_char[key])
        {
            char c = shift_held ? scancode_to_char_shift[key] : scancode_to_char[key];
            if (c)
            {
                print_char(c);
            }
        }
    }
    else if (!is_release && key_states[key] && !extended_code)
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
