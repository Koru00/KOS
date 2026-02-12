#pragma once

#include "hal/hal_core.h"
#include "hal/hal_interface_clean.h"
#include "kos/types.h"
#include "kos/config.h"

// =============================================================================
// KOS - Driver Framework (HAL-based)
// =============================================================================

// Driver types
typedef enum {
    KOS_DRIVER_TYPE_UNKNOWN = 0,
    KOS_DRIVER_TYPE_CHAR,
    KOS_DRIVER_TYPE_BLOCK,
    KOS_DRIVER_TYPE_NETWORK,
    KOS_DRIVER_TYPE_INPUT,
    KOS_DRIVER_TYPE_OUTPUT,
    KOS_DRIVER_TYPE_STORAGE,
    KOS_DRIVER_TYPE_USB,
    KOS_DRIVER_TYPE_PCI,
    KOS_DRIVER_TYPE_INTERRUPT,
    KOS_DRIVER_TYPE_TIMER
} kos_driver_type_t;

// Driver states
typedef enum {
    KOS_DRIVER_STATE_UNINITIALIZED = 0,
    KOS_DRIVER_STATE_INITIALIZING,
    KOS_DRIVER_STATE_INITIALIZED,
    KOS_DRIVER_STATE_STARTING,
    KOS_DRIVER_STATE_RUNNING,
    KOS_DRIVER_STATE_STOPPING,
    KOS_DRIVER_STATE_STOPPED,
    KOS_DRIVER_STATE_ERROR
} kos_driver_state_t;

// Driver flags
typedef enum {
    KOS_DRIVER_FLAG_NONE = 0,
    KOS_DRIVER_FLAG_CRITICAL = 0x01,
    KOS_DRIVER_FLAG_HOTPLUG = 0x02,
    KOS_DRIVER_FLAG_REMOVABLE = 0x04,
    KOS_DRIVER_FLAG_EXCLUSIVE = 0x08,
    KOS_DRIVER_FLAG_SHARED = 0x10,
    KOS_DRIVER_FLAG_POLLING = 0x20,
    KOS_DRIVER_FLAG_INTERRUPT_DRIVEN = 0x40,
    KOS_DRIVER_FLAG_DMA_CAPABLE = 0x80
} kos_driver_flags_t;

// Forward declarations
typedef struct kos_driver kos_driver_t;
typedef struct kos_device kos_device_t;
typedef struct kos_driver_manager kos_driver_manager_t;

// Driver operations
typedef struct kos_driver_ops {
    // Core operations
    hal_result_t (*init)(kos_driver_t* driver);
    hal_result_t (*shutdown)(kos_driver_t* driver);
    hal_result_t (*start)(kos_driver_t* driver);
    hal_result_t (*stop)(kos_driver_t* driver);
    
    // Device operations
    hal_result_t (*probe)(kos_driver_t* driver, kos_device_t* device);
    hal_result_t (*remove)(kos_driver_t* driver, kos_device_t* device);
    hal_result_t (*suspend)(kos_driver_t* driver, kos_device_t* device);
    hal_result_t (*resume)(kos_driver_t* driver, kos_device_t* device);
    
    // I/O operations
    hal_result_t (*read)(kos_device_t* device, void* buffer, size_t size, size_t* bytes_read);
    hal_result_t (*write)(kos_device_t* device, const void* buffer, size_t size, size_t* bytes_written);
    hal_result_t (*ioctl)(kos_device_t* device, uint32_t request, void* arg);
    
    // Interrupt handling
    hal_result_t (*interrupt_handler)(kos_device_t* device, uint32_t irq, void* context);
    
    // Power management
    hal_result_t (*power_on)(kos_device_t* device);
    hal_result_t (*power_off)(kos_device_t* device);
    hal_result_t (*sleep)(kos_device_t* device);
    hal_result_t (*wakeup)(kos_device_t* device);
    
    // Configuration
    hal_result_t (*get_config)(kos_device_t* device, void* config, size_t size);
    hal_result_t (*set_config)(kos_device_t* device, const void* config, size_t size);
    
    // Status and diagnostics
    hal_result_t (*get_status)(kos_device_t* device, void* status, size_t size);
    hal_result_t (*reset)(kos_device_t* device);
    hal_result_t (*self_test)(kos_device_t* device);
} kos_driver_ops_t;

// Device information
typedef struct kos_device_info {
    char name[KOS_CONFIG_MAX_NAME_LENGTH];
    char description[KOS_CONFIG_MAX_DESC_LENGTH];
    char version[KOS_CONFIG_MAX_VERSION_LENGTH];
    char manufacturer[KOS_CONFIG_MAX_NAME_LENGTH];
    uint32_t device_id;
    uint32_t vendor_id;
    uint32_t class_id;
    uint32_t subclass_id;
    uint64_t base_address;
    uint64_t memory_size;
    uint32_t irq_line;
    uint32_t dma_channel;
    kos_driver_flags_t flags;
} kos_device_info_t;

