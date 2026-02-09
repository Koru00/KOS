#pragma once

#include "driver.h"
#include "../types.h"

// =============================================================================
// KOS - Keyboard Driver Interface
// =============================================================================

// Key codes (subset for brevity)
typedef enum {
    KOS_KEY_UNKNOWN = 0,
    
    // Letters
    KOS_KEY_A, KOS_KEY_B, KOS_KEY_C, KOS_KEY_D, KOS_KEY_E, KOS_KEY_F,
    KOS_KEY_G, KOS_KEY_H, KOS_KEY_I, KOS_KEY_J, KOS_KEY_K, KOS_KEY_L,
    KOS_KEY_M, KOS_KEY_N, KOS_KEY_O, KOS_KEY_P, KOS_KEY_Q, KOS_KEY_R,
    KOS_KEY_S, KOS_KEY_T, KOS_KEY_U, KOS_KEY_V, KOS_KEY_W, KOS_KEY_X,
    KOS_KEY_Y, KOS_KEY_Z,
    
    // Numbers
    KOS_KEY_0, KOS_KEY_1, KOS_KEY_2, KOS_KEY_3, KOS_KEY_4,
    KOS_KEY_5, KOS_KEY_6, KOS_KEY_7, KOS_KEY_8, KOS_KEY_9,
    
    // Special keys
    KOS_KEY_ENTER, KOS_KEY_BACKSPACE, KOS_KEY_TAB, KOS_KEY_SPACE,
    KOS_KEY_ESCAPE, KOS_KEY_CAPS_LOCK,
    
    // Modifiers
    KOS_KEY_LEFT_SHIFT, KOS_KEY_RIGHT_SHIFT,
    KOS_KEY_LEFT_CTRL, KOS_KEY_RIGHT_CTRL,
    KOS_KEY_LEFT_ALT, KOS_KEY_RIGHT_ALT,
    
    // Navigation
    KOS_KEY_UP, KOS_KEY_DOWN, KOS_KEY_LEFT, KOS_KEY_RIGHT,
    KOS_KEY_HOME, KOS_KEY_END, KOS_KEY_PAGE_UP, KOS_KEY_PAGE_DOWN,
    
    // Function keys
    KOS_KEY_F1, KOS_KEY_F2, KOS_KEY_F3, KOS_KEY_F4, KOS_KEY_F5,
    KOS_KEY_F6, KOS_KEY_F7, KOS_KEY_F8, KOS_KEY_F9, KOS_KEY_F10,
    KOS_KEY_F11, KOS_KEY_F12
} kos_key_code_t;

// Key event types
typedef enum {
    KOS_KEY_EVENT_PRESS = 0,
    KOS_KEY_EVENT_RELEASE = 1
} kos_key_event_type_t;

// Key event structure
typedef struct kos_key_event {
    kos_key_code_t key_code;
    kos_key_event_type_t event_type;
    u64 timestamp;  // System tick when event occurred
} kos_key_event_t;

// Keyboard event listener function type
typedef void (*kos_keyboard_listener_t)(const kos_key_event_t* event);

// Keyboard driver private data
typedef struct kos_keyboard_private {
    kos_keyboard_listener_t listeners[KOS_MAX_KEYBOARD_LISTENERS];
    usize listener_count;
    u8 extended_mode;
    u8 last_scancode;
} kos_keyboard_private_t;

// Keyboard driver functions
kos_result_t kos_keyboard_init(kos_driver_t* driver);
kos_result_t kos_keyboard_shutdown(kos_driver_t* driver);
kos_result_t kos_keyboard_reset(kos_driver_t* driver);

// Event handling
kos_result_t kos_keyboard_register_listener(kos_driver_t* driver, kos_keyboard_listener_t listener);
kos_result_t kos_keyboard_unregister_listener(kos_driver_t* driver, kos_keyboard_listener_t listener);
void kos_keyboard_handle_interrupt(kos_driver_t* driver);

// Utility functions
const char* kos_key_to_string(kos_key_code_t key);
b8 kos_key_is_printable(kos_key_code_t key);
b8 kos_key_is_modifier(kos_key_code_t key);

// Driver instance declaration
extern kos_driver_t g_keyboard_driver;
