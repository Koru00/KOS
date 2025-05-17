#pragma once
#include <stdint.h>

// Keyboard scancode constants
#define KEYBOARD_KEY_A 0x1E
#define KEYBOARD_KEY_B 0x30
#define KEYBOARD_KEY_C 0x2E
#define KEYBOARD_KEY_ENTER 0x1C
#define KEYBOARD_KEY_BACKSPACE 0x0E

// Function to initialize the keyboard
void keyboard_init();

// Keyboard interrupt handler callback
void keyboard_callback();
