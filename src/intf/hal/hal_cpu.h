#pragma once

#include "hal_types.h"
#include "hal_interface.h"

// =============================================================================
// KOS - HAL CPU Operations Interface
// =============================================================================

// CPU operations interface structure
struct hal_cpu_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // CPU information
    hal_result_t (*get_info)(hal_cpu_info_t* info);
    hal_result_t (*get_features)(hal_cpu_features_t* features);
    hal_result_t (*get_vendor)(char* vendor, size_t size);
    hal_result_t (*get_model)(char* model, size_t size);
    hal_result_t (*get_family)(char* family, size_t size);
    
    // CPU control
    hal_result_t (*halt)(void);
    hal_result_t (*reset_system)(void);
    hal_result_t (*shutdown_system)(void);
    hal_result_t (*reboot)(void);
    hal_result_t (*power_off)(void);
    hal_result_t (*sleep)(uint32_t milliseconds);
    
    // Frequency control
    hal_result_t (*get_frequency)(hal_frequency_t* frequency);
    hal_result_t (*set_frequency)(hal_frequency_t frequency);
    hal_result_t (*get_max_frequency)(hal_frequency_t* frequency);
    hal_result_t (*get_min_frequency)(hal_frequency_t* frequency);
    hal_result_t (*set_boost)(hal_frequency_t frequency);
    
    // Context management
    hal_result_t (*save_context)(hal_cpu_context_t* context);
    hal_result_t (*restore_context)(const hal_cpu_context_t* context);
    hal_result_t (*init_context)(hal_cpu_context_t* context);
    hal_result_t (*cleanup_context)(hal_cpu_context_t* context);
    hal_result_t (*copy_context)(hal_cpu_context_t* dest, const hal_cpu_context_t* src);
    hal_result_t (*swap_context)(hal_cpu_context_t* ctx1, hal_cpu_context_t* ctx2);
    
    // Register access
    hal_result_t (*read_register)(hal_u32_t reg, hal_u64_t* value);
    hal_result_t (*write_register)(hal_u32_t reg, hal_u64_t value);
    hal_result_t (*read_msr)(hal_u32_t msr, hal_u64_t* value);
    hal_result_t (*write_msr)(hal_u32_t msr, hal_u64_t value);
    hal_result_t (*read_cr)(hal_u32_t cr, hal_u64_t* value);
    hal_result_t (*write_cr)(hal_u32_t cr, hal_u64_t* value);
    
    // Control registers
    hal_result_t (*enable_interrupts)(void);
    hal_result_t (*disable_interrupts)(void);
    hal_result_t (*save_interrupt_state)(hal_u32_t* flags);
    hal_result_t (*restore_interrupt_state)(hal_u32_t flags);
    hal_u32_t (*get_interrupt_state)(void);
    
    // Cache control
    hal_result_t (*invalidate_cache)(void);
    hal_result_t (*flush_cache)(void);
    hal_result_t (*enable_cache)(void);
    hal_result_t (*disable_cache)(void);
    hal_result_t (*write_back_cache)(void);
    hal_result_t (*flush_tlb)(void);
    
    // TLB control
    hal_result_t (*invalidate_tlb)(void);
    hal_result_t (*invalidate_tlb_page)(hal_virt_addr_t addr);
    hal_result_t (*flush_tlb_page)(hal_virt_addr_t addr);
    
    // CPUID and feature detection
    hal_result_t (*cpuid)(hal_u32_t leaf, hal_u32_t subleaf, hal_u32_t* eax, hal_u32_t* ebx, hal_u32_t* ecx, hal_u32_t* edx);
    hal_result_t (*get_cpuid_info)(hal_cpu_info_t* info);
    hal_result_t (*detect_features)(void);
    
    // Performance monitoring
    hal_result_t (*get_cycle_count)(hal_u64_t* count);
    hal_result_t (*get_timestamp)(hal_timestamp_t* timestamp);
    hal_result_t (*get_tsc_frequency)(hal_frequency_t* frequency);
    hal_result_t (*synchronize_tsc)(void);
    
    // Multi-core support
    hal_result_t (*get_cpu_count)(hal_u32_t* count);
    hal_result_t (*get_cpu_id)(hal_u32_t* id);
    hal_result_t (*set_cpu_affinity)(hal_u32_t cpu_mask);
    hal_result_t (*get_cpu_affinity)(hal_u32_t* cpu_mask);
    
    // Power management
    hal_result_t (*enter_sleep_state)(hal_u32_t state);
    hal_result_t (*exit_sleep_state)(hal_u32_t state);
    hal_result_t (*get_sleep_state)(hal_u32_t* state);
    hal_result_t (*set_sleep_state)(hal_u32_t state);
    
    // Thermal management
    hal_result_t (*get_temperature)(hal_i32_t* temperature);
    hal_result_t (*get_throttling_state)(hal_u32_t* state);
    hal_result_t (*set_throttling_state)(hal_u32_t state);
    
    // Debugging support
    hal_result_t (*enable_debug_breakpoint)(hal_u64_t address);
    hal_result_t (*disable_debug_breakpoint)(hal_u64_t address);
    hal_result_t (*set_debug_breakpoint)(hal_u64_t address);
    hal_result_t (*clear_debug_breakpoint)(hal_u64_t address);
    hal_result_t (*trigger_debug_breakpoint)(void);
    
    // Virtualization support
    hal_result_t (*is_virtualized)(hal_bool_t* virtualized);
    hal_result_t (*get_virtualization_info)(hal_virtualization_info_t* info);
    hal_result_t (*exit_virtualization)(void);
    
    // Security features
    hal_result_t (*enable_sme)(void);
    hal_result_t (*disable_sme)(void);
    hal_result_t (*is_sme_enabled)(hal_bool_t* enabled);
    hal_result_t (*get_sme_state)(hal_u32_t* state);
    hal_result_t (*set_sme_state)(hal_u32_t state);
    
    // Random number generation
    hal_result_t (*get_random_bytes)(hal_u8_t* buffer, hal_size_t size);
    hal_result_t (*seed_random)(hal_u64_t seed);
    
    // CPU identification
    hal_result_t (*get_cpu_signature)(hal_u8_t* signature, hal_size_t size);
    hal_result_t (*get_cpu_serial)(hal_u8_t* serial, hal_size_t size);
    hal_result_t (*get_cpu_uuid)(hal_u8_t* uuid, hal_size_t size);
    
    // CPU utilization
    hal_result_t (*get_utilization)(hal_u32_t* utilization);
    hal_result_t (*get_idle_time)(hal_u64_t* idle_time);
    hal_result_t (*get_busy_time)(hal_u64_t* busy_time);
    hal_result_t (*reset_utilization_stats)(void);
};

