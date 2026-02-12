#pragma once

#include "../types.h"
#include "../config.h"

// =============================================================================
// KOS - Legacy Driver Interface (Deprecated - Use driver_framework.h)
// =============================================================================

// This file is deprecated. Use driver_framework.h for new development.
// Keeping for backward compatibility only.

// Driver types (legacy)
typedef enum {
    KOS_DRIVER_TYPE_UNKNOWN = 0,
    KOS_DRIVER_TYPE_KEYBOARD = 1,
    KOS_DRIVER_TYPE_VGA = 2,
    KOS_DRIVER_TYPE_SERIAL = 3,
    KOS_DRIVER_TYPE_TIMER = 4,
    KOS_DRIVER_TYPE_STORAGE = 5,
    KOS_DRIVER_TYPE_NETWORK = 6
} kos_driver_type_legacy_t;

// Driver states (legacy)
typedef enum {
    KOS_DRIVER_STATE_UNINITIALIZED = 0,
    KOS_DRIVER_STATE_INITIALIZING = 1,
    KOS_DRIVER_STATE_READY = 2,
    KOS_DRIVER_STATE_ERROR = 3,
    KOS_DRIVER_STATE_DISABLED = 4
} kos_driver_state_legacy_t;

// Legacy driver interface (deprecated)
typedef struct kos_driver_legacy {
    const char* name;
    kos_driver_type_legacy_t type;
    kos_driver_state_legacy_t state;
    
    // Driver operations
    kos_result_t (*init)(struct kos_driver_legacy* driver);
    kos_result_t (*shutdown)(struct kos_driver_legacy* driver);
    kos_result_t (*reset)(struct kos_driver_legacy* driver);
    kos_result_t (*suspend)(struct kos_driver_legacy* driver);
    kos_result_t (*resume)(struct kos_driver_legacy* driver);
    
    // Driver-specific data
    void* private_data;
} kos_driver_legacy_t;

// Legacy driver manager (deprecated)
typedef struct kos_driver_manager_legacy {
    kos_driver_legacy_t* drivers[KOS_DRIVER_TYPE_NETWORK + 1];  // One slot per type
    usize driver_count;
} kos_driver_manager_legacy_t;

// Legacy functions (deprecated)
kos_result_t kos_driver_manager_init_legacy(kos_driver_manager_legacy_t* manager);
kos_driver_t* kos_driver_find_by_name_legacy(kos_driver_manager_legacy_t* manager, const char* name);

#endif // KOS_DRIVERS_DRIVER_H
