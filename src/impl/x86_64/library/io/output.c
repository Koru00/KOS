#include "keyboard.h"
#include "vga.h"

int output_kbd_listener(const keycode_t Key)
{
}

void init_output()
{
  add_keyboard_listener(output_kbd_listener);
}