// CPU operation convenience functions
hal_result_t hal_cpu_init(void);
hal_result_t hal_cpu_shutdown(void);
hal_result_t hal_cpu_reset(void);
hal_result_t hal_cpu_get_info(hal_cpu_info_t* info);
hal_result_t hal_cpu_get_stats(hal_cpu_stats_t* stats);
hal_result_t hal_cpu_reset_stats(void);
hal_result_t hal_cpu_dump_stats(void);
hal_result_t hal_cpu_get_features(hal_cpu_features_t* features);
hal_result_t hal_cpu_get_frequency(hal_frequency_t* frequency);
hal_result_t hal_cpu_set_frequency(hal_frequency_t frequency);

hal_result_t hal_cpu_save_context(hal_cpu_context_t* context);
hal_result_t hal_cpu_restore_context(const hal_cpu_context_t* context);
hal_result_t hal_cpu_init_context(hal_cpu_context_t* context);

hal_result_t hal_cpu_enable_interrupts(void);
hal_result_t hal_cpu_disable_interrupts(void);
hal_u32_t hal_cpu_get_interrupt_state(void);

hal_result_t hal_cpu_get_cycle_count(hal_u64_t* count);
hal_result_t hal_cpu_get_timestamp(hal_timestamp_t* timestamp);
hal_result_t hal_cpu_get_tsc_frequency(hal_frequency_t* frequency);

