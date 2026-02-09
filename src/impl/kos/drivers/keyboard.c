#include <stdint.h>
#include <stddef.h>

// =============================================================================
// KOS - Keyboard Driver Implementation (Standalone)
// =============================================================================

// Basic types
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uintptr_t uptr;
typedef size_t    usize;
typedef _Bool    b8;

typedef enum {
    KOS_SUCCESS = 0,
    KOS_ERROR_INVALID_PARAM = -1,
    KOS_ERROR_OUT_OF_MEMORY = -2,
    KOS_ERROR_NOT_FOUND = -3
} kos_result_t;

// Keyboard key codes (simplified)
typedef enum {
    KOS_KEY_UNKNOWN = 0,
    KOS_KEY_A, KOS_KEY_B, KOS_KEY_C, KOS_KEY_D, KOS_KEY_E, KOS_KEY_F,
    KOS_KEY_G, KOS_KEY_H, KOS_KEY_I, KOS_KEY_J, KOS_KEY_K, KOS_KEY_L,
    KOS_KEY_M, KOS_KEY_N, KOS_KEY_O, KOS_KEY_P, KOS_KEY_Q, KOS_KEY_R,
    KOS_KEY_S, KOS_KEY_T, KOS_KEY_U, KOS_KEY_V, KOS_KEY_W, KOS_KEY_X,
    KOS_KEY_Y, KOS_KEY_Z,
    KOS_KEY_0, KOS_KEY_1, KOS_KEY_2, KOS_KEY_3, KOS_KEY_4,
    KOS_KEY_5, KOS_KEY_6, KOS_KEY_7, KOS_KEY_8, KOS_KEY_9,
    KOS_KEY_ENTER, KOS_KEY_BACKSPACE, KOS_KEY_TAB, KOS_KEY_SPACE,
    KOS_KEY_ESCAPE, KOS_KEY_CAPS_LOCK,
    KOS_KEY_LEFT_SHIFT, KOS_KEY_RIGHT_SHIFT,
    KOS_KEY_LEFT_CTRL, KOS_KEY_RIGHT_CTRL,
    KOS_KEY_LEFT_ALT, KOS_KEY_RIGHT_ALT,
    KOS_KEY_UP, KOS_KEY_DOWN, KOS_KEY_LEFT, KOS_KEY_RIGHT,
    KOS_KEY_HOME, KOS_KEY_END, KOS_KEY_PAGE_UP, KOS_KEY_PAGE_DOWN,
    KOS_KEY_F1, KOS_KEY_F2, KOS_KEY_F3, KOS_KEY_F4, KOS_KEY_F5,
    KOS_KEY_F6, KOS_KEY_F7, KOS_KEY_F8, KOS_KEY_F9, KOS_KEY_F10,
    KOS_KEY_F11, KOS_KEY_F12
} kos_key_code_t;

// Keyboard event types
typedef enum {
    KOS_KEY_EVENT_PRESS = 0,
    KOS_KEY_EVENT_RELEASE = 1
} kos_key_event_type_t;

// Keyboard event structure
typedef struct kos_key_event {
    kos_key_code_t key_code;
    kos_key_event_type_t event_type;
    u64 timestamp;
} kos_key_event_t;

// Keyboard event listener function type
typedef void (*kos_keyboard_listener_t)(const kos_key_event_t* event);

// Keyboard driver private data
typedef struct kos_keyboard_private {
    kos_keyboard_listener_t listeners[10];
    usize listener_count;
    u8 extended_mode;
    u8 last_scancode;
} kos_keyboard_private_t;

// Driver structure (simplified)
typedef struct kos_driver {
    const char* name;
    void* private_data;
} kos_driver_t;

