#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../utils/string.h"
#include "hal/hal_interface.h"
#include "hal/hal_types.h"
#include "hal/hal_cpu.h"
#include "hal/hal_interrupt_simple.h"
#include "hal/hal_memory_simple.h"
#include "hal/hal_timer_simple.h"
#include "hal/hal_platform_simple.h"

// =============================================================================
// KOS - Kernel Core Implementation (Fixed)
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
    KOS_ERROR_NOT_FOUND = -3,
    KOS_ERROR_INVALID_STATE = -8,
    KOS_ERROR_NOT_IMPLEMENTED = -7
} kos_result_t;

// Kernel configuration
typedef struct kos_kernel_config {
    b8 enable_serial_logging;
    b8 enable_vga_logging;
    b8 enable_keyboard;
    b8 enable_timer;
} kos_kernel_config_t;

// Kernel state (simplified)
typedef struct kos_kernel_state {
    u64 start_timestamp;
    u64 uptime_ticks;
    u32 boot_flags;
} kos_kernel_state_t;

// Global kernel state
kos_kernel_state_t g_kernel_state;

// Default kernel configuration
const kos_kernel_config_t g_default_kernel_config = {
    .enable_serial_logging = true,
    .enable_vga_logging = false,
    .enable_keyboard = true,
    .enable_timer = true
};

// GDT functions (simplified)
typedef struct {
    u64 entries[5];
    struct {
        u16 limit;
        u64 base;
    } pointer;
} kos_gdt_t;

extern kos_result_t kos_gdt_init(kos_gdt_t* gdt);
extern void kos_gdt_load(const kos_gdt_t* gdt);
extern kos_result_t kos_gdt_setup_kernel_segments(kos_gdt_t* gdt);
extern kos_result_t kos_gdt_setup_user_segments(kos_gdt_t* gdt);
extern void kos_gdt_flush(void);

// Memory functions (simplified)
typedef struct {
    void* heap_start;
    u32 heap_size;
} memory_manager_t;

extern kos_result_t kos_memory_init(memory_manager_t* manager);
extern memory_manager_t g_memory_manager;

// Driver functions (simplified)
typedef struct kos_driver {
    const char* name;
    void* private_data;
} kos_driver_t;

// Simplified driver manager functions
kos_result_t kos_driver_manager_init(void* manager) {
    (void)manager; // Suppress unused parameter
    return KOS_SUCCESS;
}

kos_result_t kos_driver_register(void* manager, kos_driver_t* driver) {
    (void)manager; // Suppress unused parameter
    (void)driver;  // Suppress unused parameter
    return KOS_SUCCESS;
}

kos_result_t kos_driver_init_all(void* manager) {
    (void)manager; // Suppress unused parameter
    return KOS_SUCCESS;
}

// Keyboard driver
extern kos_driver_t g_keyboard_driver;
extern kos_result_t kos_keyboard_init(kos_driver_t* driver);
extern void kos_keyboard_handle_interrupt(kos_driver_t* driver);

// Kernel functions
kos_result_t kos_kernel_init(const kos_kernel_config_t* config) {
    if (!config) {
        config = &g_default_kernel_config;
    }
    
    // Initialize kernel state
    kos_memset(&g_kernel_state, 0, sizeof(kos_kernel_state_t));
    
    // Initialize GDT
    kos_gdt_t gdt;
    kos_result_t result = kos_gdt_init(&gdt);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    result = kos_gdt_setup_kernel_segments(&gdt);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    result = kos_gdt_setup_user_segments(&gdt);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    kos_gdt_load(&gdt);
    kos_gdt_flush();
    
    // Disable interrupts during setup using HAL
    hal_interrupt_disable();
    
    // Initialize memory manager
    result = kos_memory_init(&g_memory_manager);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    // Initialize driver manager
    void* driver_manager = NULL; // Simplified
    result = kos_driver_manager_init(driver_manager);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    // Register keyboard driver
    if (config->enable_keyboard) {
        result = kos_driver_register(driver_manager, &g_keyboard_driver);
        if (result != KOS_SUCCESS) {
            return result;
        }
        
        result = kos_keyboard_init(&g_keyboard_driver);
        if (result != KOS_SUCCESS) {
            return result;
        }
    }
    
    // Initialize all registered drivers
    result = kos_driver_init_all(driver_manager);
    if (result != KOS_SUCCESS) {
        return result;
    }
    
    // Enable interrupts using HAL
    hal_interrupt_enable();
    
    return KOS_SUCCESS;
}

kos_result_t kos_kernel_start(void) {
    // Main kernel loop
    while (true) {
        // Save power when idle using HAL
        hal_halt();
    }
    
    // This should never return
    return KOS_ERROR_NOT_IMPLEMENTED;
}

const char* kos_kernel_get_version(void) {
    return "0.1.0";
}

void kos_kernel_panic(const char* message) {
    (void)message; // Suppress unused parameter warning
    
    // Disable interrupts using HAL
    hal_interrupt_disable();
    
    // Halt the system using HAL
    while (true) {
        hal_halt();
    }
}

void kos_kernel_assert(b8 condition, const char* message) {
    if (!condition) {
        kos_kernel_panic(message);
    }
}
