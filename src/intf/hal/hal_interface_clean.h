#pragma once

#include "hal_core.h"

// =============================================================================
// KOS - HAL Interface (Clean Version)
// =============================================================================

// Forward declarations for operation structures
struct hal_cpu_ops;
struct hal_memory_ops;
struct hal_interrupt_ops;
struct hal_timer_ops;
struct hal_platform_ops;

// CPU operations interface
struct hal_cpu_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // CPU information
    hal_result_t (*get_info)(hal_cpu_info_t* info);
    hal_result_t (*get_features)(hal_u64_t* features);
    
    // CPU control
    hal_result_t (*enable)(void);
    hal_result_t (*disable)(void);
    hal_result_t (*halt)(void);
    hal_result_t (*pause)(void);
    hal_result_t (*resume)(void);
    
    // Context management
    hal_result_t (*save_context)(hal_cpu_context_t* context);
    hal_result_t (*restore_context)(const hal_cpu_context_t* context);
    hal_result_t (*init_context)(hal_cpu_context_t* context);
    
    // Performance monitoring
    hal_result_t (*get_cycle_count)(hal_u64_t* count);
    hal_result_t (*get_timestamp)(hal_timestamp_t* timestamp);
    hal_result_t (*get_tsc_frequency)(hal_frequency_t* frequency);
    hal_result_t (*get_utilization)(hal_u32_t* utilization);
    hal_result_t (*get_idle_time)(hal_u64_t* idle_time);
    hal_result_t (*get_busy_time)(hal_u64_t* busy_time);
    hal_result_t (*reset_utilization_stats)(void);
    
    // Statistics
    hal_result_t (*get_stats)(hal_cpu_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
};

// Memory operations interface
struct hal_memory_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Memory information
    hal_result_t (*get_info)(hal_memory_info_t* info);
    
    // Memory allocation
    void* (*alloc)(hal_size_t size);
    void* (*realloc)(void* ptr, hal_size_t new_size);
    void* (*calloc)(hal_size_t count, hal_size_t size);
    void (*free)(void* ptr);
    
    // Memory mapping
    hal_result_t (*map_physical)(hal_phys_addr_t phys_addr, hal_size_t size, hal_virt_addr_t* virt_addr);
    hal_result_t (*unmap)(hal_virt_addr_t virt_addr, hal_size_t size);
    
    // Memory protection
    hal_result_t (*protect)(hal_virt_addr_t addr, hal_size_t size, hal_memory_prot_t protection);
    hal_result_t (*unprotect)(hal_virt_addr_t addr, hal_size_t size);
    
    // Memory statistics
    hal_result_t (*get_stats)(hal_memory_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
};

// Interrupt operations interface
struct hal_interrupt_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Interrupt information
    hal_result_t (*get_info)(hal_interrupt_info_t* info);
    
    // Interrupt control
    hal_result_t (*enable)(void);
    hal_result_t (*disable)(void);
    hal_result_t (*enable_irq)(hal_u32_t irq);
    hal_result_t (*disable_irq)(hal_u32_t irq);
    hal_result_t (*mask_irq)(hal_u32_t irq);
    hal_result_t (*unmask_irq)(hal_u32_t irq);
    
    // Interrupt handlers
    hal_result_t (*register_handler)(hal_u32_t vector, hal_interrupt_handler_t handler);
    hal_result_t (*unregister_handler)(hal_u32_t vector);
    hal_result_t (*enable_handler)(hal_u32_t vector);
    hal_result_t (*disable_handler)(hal_u32_t vector);
    hal_result_t (*set_priority)(hal_u32_t vector, hal_u8_t priority);
    hal_result_t (*get_priority)(hal_u32_t vector, hal_u8_t* priority);
    
    // Statistics
    hal_result_t (*get_stats)(hal_interrupt_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
};

// Timer operations interface
struct hal_timer_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Timer information
    hal_result_t (*get_info)(hal_timer_info_t* info);
    
    // Timer operations
    hal_result_t (*get_timestamp)(hal_timestamp_t* timestamp);
    hal_result_t (*get_cycle_count)(hal_u64_t* count);
    hal_result_t (*get_tsc_frequency)(hal_frequency_t* frequency);
    hal_result_t (*delay)(hal_u32_t milliseconds);
    hal_result_t (*delay_microseconds)(hal_u32_t microseconds);
    hal_result_t (*delay_nanoseconds)(hal_u64_t nanoseconds);
    
    // Timer handlers
    hal_result_t (*register_timer)(hal_u32_t timer_id, hal_timer_handler_t handler);
    hal_result_t (*unregister_timer)(hal_u32_t timer_id);
    hal_result_t (*start_timer)(hal_u32_t timer_id, hal_u32_t interval_ms);
    hal_result_t (*stop_timer)(hal_u32_t timer_id);
    
    // Statistics
    hal_result_t (*get_stats)(hal_timer_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
};