// Device structure
struct kos_device {
    kos_device_info_t info;
    kos_driver_t* driver;
    void* private_data;
    kos_driver_state_t state;
    uint32_t reference_count;
    hal_bool_t is_open;
    // hal_mutex_t lock;  // Simplified - no mutex for now
    kos_device_t* next;
};

// Driver structure
struct kos_driver {
    char name[KOS_CONFIG_MAX_NAME_LENGTH];
    char description[KOS_CONFIG_MAX_DESC_LENGTH];
    char version[KOS_CONFIG_MAX_VERSION_LENGTH];
    kos_driver_type_t type;
    kos_driver_flags_t flags;
    kos_driver_state_t state;
    
    // Operations
    kos_driver_ops_t* ops;
    
    // Device management
    kos_device_t* devices;
    uint32_t device_count;
    uint32_t max_devices;
    
    // HAL integration
    hal_interface_t* hal_interface;
    
    // Statistics
    uint64_t init_count;
    uint64_t start_count;
    uint64_t stop_count;
    uint64_t error_count;
    
    // Private data
    void* private_data;
    
    // List management
    kos_driver_t* next;
};

// Driver manager structure
struct kos_driver_manager {
    kos_driver_t* drivers;
    kos_device_t* devices;
    uint32_t driver_count;
    uint32_t device_count;
    hal_bool_t initialized;
    
    // Statistics
    uint64_t total_drivers;
    uint64_t total_devices;
    uint64_t active_drivers;
    uint64_t active_devices;
    uint64_t failed_operations;
    
    // Simplified - no mutex for now
    // hal_mutex_t driver_lock;
    // hal_mutex_t device_lock;
};

// =============================================================================
// Driver Framework API
// =============================================================================

// Driver manager operations
hal_result_t kos_driver_manager_init(void);
hal_result_t kos_driver_manager_shutdown(void);
hal_result_t kos_driver_manager_register_driver(kos_driver_t* driver);
hal_result_t kos_driver_manager_unregister_driver(kos_driver_t* driver);
hal_result_t kos_driver_manager_register_device(kos_device_t* device);
hal_result_t kos_driver_manager_unregister_device(kos_device_t* device);

// Driver operations
hal_result_t kos_driver_init(kos_driver_t* driver, const char* name, kos_driver_type_t type, kos_driver_ops_t* ops);
hal_result_t kos_driver_shutdown(kos_driver_t* driver);
hal_result_t kos_driver_start(kos_driver_t* driver);
hal_result_t kos_driver_stop(kos_driver_t* driver);
hal_result_t kos_driver_add_device(kos_driver_t* driver, kos_device_t* device);
hal_result_t kos_driver_remove_device(kos_driver_t* driver, kos_device_t* device);

// Device operations
hal_result_t kos_device_init(kos_device_t* device, const kos_device_info_t* info);
hal_result_t kos_device_shutdown(kos_device_t* device);
hal_result_t kos_device_open(kos_device_t* device);
hal_result_t kos_device_close(kos_device_t* device);
hal_result_t kos_device_read(kos_device_t* device, void* buffer, size_t size, size_t* bytes_read);
hal_result_t kos_device_write(kos_device_t* device, const void* buffer, size_t size, size_t* bytes_written);
hal_result_t kos_device_ioctl(kos_device_t* device, uint32_t request, void* arg);

// Utility functions
kos_driver_t* kos_driver_find_by_name(const char* name);
kos_device_t* kos_device_find_by_name(const char* name);
kos_device_t* kos_device_find_by_id(uint32_t device_id);
hal_result_t kos_driver_manager_dump_stats(void);
hal_result_t kos_driver_manager_dump_drivers(void);
hal_result_t kos_driver_manager_dump_devices(void);

// =============================================================================
// Driver Registration Macros
// =============================================================================

#define KOS_DRIVER_REGISTER(name, type, ops) \
    static kos_driver_t kos_driver_##name = { \
        .name = #name, \
        .type = type, \
        .ops = &ops, \
        .state = KOS_DRIVER_STATE_UNINITIALIZED \
    }; \
    __attribute__((constructor)) \
    static void kos_driver_##name##_register(void) { \
        kos_driver_manager_register_driver(&kos_driver_##name); \
    }

#define KOS_DRIVER_DEFINE_OPS(prefix) \
    static kos_driver_ops_t kos_driver_ops_##prefix = { \
        .init = prefix##_init, \
        .shutdown = prefix##_shutdown, \
        .start = prefix##_start, \
        .stop = prefix##_stop, \
        .probe = prefix##_probe, \
        .remove = prefix##_remove, \
        .read = prefix##_read, \
        .write = prefix##_write, \
        .ioctl = prefix##_ioctl, \
        .interrupt_handler = prefix##_interrupt_handler, \
        .power_on = prefix##_power_on, \
        .power_off = prefix##_power_off, \
        .get_config = prefix##_get_config, \
        .set_config = prefix##_set_config, \
        .get_status = prefix##_get_status, \
        .reset = prefix##_reset, \
        .self_test = prefix##_self_test \
    }
