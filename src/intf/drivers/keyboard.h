#pragma once
#include <stdint.h>

// Keyboard scancode constants
#define KEYBOARD_KEY_A 0x1E
#define KEYBOARD_KEY_B 0x30
#define KEYBOARD_KEY_C 0x2E
#define KEYBOARD_KEY_ENTER 0x1C
#define KEYBOARD_KEY_BACKSPACE 0x0E


typedef enum keycode
{
    // Letters
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F,
    KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L,
    KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R,
    KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X,
    KEY_Y, KEY_Z,

    // Numbers
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4,
    KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

    // Function Keys
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
    KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,
    KEY_F11, KEY_F12, KEY_F13, KEY_F14, KEY_F15,
    KEY_F16, KEY_F17, KEY_F18, KEY_F19, KEY_F20,
    KEY_F21, KEY_F22, KEY_F23, KEY_F24,

    // Control Keys
    KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, KEY_LEFT_GUI,
    KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, KEY_RIGHT_ALT, KEY_RIGHT_GUI,

    // Navigation Keys
    KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    KEY_HOME, KEY_END, KEY_PAGE_UP, KEY_PAGE_DOWN,
    KEY_INSERT, KEY_DELETE,

    // Special Characters
    KEY_MINUS, KEY_EQUAL, KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET,
    KEY_BACKSLASH, KEY_SEMICOLON, KEY_APOSTROPHE, KEY_GRAVE,
    KEY_COMMA, KEY_PERIOD, KEY_SLASH,

    // Control + Lock
    KEY_ESC, KEY_TAB, KEY_CAPS_LOCK, KEY_ENTER,
    KEY_BACKSPACE, KEY_SPACE,
    KEY_PRINT_SCREEN, KEY_SCROLL_LOCK, KEY_PAUSE_BREAK,

    // Numpad
    KEY_NUM_LOCK,
    KEY_NUMPAD_DIVIDE, KEY_NUMPAD_MULTIPLY,
    KEY_NUMPAD_MINUS, KEY_NUMPAD_PLUS,
    KEY_NUMPAD_ENTER, KEY_NUMPAD_DECIMAL,
    KEY_NUMPAD0, KEY_NUMPAD1, KEY_NUMPAD2, KEY_NUMPAD3,
    KEY_NUMPAD4, KEY_NUMPAD5, KEY_NUMPAD6, KEY_NUMPAD7,
    KEY_NUMPAD8, KEY_NUMPAD9,

    // Menu
    KEY_MENU,

    KEY_UNKNOWN // Fallback for unmapped/unknown keys
} keycode_t;

// Function to initialize the keyboard
void keyboard_init();

// Keyboard interrupt handler callback
void keyboard_callback();
void key_realesed(const keycode_t Key);

// this is the declaration of a pointer to function which gets a key.
typedef int (*key_pressed_fn)(const keycode_t Key);
typedef int (*key_realesed_fn)(const keycode_t Key);

// this is a function to add a listener to the keyboard.
// for now only one listener is accepted.
// if you add more the function will fail.
void add_keyboard_listener(key_pressed_fn a_listener);
void add_keyboard_realese_listener(key_realesed_fn a_listener);

char keycode_to_ascii(keycode_t key);