// Scancode to key mapping (simplified)
static const kos_key_code_t scancode_map[128] = {
    [0x01] = KOS_KEY_ESCAPE,
    [0x02] = KOS_KEY_1, [0x03] = KOS_KEY_2, [0x04] = KOS_KEY_3, [0x05] = KOS_KEY_4,
    [0x06] = KOS_KEY_5, [0x07] = KOS_KEY_6, [0x08] = KOS_KEY_7, [0x09] = KOS_KEY_8,
    [0x0A] = KOS_KEY_9, [0x0B] = KOS_KEY_0, [0x0E] = KOS_KEY_BACKSPACE, [0x0F] = KOS_KEY_TAB,
    [0x1C] = KOS_KEY_ENTER, [0x1D] = KOS_KEY_LEFT_CTRL,
    [0x1E] = KOS_KEY_A, [0x1F] = KOS_KEY_S, [0x20] = KOS_KEY_D, [0x21] = KOS_KEY_F,
    [0x22] = KOS_KEY_G, [0x23] = KOS_KEY_H, [0x24] = KOS_KEY_J, [0x25] = KOS_KEY_K,
    [0x26] = KOS_KEY_L, [0x2A] = KOS_KEY_LEFT_SHIFT, [0x2C] = KOS_KEY_Z, [0x2D] = KOS_KEY_X,
    [0x2E] = KOS_KEY_C, [0x2F] = KOS_KEY_V, [0x30] = KOS_KEY_B, [0x31] = KOS_KEY_N,
    [0x32] = KOS_KEY_M, [0x38] = KOS_KEY_LEFT_ALT, [0x39] = KOS_KEY_SPACE,
    [0x3A] = KOS_KEY_CAPS_LOCK,
    [0x3B] = KOS_KEY_F1, [0x3C] = KOS_KEY_F2, [0x3D] = KOS_KEY_F3, [0x3E] = KOS_KEY_F4,
    [0x3F] = KOS_KEY_F5, [0x40] = KOS_KEY_F6, [0x41] = KOS_KEY_F7, [0x42] = KOS_KEY_F8,
    [0x43] = KOS_KEY_F9, [0x44] = KOS_KEY_F10, [0x57] = KOS_KEY_F11, [0x58] = KOS_KEY_F12
};

// Driver instance
kos_driver_t g_keyboard_driver;

// Private data
static kos_keyboard_private_t g_keyboard_private;

// Port I/O functions
static inline u8 kos_port_byte_in(u16 port) {
    u8 result;
    asm volatile("in %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

kos_result_t kos_keyboard_init(kos_driver_t* driver) {
    if (!driver) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    // Initialize private data
    g_keyboard_private.listener_count = 0;
    g_keyboard_private.extended_mode = 0;
    driver->private_data = &g_keyboard_private;
    
    return KOS_SUCCESS;
}

kos_result_t kos_keyboard_shutdown(kos_driver_t* driver) {
    if (!driver) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    // Clear all listeners
    g_keyboard_private.listener_count = 0;
    
    return KOS_SUCCESS;
}

kos_result_t kos_keyboard_register_listener(kos_driver_t* driver, kos_keyboard_listener_t listener) {
    if (!driver || !listener) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    if (g_keyboard_private.listener_count >= 10) {
        return KOS_ERROR_OUT_OF_MEMORY;
    }
    
    g_keyboard_private.listeners[g_keyboard_private.listener_count++] = listener;
    
    return KOS_SUCCESS;
}

void kos_keyboard_handle_interrupt(kos_driver_t* driver) {
    if (!driver) {
        return;
    }
    
    // Read scancode from keyboard controller
    u8 scancode = kos_port_byte_in(0x60);
    b8 is_release = scancode & 0x80;
    
    // Handle extended scancode prefix
    if (scancode == 0xE0) {
        g_keyboard_private.extended_mode = 1;
        g_keyboard_private.last_scancode = scancode;
        return;
    }
    
    // Determine key code
    kos_key_code_t key_code = KOS_KEY_UNKNOWN;
    u8 scancode_index = scancode & 0x7F;
    
    if (g_keyboard_private.extended_mode) {
        // Extended scancode (simplified)
        g_keyboard_private.extended_mode = 0;
    } else {
        // Regular scancode
        if (scancode_index < 128) {
            key_code = scancode_map[scancode_index];
        }
    }
    
    // Create key event
    kos_key_event_t event = {
        .key_code = key_code,
        .event_type = is_release ? KOS_KEY_EVENT_RELEASE : KOS_KEY_EVENT_PRESS,
        .timestamp = 0  // TODO: Get from timer
    };
    
    // Notify listeners
    for (usize i = 0; i < g_keyboard_private.listener_count; i++) {
        if (g_keyboard_private.listeners[i]) {
            g_keyboard_private.listeners[i](&event);
        }
    }
    
    g_keyboard_private.last_scancode = scancode;
}
