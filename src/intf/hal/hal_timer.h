#pragma once

#include "hal_types.h"
#include "hal_interface.h"

// =============================================================================
// KOS - HAL Timer Operations Interface
// =============================================================================

// Timer information structure
typedef struct {
    hal_u32_t frequency;
    hal_u32_t resolution;
    hal_u32_t min_frequency;
    hal_u32_t max_frequency;
    hal_bool_t has_pit;
    hal_bool_t has_apic_timer;
    hal_bool_t has_tsc;
    hal_bool_t has_hpet;
    hal_u64_t tsc_frequency;
} hal_timer_info_t;

// Timer modes
typedef enum {
    HAL_TIMER_MODE_ONE_SHOT = 0,
    HAL_TIMER_MODE_PERIODIC = 1,
    HAL_TIMER_MODE_RATE_GENERATOR = 2,
    HAL_TIMER_MODE_SQUARE_WAVE = 3
} hal_timer_mode_t;

// Timer types
typedef enum {
    HAL_TIMER_TYPE_PIT = 0,
    HAL_TIMER_TYPE_APIC = 1,
    HAL_TIMER_TYPE_TSC = 2,
    HAL_TIMER_TYPE_HPET = 3,
    HAL_TIMER_TYPE_LOCAL = 4
} hal_timer_type_t;

// Timer handler function type
typedef void (*hal_timer_handler_t)(hal_u32_t timer_id, void* context);

// Timer operations interface structure
struct hal_timer_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Timer information
    hal_result_t (*get_info)(hal_timer_info_t* info);
    hal_u32_t (*get_frequency)(void);
    hal_u32_t (*get_resolution)(void);
    hal_u32_t (*get_min_frequency)(void);
    hal_u32_t (*get_max_frequency)(void);
    
    // Timer control
    hal_result_t (*start)(hal_u32_t timer_id);
    hal_result_t (*stop)(hal_u32_t timer_id);
    hal_result_t (*reset)(hal_u32_t timer_id);
    hal_result_t (*set_mode)(hal_u32_t timer_id, hal_timer_mode_t mode);
    hal_result_t (*set_frequency)(hal_u32_t timer_id, hal_u32_t frequency);
    hal_result_t (*set_period)(hal_u32_t timer_id, hal_u32_t period);
    
    // Timer configuration
    hal_result_t (*configure)(hal_u32_t timer_id, hal_u32_t frequency, hal_timer_mode_t mode);
    hal_result_t (*get_config)(hal_u32_t timer_id, hal_u32_t* frequency, hal_timer_mode_t* mode);
    
    // Timer registration
    hal_result_t (*register_handler)(hal_u32_t timer_id, hal_timer_handler_t handler, void* context);
    hal_result_t (*unregister_handler)(hal_u32_t timer_id);
    hal_timer_handler_t (*get_handler)(hal_u32_t timer_id);
    
    // Time measurement
    hal_result_t (*get_timestamp)(hal_timestamp_t* timestamp);
    hal_result_t (*get_cycle_count)(hal_u64_t* count);
    hal_result_t (*get_tsc_frequency)(hal_frequency_t* frequency);
    hal_result_t (*synchronize_tsc)(void);
    
    // Delay functions
    hal_result_t (*delay)(hal_u32_t milliseconds);
    hal_result_t (*delay_microseconds)(hal_u32_t microseconds);
    hal_result_t (*delay_nanoseconds)(hal_u32_t nanoseconds);
    hal_result_t (*busy_wait)(hal_u32_t microseconds);
    
    // PIT operations
    hal_result_t (*pit_init)(void);
    hal_result_t (*pit_set_frequency)(hal_u32_t frequency);
    hal_result_t (*pit_set_period)(hal_u16_t period);
    hal_result_t (*pit_start)(void);
    hal_result_t (*pit_stop)(void);
    hal_result_t (*pit_read_count)(hal_u16_t* count);
    
    // APIC timer operations
    hal_result_t (*apic_timer_init)(void);
    hal_result_t (*apic_timer_set_divisor)(hal_u32_t divisor);
    hal_result_t (*apic_timer_set_initial_count)(hal_u32_t count);
    hal_result_t (*apic_timer_enable)(void);
    hal_result_t (*apic_timer_disable)(void);
    hal_result_t (*apic_timer_get_lvt)(hal_u32_t* lvt);
    hal_result_t (*apic_timer_set_lvt)(hal_u32_t lvt);
    
    // TSC operations
    hal_result_t (*tsc_init)(void);
    hal_result_t (*tsc_enable)(void);
    hal_result_t (*tsc_disable)(void);
    hal_result_t (*tsc_synchronize)(void);
    hal_result_t (*tsc_get_frequency)(hal_u64_t* frequency);
    hal_result_t (*tsc_calibrate)(void);
    
    // HPET operations
    hal_result_t (*hpet_init)(void);
    hal_result_t (*hpet_enable)(void);
    hal_result_t (*hpet_disable)(void);
    hal_result_t (*hpet_set_frequency)(hal_u32_t frequency);
    hal_result_t (*hpet_set_period)(hal_u64_t period);
    hal_result_t (*hpet_get_counter)(hal_u64_t* counter);
    hal_result_t (*hpet_get_maximum_counter)(hal_u64_t* max_counter);
    
    // Local APIC timer operations
    hal_result_t (*local_timer_init)(void);
    hal_result_t (*local_timer_set_divisor)(hal_u32_t divisor);
    hal_result_t (*local_timer_set_initial_count)(hal_u32_t count);
    hal_result_t (*local_timer_enable)(void);
    hal_result_t (*local_timer_disable)(void);
    
    // Timer statistics
    hal_result_t (*get_stats)(hal_timer_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
    
    // Timer debugging
    hal_result_t (*dump_timer_info)(void);
    hal_result_t (*dump_timer_config)(hal_u32_t timer_id);
    hal_result_t (*dump_all_timers)(void);
    
    // Power management
    hal_result_t (*enter_sleep)(hal_u32_t milliseconds);
    hal_result_t (*exit_sleep)(void);
    hal_result_t (*get_sleep_state)(hal_u32_t* state);
    hal_result_t (*set_sleep_state)(hal_u32_t state);
};

