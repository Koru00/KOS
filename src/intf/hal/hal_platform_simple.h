#pragma once

#include "hal_types.h"
#include "hal_interface.h"

// =============================================================================
// KOS - HAL Platform Operations Interface (Simple)
// =============================================================================

// Platform operations interface structure
struct hal_platform_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Platform information
    hal_result_t (*get_info)(hal_platform_info_t* info);
    
    // Power management
    hal_result_t (*power_off)(void);
    hal_result_t (*reboot)(void);
    hal_result_t (*shutdown_system)(void);
    
    // Virtualization support
    hal_result_t (*is_virtualized)(hal_bool_t* virtualized);
    hal_result_t (*get_virtualization_info)(hal_virtualization_info_t* info);
    
    // Platform statistics
    hal_result_t (*get_stats)(hal_platform_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
};

// Platform operation convenience functions
hal_result_t hal_platform_init(void);
hal_result_t hal_platform_shutdown(void);
hal_result_t hal_platform_reset(void);
hal_result_t hal_platform_get_info(hal_platform_info_t* info);
hal_result_t hal_platform_get_stats(hal_platform_stats_t* stats);
hal_result_t hal_platform_reset_stats(void);
hal_result_t hal_platform_dump_stats(void);
