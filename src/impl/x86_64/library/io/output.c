#include "keyboard.h"
#include "vga.h"

int output_kbd_listener(const keycode_t Key)
{
  switch (Key) {
    case KEY_ENTER:
    vga_write('q');
    break;
  }
}

void init_output()
{
  int res;
  res = add_keyboard_listener(output_kbd_listener); 
}