// Timer operation convenience functions
hal_result_t hal_timer_init(void);
hal_result_t hal_timer_shutdown(void);
hal_result_t hal_timer_reset(void);

hal_result_t hal_timer_start(hal_u32_t timer_id);
hal_result_t hal_timer_stop(hal_u32_t timer_id);
hal_result_t hal_timer_set_frequency(hal_u32_t timer_id, hal_u32_t frequency);
hal_result_t hal_timer_set_period(hal_u32_t timer_id, hal_u32_t period);

hal_result_t hal_timer_get_timestamp(hal_timestamp_t* timestamp);
hal_result_t hal_timer_get_cycle_count(hal_u64_t* count);
hal_result_t hal_timer_get_tsc_frequency(hal_frequency_t* frequency);

hal_result_t hal_timer_delay(hal_u32_t milliseconds);
hal_result_t hal_timer_delay_microseconds(hal_u32_t microseconds);
hal_result_t hal_timer_delay_nanoseconds(hal_u32_t nanoseconds);

hal_result_t hal_timer_get_info(hal_timer_info_t* info);
hal_u32_t hal_timer_get_frequency(void);
hal_u32_t hal_timer_get_resolution(void);
hal_u32_t hal_timer_get_min_frequency(void);
hal_u32_t hal_timer_get_max_frequency(void);

// Timer control macros
#define HAL_TIMER_START(id) hal_timer_start(id)
#define HAL_TIMER_STOP(id) hal_timer_stop(id)
#define HAL_TIMER_SET_FREQUENCY(id, freq) hal_timer_set_frequency(id, freq)
#define HAL_TIMER_SET_PERIOD(id, period) hal_timer_set_period(id, period)

// Timer measurement macros
#define HAL_TIMER_GET_TIMESTAMP(ts) hal_timer_get_timestamp(ts)
#define HAL_TIMER_GET_CYCLE_COUNT(count) hal_timer_get_cycle_count(count)
#define HAL_TIMER_GET_TSC_FREQUENCY(freq) hal_timer_get_tsc_frequency(freq)

// Timer delay macros
#define HAL_TIMER_DELAY(ms) hal_timer_delay(ms)
#define HAL_TIMER_DELAY_US(us) hal_timer_delay_microseconds(us)
#define HAL_TIMER_DELAY_NS(ns) hal_timer_delay_nanoseconds(ns)

// Timer info macros
#define HAL_TIMER_GET_INFO(info) hal_timer_get_info(info)
#define HAL_TIMER_GET_FREQUENCY() hal_timer_get_frequency()
#define HAL_TIMER_GET_RESOLUTION() hal_timer_get_resolution()
#define HAL_TIMER_GET_MIN_FREQUENCY() hal_timer_get_min_frequency()
#define HAL_TIMER_GET_MAX_FREQUENCY() hal_timer_get_max_frequency()

// Timer validation macros
#define HAL_TIMER_VALIDATE_PARAM(param) HAL_VALIDATE_PARAM(param)
#define HAL_TIMER_VALIDATE_STATE(condition) HAL_VALIDATE_STATE(condition)

// Time conversion macros
#define HAL_MS_TO_NS(ms) ((ms) * 1000000ULL)
#define HAL_US_TO_NS(us) ((us) * 1000ULL)
#define HAL_NS_TO_MS(ns) ((ns) / 1000000ULL)
#define HAL_NS_TO_US(ns) ((ns) / 1000ULL)

// Frequency conversion macros
#define HAL_HZ_TO_MHZ(hz) ((hz) / 1000000)
#define HAL_MHZ_TO_HZ(mhz) ((mhz) * 1000000)
#define HAL_KHZ_TO_HZ(khz) ((khz) * 1000)
#define HAL_HZ_TO_KHZ(hz) ((hz) / 1000)
