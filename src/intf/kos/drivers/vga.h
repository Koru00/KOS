#pragma once

#include "driver.h"
#include "../types.h"

// =============================================================================
// KOS - VGA Display Driver Interface
// =============================================================================

// VGA colors
typedef enum {
    KOS_VGA_COLOR_BLACK = 0,
    KOS_VGA_COLOR_BLUE = 1,
    KOS_VGA_COLOR_GREEN = 2,
    KOS_VGA_COLOR_CYAN = 3,
    KOS_VGA_COLOR_RED = 4,
    KOS_VGA_COLOR_MAGENTA = 5,
    KOS_VGA_COLOR_BROWN = 6,
    KOS_VGA_COLOR_LIGHT_GRAY = 7,
    KOS_VGA_COLOR_DARK_GRAY = 8,
    KOS_VGA_COLOR_LIGHT_BLUE = 9,
    KOS_VGA_COLOR_LIGHT_GREEN = 10,
    KOS_VGA_COLOR_LIGHT_CYAN = 11,
    KOS_VGA_COLOR_LIGHT_RED = 12,
    KOS_VGA_COLOR_LIGHT_MAGENTA = 13,
    KOS_VGA_COLOR_YELLOW = 14,
    KOS_VGA_COLOR_WHITE = 15
} kos_vga_color_t;

// VGA character structure
typedef struct kos_vga_char {
    u8 character;
    u8 color;  // Foreground (4 bits) | Background (4 bits)
} kos_vga_char_t;

// VGA screen dimensions
#define KOS_VGA_WIDTH  80
#define KOS_VGA_HEIGHT 25
#define KOS_VGA_SIZE   (KOS_VGA_WIDTH * KOS_VGA_HEIGHT)

// VGA driver private data
typedef struct kos_vga_private {
    volatile kos_vga_char_t* buffer;
    u16 cursor_x, cursor_y;
    u8 current_color;
    u8 default_color;
    b8 cursor_enabled;
} kos_vga_private_t;

// VGA driver functions
kos_result_t kos_vga_init(kos_driver_t* driver);
kos_result_t kos_vga_shutdown(kos_driver_t* driver);
kos_result_t kos_vga_reset(kos_driver_t* driver);

// Display operations
void kos_vga_clear(kos_driver_t* driver);
void kos_vga_clear_row(kos_driver_t* driver, u16 row);
void kos_vga_put_char(kos_driver_t* driver, u8 character);
void kos_vga_put_char_at(kos_driver_t* driver, u8 character, u16 x, u16 y);
void kos_vga_put_string(kos_driver_t* driver, const char* string);
void kos_vga_put_string_at(kos_driver_t* driver, const char* string, u16 x, u16 y);

// Color management
void kos_vga_set_color(kos_driver_t* driver, u8 foreground, u8 background);
void kos_vga_set_default_color(kos_driver_t* driver, u8 foreground, u8 background);
u8 kos_vga_make_color(kos_vga_color_t foreground, kos_vga_color_t background);

// Cursor management
void kos_vga_set_cursor(kos_driver_t* driver, u16 x, u16 y);
void kos_vga_get_cursor(kos_driver_t* driver, u16* x, u16* y);
void kos_vga_enable_cursor(kos_driver_t* driver, b8 enabled);
void kos_vga_update_cursor(kos_driver_t* driver);

// Utility functions
void kos_vga_scroll_up(kos_driver_t* driver);
void kos_vga_scroll_down(kos_driver_t* driver);
void kos_vga_newline(kos_driver_t* driver);
void kos_vga_tab(kos_driver_t* driver);
void kos_vga_backspace(kos_driver_t* driver);

// Driver instance declaration
extern kos_driver_t g_vga_driver;
