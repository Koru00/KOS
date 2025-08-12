#include "keyboard.h"
#include "vga.h"

// State variables
static int caps = 0;
static int left_shift = 0;
static int right_shift = 0;
static int shifted = 0;
static const int tab_dimension = 3;
static int printing = 0;
static keycode_t last_key = 0;

// Key mapping structure
typedef struct
{
    keycode_t key;
    char normal;
    char shifted;
    char caps;
} key_ascii_map_t;

// Table-driven key mapping for maintainability
static const key_ascii_map_t key_ascii_map[] = {
    {KEY_A, 'a', 'A', 'A'}, {KEY_B, 'b', 'B', 'B'}, {KEY_C, 'c', 'C', 'C'}, {KEY_D, 'd', 'D', 'D'}, {KEY_E, 'e', 'E', 'E'}, {KEY_F, 'f', 'F', 'F'}, {KEY_G, 'g', 'G', 'G'}, {KEY_H, 'h', 'H', 'H'}, {KEY_I, 'i', 'I', 'I'}, {KEY_J, 'j', 'J', 'J'}, {KEY_K, 'k', 'K', 'K'}, {KEY_L, 'l', 'L', 'L'}, {KEY_M, 'm', 'M', 'M'}, {KEY_N, 'n', 'N', 'N'}, {KEY_O, 'o', 'O', 'O'}, {KEY_P, 'p', 'P', 'P'}, {KEY_Q, 'q', 'Q', 'Q'}, {KEY_R, 'r', 'R', 'R'}, {KEY_S, 's', 'S', 'S'}, {KEY_T, 't', 'T', 'T'}, {KEY_U, 'u', 'U', 'U'}, {KEY_V, 'v', 'V', 'V'}, {KEY_W, 'w', 'W', 'W'}, {KEY_X, 'x', 'X', 'X'}, {KEY_Y, 'y', 'Y', 'Y'}, {KEY_Z, 'z', 'Z', 'Z'}, {KEY_1, '1', '!', '1'}, {KEY_2, '2', '@', '2'}, {KEY_3, '3', '#', '3'}, {KEY_4, '4', '$', '4'}, {KEY_5, '5', '%', '5'}, {KEY_6, '6', '^', '6'}, {KEY_7, '7', '&', '7'}, {KEY_8, '8', '*', '8'}, {KEY_9, '9', '(', '9'}, {KEY_0, '0', ')', '0'}, {KEY_MINUS, '-', '_', '-'}, {KEY_EQUAL, '=', '+', '='}, {KEY_LEFT_BRACKET, '[', '{', '['}, {KEY_RIGHT_BRACKET, ']', '}', ']'}, {KEY_BACKSLASH, '\\', '|', '\\'}, {KEY_SEMICOLON, ';', ':', ';'}, {KEY_APOSTROPHE, '\'', '"', '\''}, {KEY_GRAVE, '`', '~', '`'}, {KEY_COMMA, ',', '<', ','}, {KEY_PERIOD, '.', '>', '.'}, {KEY_SLASH, '/', '?', '/'}, {KEY_NUMPAD0, '0', '0', '0'}, {KEY_NUMPAD1, '1', '1', '1'}, {KEY_NUMPAD2, '2', '2', '2'}, {KEY_NUMPAD3, '3', '3', '3'}, {KEY_NUMPAD4, '4', '4', '4'}, {KEY_NUMPAD5, '5', '5', '5'}, {KEY_NUMPAD6, '6', '6', '6'}, {KEY_NUMPAD7, '7', '7', '7'}, {KEY_NUMPAD8, '8', '8', '8'}, {KEY_NUMPAD9, '9', '9', '9'}, {KEY_SPACE, ' ', ' ', ' '},
    // Add more mappings as needed
};

// Lookup function for key mapping
static char lookup_ascii(keycode_t key)
{
    for (unsigned i = 0; i < sizeof(key_ascii_map) / sizeof(key_ascii_map[0]); ++i)
    {
        if (key_ascii_map[i].key == key)
        {
            if (key >= KEY_A && key <= KEY_Z)
            {
                // For letters, caps/shift logic
                if (caps ^ shifted)
                    return key_ascii_map[i].caps;
                else
                    return key_ascii_map[i].normal;
            }
            else
            {
                // For non-letters, shift logic
                return shifted ? key_ascii_map[i].shifted : key_ascii_map[i].normal;
            }
        }
    }
    return 0;
}

// Convert keycode to ASCII and handle special keys
char keycode_to_ascii(keycode_t key)
{
    if (!printing)
        return 0;

    switch (key)
    {
    case KEY_ENTER:
        vga_newline();
        return 0;
    case KEY_TAB:
        tab();
        return 0;
    case KEY_BACKSPACE:
        vga_backspace();
        return 0;
    default:
        return lookup_ascii(key);
    }
}

// Toggle caps lock state
void caps_lock(keycode_t Key)
{
    if (Key == KEY_CAPS_LOCK)
        caps = !caps;
}

// Handle shift key press
void shift_press(keycode_t Key)
{
    if (Key == KEY_LEFT_SHIFT)
        left_shift = 1;
    if (Key == KEY_RIGHT_SHIFT)
        right_shift = 1;
    shifted = left_shift || right_shift;
}

// Handle shift key release
void shift_release(keycode_t Key)
{
    if (Key == KEY_LEFT_SHIFT)
        left_shift = 0;
    if (Key == KEY_RIGHT_SHIFT)
        right_shift = 0;
    shifted = left_shift || right_shift;
}

// Handle key press events
void input_kbd_listener(const keycode_t Key)
{
    caps_lock(Key);
    shift_press(Key);
    char c = keycode_to_ascii(Key);
    if (c == 0)
        return;
    vga_write(c);
    last_key = Key;
}

// Handle key release events
void input_kbd_listener_released(const keycode_t Key)
{
    shift_release(Key);
}

// Enable or disable printing
void kb_print(int state)
{
    printing = state;
}

// Initialize input system
void init_input()
{
    add_keyboard_listener(input_kbd_listener);
    add_keyboard_release_listener(input_kbd_listener_released);
}

// Insert spaces for tab key
void tab()
{
    for (int i = 0; i < tab_dimension; i++)
        vga_write(' ');
}

// Get last pressed key (blocking)
keycode_t getch()
{
    // TODO: Consider a non-blocking or interrupt-driven approach
    while (last_key == 0)
        ;
    keycode_t ret = last_key;
    last_key = 0;
    return ret;
}