hal_result_t hal_cpu_get_cpu_count(hal_u32_t* count);
hal_result_t hal_cpu_get_cpu_id(hal_u32_t* id);
hal_result_t hal_cpu_set_cpu_affinity(hal_u32_t cpu_mask);

// CPU feature detection macros
#define HAL_CPU_HAS_FEATURE(feature) (hal_cpu_has_feature(feature))
#define HAL_CPU_HAS_FPU() (hal_cpu_has_feature(HAL_CPU_FEATURE_FPU))
#define HAL_CPU_HAS_MMX() (hal_cpu_has_feature(HAL_CPU_FEATURE_MMX))
#define HAL_CPU_HAS_SSE() (hal_cpu_has_feature(HAL_CPU_FEATURE_SSE))
#define HAL_CPU_HAS_SSE2() (hal_cpu_has_feature(HAL_CPU_FEATURE_SSE2))
#define HAL_CPU_HAS_SSE3() (hal_cpu_has_feature(HAL_CPU_FEATURE_SSE3))
#define HAL_CPU_HAS_SSE4_1() (hal_cpu_has_feature(HAL_CPU_FEATURE_SSE4_1))
#define HAL_CPU_HAS_SSE4_2() (hal_cpu_has_feature(HAL_CPU_FEATURE_SSE4_2))
#define HAL_CPU_HAS_AVX() (hal_cpu_has_feature(HAL_CPU_FEATURE_AVX))
#define HAL_CPU_HAS_AVX2() (hal_cpu_has_feature(HAL_CPU_FEATURE_AVX2))

// CPU info access macros
#define HAL_CPU_GET_INFO(info) hal_cpu_get_info(info)
#define HAL_CPU_GET_FEATURES(features) hal_cpu_get_features(features)
#define HAL_CPU_GET_FREQUENCY(freq) hal_cpu_get_frequency(freq)

// CPU context macros
#define HAL_CPU_SAVE_CONTEXT(ctx) hal_cpu_save_context(ctx)
#define HAL_CPU_RESTORE_CONTEXT(ctx) hal_cpu_restore_context(ctx)
#define HAL_CPU_INIT_CONTEXT(ctx) hal_cpu_init_context(ctx)

// CPU control macros
#define HAL_CPU_HALT() hal_cpu_halt()
#define HAL_CPU_SLEEP(ms) hal_cpu_sleep(ms)
#define HAL_CPU_ENABLE_INTERRUPTS() hal_cpu_enable_interrupts()
#define HAL_CPU_DISABLE_INTERRUPTS() hal_cpu_disable_interrupts()

// CPU performance macros
#define HAL_CPU_GET_CYCLE_COUNT(count) hal_cpu_get_cycle_count(count)
#define HAL_CPU_GET_TIMESTAMP(ts) hal_cpu_get_timestamp(ts)
#define HAL_CPU_GET_TSC_FREQUENCY(freq) hal_cpu_get_tsc_frequency(freq)

// CPU multi-core macros
#define HAL_CPU_GET_COUNT(count) hal_cpu_get_cpu_count(count)
#define HAL_CPU_GET_ID(id) hal_cpu_get_cpu_id(id)
#define HAL_CPU_SET_AFFINITY(mask) hal_cpu_set_cpu_affinity(mask)

// CPU validation macros
#define HAL_CPU_VALIDATE_PARAM(param) HAL_VALIDATE_PARAM(param)
#define HAL_CPU_VALIDATE_STATE(condition) HAL_VALIDATE_STATE(condition)