// Platform operations interface
struct hal_platform_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Platform information
    hal_result_t (*get_info)(hal_platform_info_t* info);
    hal_result_t (*get_virtualization_info)(hal_virtualization_info_t* info);
    hal_result_t (*get_hypervisor_info)(hal_hypervisor_info_t* info);
    
    // Platform control
    hal_result_t (*reboot)(hal_reset_type_t type);
    hal_result_t (*shutdown_system)(void);
    hal_result_t (*sleep)(hal_u32_t milliseconds);
    hal_result_t (*hibernate)(void);
    
    // Power management
    hal_result_t (*get_power_state)(hal_power_state_t* state);
    hal_result_t (*set_power_state)(hal_power_state_t state);
    
    // Device management
    hal_result_t (*enumerate_devices)(void);
    hal_result_t (*get_device_info)(hal_u32_t device_id, void* info, hal_size_t info_size);
    
    // Statistics
    hal_result_t (*get_stats)(hal_platform_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
};

// Main HAL interface structure
typedef struct hal_interface {
    // Architecture information
    hal_architecture_t architecture;
    char arch_name[32];
    hal_u32_t arch_bits;
    
    // Operation interfaces
    struct hal_cpu_ops* cpu_ops;
    struct hal_memory_ops* memory_ops;
    struct hal_interrupt_ops* interrupt_ops;
    struct hal_timer_ops* timer_ops;
    struct hal_platform_ops* platform_ops;
    
    // Global information
    hal_cpu_info_t cpu_info;
    hal_memory_info_t memory_info;
    hal_interrupt_info_t interrupt_info;
    hal_timer_info_t timer_info;
    hal_platform_info_t platform_info;
    
    // Global statistics
    hal_cpu_stats_t cpu_stats;
    hal_memory_stats_t memory_stats;
    hal_interrupt_stats_t interrupt_stats;
    hal_timer_stats_t timer_stats;
    hal_platform_stats_t platform_stats;
} hal_interface_t;

// HAL initialization and management
hal_result_t hal_init(void);
hal_result_t hal_shutdown(void);
hal_result_t hal_reset(void);
hal_result_t hal_cleanup(void);

// Interface access
hal_interface_t* hal_get_interface(void);
hal_result_t hal_set_interface(const hal_interface_t* interface);

// Architecture detection
hal_architecture_t hal_get_current_architecture(void);
const char* hal_get_architecture_name(hal_architecture_t arch);
hal_u32_t hal_get_architecture_bits(hal_architecture_t arch);

// Error handling
const char* hal_get_error_string(hal_result_t error);
hal_result_t hal_set_error_handler(hal_result_t (*handler)(hal_result_t error, const char* message));

// Configuration management
typedef struct hal_config {
    hal_bool_t enable_statistics;
    hal_bool_t enable_debug;
    hal_bool_t enable_tracing;
    hal_u32_t max_interrupt_nesting;
    hal_u32_t max_memory_pools;
    hal_size_t min_block_size;
    hal_size_t max_block_size;
} hal_config_t;

hal_result_t hal_set_config(const hal_config_t* config);
hal_result_t hal_get_config(hal_config_t* config);
hal_result_t hal_validate_config(const hal_config_t* config);

// Utility functions
hal_result_t hal_dump_platform_info(void);
hal_result_t hal_dump_cpu_info(void);
hal_result_t hal_dump_memory_info(void);
hal_result_t hal_dump_interrupt_info(void);
hal_result_t hal_dump_timer_info(void);

// Version information
#define HAL_VERSION_MAJOR 1
#define HAL_VERSION_MINOR 0
#define HAL_VERSION_PATCH 0
#define HAL_VERSION_STRING "1.0.0"

// Compatibility checks
#define HAL_VERSION_CHECK(major, minor, patch) \
    ((HAL_VERSION_MAJOR > (major)) || \
     (HAL_VERSION_MAJOR == (major) && HAL_VERSION_MINOR > (minor)) || \
     (HAL_VERSION_MAJOR == (major) && HAL_VERSION_MINOR == (minor) && HAL_VERSION_PATCH >= (patch)))

// Feature availability
#define HAL_HAS_CPU_OPERATIONS 1
#define HAL_HAS_MEMORY_OPERATIONS 1
#define HAL_HAS_INTERRUPT_OPERATIONS 1
#define HAL_HAS_TIMER_OPERATIONS 1
#define HAL_HAS_PLATFORM_OPERATIONS 1
#define HAL_HAS_STATISTICS 1
#define HAL_HAS_DEBUGGING 1
#define HAL_HAS_TRACING 1
