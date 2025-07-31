#include "print.h"
#include "ports.h"
#include "isr.h"
#include "string.h"
#include "debug.h"
#include "keyboard.h"
#include "vga.h"
#include "cursor.h"
#include <stdint.h>

static keycode_t scancode_map[128] = {
    [0x01] = KEY_ESC,
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
    [0x0C] = KEY_MINUS,
    [0x0D] = KEY_EQUAL,
    [0x0E] = KEY_BACKSPACE,
    [0x0F] = KEY_TAB,
    [0x10] = KEY_Q,
    [0x11] = KEY_W,
    [0x12] = KEY_E,
    [0x13] = KEY_R,
    [0x14] = KEY_T,
    [0x15] = KEY_Y,
    [0x16] = KEY_U,
    [0x17] = KEY_I,
    [0x18] = KEY_O,
    [0x19] = KEY_P,
    [0x1A] = KEY_LEFT_BRACKET,
    [0x1B] = KEY_RIGHT_BRACKET,
    [0x1C] = KEY_ENTER,
    [0x1D] = KEY_LEFT_CTRL,
    [0x1E] = KEY_A,
    [0x1F] = KEY_S,
    [0x20] = KEY_D,
    [0x21] = KEY_F,
    [0x22] = KEY_G,
    [0x23] = KEY_H,
    [0x24] = KEY_J,
    [0x25] = KEY_K,
    [0x26] = KEY_L,
    [0x27] = KEY_SEMICOLON,
    [0x28] = KEY_APOSTROPHE,
    [0x29] = KEY_GRAVE,
    [0x2A] = KEY_LEFT_SHIFT,
    [0x2B] = KEY_BACKSLASH,
    [0x2C] = KEY_Z,
    [0x2D] = KEY_X,
    [0x2E] = KEY_C,
    [0x2F] = KEY_V,
    [0x30] = KEY_B,
    [0x31] = KEY_N,
    [0x32] = KEY_M,
    [0x33] = KEY_COMMA,
    [0x34] = KEY_PERIOD,
    [0x35] = KEY_SLASH,
    [0x36] = KEY_RIGHT_SHIFT,
    [0x37] = KEY_NUMPAD_MULTIPLY,
    [0x38] = KEY_LEFT_ALT,
    [0x39] = KEY_SPACE,
    [0x3A] = KEY_CAPS_LOCK,
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
    [0x45] = KEY_NUM_LOCK,
    [0x46] = KEY_SCROLL_LOCK,
    [0x47] = KEY_NUMPAD7,
    [0x48] = KEY_NUMPAD8,
    [0x49] = KEY_NUMPAD9,
    [0x4A] = KEY_NUMPAD_MINUS,
    [0x4B] = KEY_NUMPAD4,
    [0x4C] = KEY_NUMPAD5,
    [0x4D] = KEY_NUMPAD6,
    [0x4E] = KEY_NUMPAD_PLUS,
    [0x4F] = KEY_NUMPAD1,
    [0x50] = KEY_NUMPAD2,
    [0x51] = KEY_NUMPAD3,
    [0x52] = KEY_NUMPAD0,
    [0x53] = KEY_NUMPAD_DECIMAL,
    [0x57] = KEY_F11,
    [0x58] = KEY_F12,
};
keycode_t extended_scancode_map[94] = {
    [0x1C] = KEY_NUMPAD_ENTER,
    [0x1D] = KEY_RIGHT_CTRL,
    [0x35] = KEY_NUMPAD_DIVIDE,
    [0x38] = KEY_RIGHT_ALT,
    [0x47] = KEY_HOME,
    [0x48] = KEY_UP,
    [0x49] = KEY_PAGE_UP,
    [0x4B] = KEY_LEFT,
    [0x4D] = KEY_RIGHT,
    [0x4F] = KEY_END,
    [0x50] = KEY_DOWN,
    [0x51] = KEY_PAGE_DOWN,
    [0x52] = KEY_INSERT,
    [0x53] = KEY_DELETE,
    [0x5B] = KEY_LEFT_GUI,
    [0x5C] = KEY_RIGHT_GUI,
    [0x5D] = KEY_MENU,
};

// we have here the last 10 keys pressed
#define NUM_HIST_KEYS 10
static keycode_t last_keys[NUM_HIST_KEYS];
// we have only one listener for now
#define MAX_LISTENER 10
static key_pressed_fn _listener[MAX_LISTENER];
static key_realesed_fn _listener_realese[MAX_LISTENER];
static int index_listener = 0;
static int index_listener_realese = 0;

int extended;

int add_keyboard_listener(key_pressed_fn a_listener)
{
    if (index_listener >= MAX_LISTENER)
    {
        log_message(__PRETTY_FUNCTION__, "listeners are more then 10", LOG_ERROR);
        return 1;
    }
    _listener[index_listener++] = a_listener;
    return 0;
}
int add_keyboard_realese_listener(key_realesed_fn a_listener)
{
    if (index_listener_realese >= MAX_LISTENER)
    {
        log_message(__PRETTY_FUNCTION__, "listeners are more then 10", LOG_ERROR);
        return 1;
    }
    _listener_realese[index_listener_realese++] = a_listener;
    return 0;
}

int is_release;

void keyboard_callback()
{
    uint8_t scancode = port_byte_in(0x60);
    is_release = scancode & 0x80;

    keycode_t key_released = scancode_map[scancode & 0x7F];

    handle_scancode(scancode);
}

void handle_scancode(uint8_t sc)
{
    keycode_t key;

    if (sc == 0xE0)
    {
        extended = 1;
        return;
    }
    if (is_release)
    {
        sc = sc & 0x7F;
    }
    if (extended)
    {
        key = extended_scancode_map[sc];
        extended = 0;
    }
    else
    {
        key = scancode_map[sc];
    }

    if (_listener != NULL && !is_release)
    {
        for (int i = 0; i < index_listener; i++)
        {
            if (_listener[i] != NULL)
            {
                _listener[i](key);
            }
        }
    }
    else if (_listener_realese != NULL && is_release)
    {
        for (int i = 0; i < index_listener_realese; i++)
        {
            if (_listener_realese[i] != NULL)
            {
                _listener_realese[i](key);
            }
        }
    }

    // handle_key(key);
}

void handle_key(keycode_t key)
{
//    log_message(__PRETTY_FUNCTION__, "key pressed");
}

int key_pressed(const keycode_t Key)
{
}
