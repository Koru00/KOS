#pragma once

#include "hal/hal_core.h"
#include "../types.h"
#include "../process/process.h"

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

// VGA IOCTL commands
#define KOS_VGA_IOCTL_SET_COLOR     0x4600
#define KOS_VGA_IOCTL_GET_COLOR     0x4601
#define KOS_VGA_IOCTL_CLEAR_SCREEN  0x4602
#define KOS_VGA_IOCTL_SET_CURSOR    0x4603
#define KOS_VGA_IOCTL_GET_CURSOR    0x4604

// VGA status structure
typedef struct {
    uint32_t cursor_x;
    uint32_t cursor_y;
    uint8_t current_color;
    uint32_t width;
    uint32_t height;
    hal_bool_t initialized;
} kos_vga_status_t;

// VGA character structure
typedef struct kos_vga_char {
    u8 character;
    u8 color;  // Foreground (4 bits) | Background (4 bits)
} kos_vga_char_t;

// VGA screen dimensions
#define KOS_VGA_WIDTH 80
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

// =============================================================================
// VGA Driver API (HAL-based)
// =============================================================================

// Initialize VGA driver
hal_result_t vga_init(void);

// Write string to VGA display
hal_result_t vga_write_string(const char* str);

// Set VGA text color
hal_result_t vga_set_color(kos_vga_color_t fg, kos_vga_color_t bg);

// Clear VGA screen
hal_result_t vga_clear(void);

// =============================================================================
// Legacy VGA Driver Functions
// =============================================================================

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

// =============================================================================
// VGA Utility Macros
// =============================================================================

#define VGA_MAKE_COLOR(fg, bg) ((fg) | ((bg) << 4))
#define VGA_PRINT(str) vga_write_string(str)
#define VGA_PRINTLN(str) do { vga_write_string(str); vga_write_string("\n"); } while(0)
