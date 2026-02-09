#pragma once

#include "hal_types.h"
#include "hal_interface.h"

// =============================================================================
// KOS - HAL Timer Operations Interface (Simple)
// =============================================================================

// Timer operations interface structure
struct hal_timer_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Timer information
    hal_result_t (*get_info)(hal_timer_info_t* info);
    
    // Time measurement
    hal_result_t (*get_timestamp)(hal_timestamp_t* timestamp);
    hal_result_t (*get_cycle_count)(hal_u64_t* count);
    hal_result_t (*get_tsc_frequency)(hal_frequency_t* frequency);
    
    // Delay functions
    hal_result_t (*delay)(hal_u32_t milliseconds);
    
    // Timer statistics
    hal_result_t (*get_stats)(hal_timer_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
};

// Timer operation convenience functions
hal_result_t hal_timer_init(void);
hal_result_t hal_timer_shutdown(void);
hal_result_t hal_timer_reset(void);

hal_result_t hal_timer_get_timestamp(hal_timestamp_t* timestamp);
hal_result_t hal_timer_get_cycle_count(hal_u64_t* count);
hal_result_t hal_timer_get_tsc_frequency(hal_frequency_t* frequency);

hal_result_t hal_timer_delay(hal_u32_t milliseconds);

hal_result_t hal_timer_get_info(hal_timer_info_t* info);
hal_result_t hal_timer_get_stats(hal_timer_stats_t* stats);
hal_result_t hal_timer_reset_stats(void);
hal_result_t hal_timer_dump_stats(void);
