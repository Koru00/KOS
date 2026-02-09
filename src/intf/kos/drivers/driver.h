#pragma once

#include "../types.h"
#include "../config.h"

// =============================================================================
// KOS - Driver Framework Interface
// =============================================================================

// Driver types
typedef enum {
    KOS_DRIVER_TYPE_UNKNOWN = 0,
    KOS_DRIVER_TYPE_KEYBOARD = 1,
    KOS_DRIVER_TYPE_VGA = 2,
    KOS_DRIVER_TYPE_SERIAL = 3,
    KOS_DRIVER_TYPE_TIMER = 4,
    KOS_DRIVER_TYPE_STORAGE = 5,
    KOS_DRIVER_TYPE_NETWORK = 6
} kos_driver_type_t;

// Driver states
typedef enum {
    KOS_DRIVER_STATE_UNINITIALIZED = 0,
    KOS_DRIVER_STATE_INITIALIZING = 1,
    KOS_DRIVER_STATE_READY = 2,
    KOS_DRIVER_STATE_ERROR = 3,
    KOS_DRIVER_STATE_DISABLED = 4
} kos_driver_state_t;

// Driver interface
typedef struct kos_driver {
    const char* name;
    kos_driver_type_t type;
    kos_driver_state_t state;
    
    // Driver operations
    kos_result_t (*init)(struct kos_driver* driver);
    kos_result_t (*shutdown)(struct kos_driver* driver);
    kos_result_t (*reset)(struct kos_driver* driver);
    kos_result_t (*suspend)(struct kos_driver* driver);
    kos_result_t (*resume)(struct kos_driver* driver);
    
    // Driver-specific data
    void* private_data;
} kos_driver_t;

// Driver manager
typedef struct kos_driver_manager {
    kos_driver_t* drivers[KOS_DRIVER_TYPE_NETWORK + 1];  // One slot per type
    usize driver_count;
} kos_driver_manager_t;

// Driver management functions
kos_result_t kos_driver_manager_init(kos_driver_manager_t* manager);
kos_result_t kos_driver_register(kos_driver_manager_t* manager, kos_driver_t* driver);
kos_result_t kos_driver_unregister(kos_driver_manager_t* manager, const char* name);
kos_driver_t* kos_driver_find(kos_driver_manager_t* manager, kos_driver_type_t type);
kos_driver_t* kos_driver_find_by_name(kos_driver_manager_t* manager, const char* name);

// Driver lifecycle functions
kos_result_t kos_driver_init_all(kos_driver_manager_t* manager);
kos_result_t kos_driver_shutdown_all(kos_driver_manager_t* manager);

// Utility macros
#define KOS_DRIVER_DECLARE(name, type, init_func, shutdown_func) \
    static kos_driver_t name##_driver = { \
        .name = #name, \
        .type = type, \
        .state = KOS_DRIVER_STATE_UNINITIALIZED, \
        .init = init_func, \
        .shutdown = shutdown_func \
    }
