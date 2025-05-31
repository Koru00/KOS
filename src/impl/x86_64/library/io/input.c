#include "keyboard.h"
#include "vga.h"

static int caps = 0;
static int shifted = 0;
static int realesed = 0;

#define CONVERT_CAPS_CHAR(_HIGH, _LOW) ((caps == 1) ? _HIGH : _LOW)
#define CONVERT_SHIFT_CHAR(_HIGH, _LOW) ((shifted == 0) ? _HIGH : _LOW)

char keycode_to_ascii(keycode_t key)
{
    switch (key)
    {
    case KEY_A:
        return CONVERT_CAPS_CHAR('A', 'a');
    case KEY_B:
        return CONVERT_CAPS_CHAR('B', 'a');
    case KEY_C:
        return CONVERT_CAPS_CHAR('C', 'c');
    case KEY_D:
        return CONVERT_CAPS_CHAR('D', 'd');
    case KEY_E:
        return CONVERT_CAPS_CHAR('E', 'e');
    case KEY_F:
        return CONVERT_CAPS_CHAR('F', 'f');
    case KEY_G:
        return CONVERT_CAPS_CHAR('G', 'g');
    case KEY_H:
        return CONVERT_CAPS_CHAR('H', 'h');
    case KEY_I:
        return CONVERT_CAPS_CHAR('I', 'i');
    case KEY_J:
        return CONVERT_CAPS_CHAR('J', 'j');
    case KEY_K:
        return CONVERT_CAPS_CHAR('K', 'k');
    case KEY_L:
        return CONVERT_CAPS_CHAR('L', 'l');
    case KEY_M:
        return CONVERT_CAPS_CHAR('M', 'm');
    case KEY_N:
        return CONVERT_CAPS_CHAR('N', 'n');
    case KEY_O:
        return CONVERT_CAPS_CHAR('O', 'o');
    case KEY_P:
        return CONVERT_CAPS_CHAR('P', 'p');
    case KEY_Q:
        return CONVERT_CAPS_CHAR('Q', 'q');
    case KEY_R:
        return CONVERT_CAPS_CHAR('R', 'r');
    case KEY_S:
        return CONVERT_CAPS_CHAR('S', 's');
    case KEY_T:
        return CONVERT_CAPS_CHAR('T', 't');
    case KEY_U:
        return CONVERT_CAPS_CHAR('U', 'u');
    case KEY_V:
        return CONVERT_CAPS_CHAR('V', 'v');
    case KEY_W:
        return CONVERT_CAPS_CHAR('W', 'w');
    case KEY_X:
        return CONVERT_CAPS_CHAR('X', 'x');
    case KEY_Y:
        return CONVERT_CAPS_CHAR('Y', 'y');
    case KEY_Z:
        return CONVERT_CAPS_CHAR('Z', 'z');
    case KEY_1:
        return CONVERT_SHIFT_CHAR('1', '!');
    case KEY_2:
        return CONVERT_SHIFT_CHAR('2', '@');
    case KEY_3:
        return CONVERT_SHIFT_CHAR('3', '#');
    case KEY_4:
        return CONVERT_SHIFT_CHAR('4', '$');
    case KEY_5:
        return CONVERT_SHIFT_CHAR('5', '%');
    case KEY_6:
        return CONVERT_SHIFT_CHAR('6', '^');
    case KEY_7:
        return CONVERT_SHIFT_CHAR('7', '&');
    case KEY_8:
        return CONVERT_SHIFT_CHAR('8', '*');
    case KEY_9:
        return CONVERT_SHIFT_CHAR('9', '(');
    case KEY_0:
        return CONVERT_SHIFT_CHAR('0', ')');
    case KEY_SPACE:
        return ' ';
    case KEY_ENTER:
        vga_newline();
        break;
    case KEY_TAB:
        return '\t';
    case KEY_BACKSPACE:
        vga_backspace();
        break;
    case KEY_MINUS:
        return CONVERT_SHIFT_CHAR('-', '_');
    case KEY_EQUAL:
        return CONVERT_SHIFT_CHAR('=', '+');
    case KEY_LEFT_BRACKET:
        return CONVERT_SHIFT_CHAR('[', '{');
    case KEY_RIGHT_BRACKET:
        return CONVERT_SHIFT_CHAR(']', '}');
    case KEY_BACKSLASH:
        return CONVERT_SHIFT_CHAR('\\', '|');
    case KEY_SEMICOLON:
        return CONVERT_SHIFT_CHAR(';', ':');
    case KEY_APOSTROPHE:
        return CONVERT_SHIFT_CHAR('\'', '\"');
    case KEY_GRAVE:
        return CONVERT_SHIFT_CHAR('`', '~');
    case KEY_COMMA:
        return CONVERT_SHIFT_CHAR(',', '<');
    case KEY_PERIOD:
        return CONVERT_SHIFT_CHAR('.', '>');
    case KEY_SLASH:
        return CONVERT_SHIFT_CHAR('/', '?');
    default:
        return 0; // Not printable
    }
}

void caps_lock(keycode_t Key)
{
    if (Key == KEY_CAPS_LOCK)
    {
        caps = (caps == 1) ? 0 : 1;
    }
}

void shift(keycode_t Key)
{
    if (Key == KEY_LEFT_SHIFT || Key == KEY_RIGHT_SHIFT)
    {
        caps = (caps == 1) ? 0 : 1;
        shifted = (shifted == 1) ? 0 : 1;
    }
}

int input_kbd_listener(const keycode_t Key)
{
    caps_lock(Key);
    shift(Key);
    char c = keycode_to_ascii(Key);
    // This is for not printing not printable key
    if (c < 38)
    {
        return;
    }
    vga_write(c);
}
int input_kbd_listener_realesed(const keycode_t Key)
{
    shift(Key);
}

void init_input()
{
    int res;
    res = add_keyboard_listener(input_kbd_listener);
    res = add_keyboard_realese_listener(input_kbd_listener_realesed);
}