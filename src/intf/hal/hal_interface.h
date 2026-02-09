#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// =============================================================================
// KOS - HAL Main Interface
// =============================================================================

// Basic HAL types
typedef uint8_t hal_u8_t;
typedef uint16_t hal_u16_t;
typedef uint32_t hal_u32_t;
typedef uint64_t hal_u64_t;
typedef uintptr_t hal_uptr_t;
typedef size_t hal_size_t;
typedef void* hal_virt_addr_t;
typedef void* hal_phys_addr_t;
typedef uint64_t hal_timestamp_t;
typedef uint32_t hal_frequency_t;

// HAL result codes
typedef enum {
    HAL_SUCCESS = 0,
    HAL_ERROR_INVALID_PARAM = -1,
    HAL_ERROR_NOT_SUPPORTED = -2,
    HAL_ERROR_OUT_OF_MEMORY = -3,
    HAL_ERROR_HARDWARE = -4,
    HAL_ERROR_TIMEOUT = -5,
    HAL_ERROR_INVALID_STATE = -6
} hal_result_t;

// Architecture types
typedef enum {
    HAL_ARCH_X86_64 = 0,
    HAL_ARCH_I386 = 1,
    HAL_ARCH_ARM64 = 2,
    HAL_ARCH_RISCV64 = 3,
    HAL_ARCH_UNKNOWN = 0xFF
} hal_architecture_t;

// CPU information structure
typedef struct {
    char vendor[16];
    char model[64];
    char family[16];
    uint32_t frequency;
    uint32_t cores;
    uint32_t threads;
    uint64_t features;
    uint32_t stepping;
    bool has_fpu;
    bool has_mmx;
    bool has_sse;
    bool has_sse2;
    bool has_sse3;
    bool has_sse4_1;
    bool has_sse4_2;
    bool has_avx;
    bool has_avx2;
} hal_cpu_info_t;

// CPU context structure
typedef struct {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip;
    uint64_t rflags;
    uint64_t cr0, cr2, cr3, cr4;
    uint64_t dr0, dr1, dr2, dr3;
    uint64_t dr6, dr7;
    uint64_t tr, ldtr, gdtr;
    uint64_t fs_base, gs_base;
    uint64_t mxcsr, fcw;
    uint64_t x87_fpu[8];
} hal_cpu_context_t;

// CPU features flags
typedef enum {
    HAL_CPU_FEATURE_FPU = (1 << 0),
    HAL_CPU_FEATURE_MMX = (1 << 1),
    HAL_CPU_FEATURE_SSE = (1 << 2),
    HAL_CPU_FEATURE_SSE2 = (1 << 3),
    HAL_CPU_FEATURE_SSE3 = (1 << 4),
    HAL_CPU_FEATURE_SSE4_1 = (1 << 5),
    HAL_CPU_FEATURE_SSE4_2 = (1 << 6),
    HAL_CPU_FEATURE_AVX = (1 << 7),
    HAL_CPU_FEATURE_AVX2 = (1 << 8),
    HAL_CPU_FEATURE_FMA = (1 << 9),
    HAL_CPU_FEATURE_BMI1 = (1 << 10),
    HAL_CPU_FEATURE_BMI2 = (1 << 11),
    HAL_CPU_FEATURE_AES = (1 << 12),
    HAL_CPU_FEATURE_VMX = (1 << 13),
    HAL_CPU_FEATURE_RDRAND = (1 << 14),
    HAL_CPU_FEATURE_RDSEED = (1 << 15)
} hal_cpu_features_t;

// Memory information structure
typedef struct {
    uint64_t total_memory;
    uint64_t available_memory;
    uint32_t page_size;
    uint32_t page_count;
    uint32_t total_pages;
    uint32_t free_pages;
    uint32_t used_pages;
    bool has_paging;
    bool has_protection;
    bool has_nx;
    bool has_pae;
} hal_memory_info_t;

// Memory protection flags
typedef enum {
    HAL_MEM_PROT_NONE = 0x0,
    HAL_MEM_PROT_READ = 0x1,
    HAL_MEM_PROT_WRITE = 0x2,
    HAL_MEM_PROT_EXECUTE = 0x4,
    HAL_MEM_PROT_READ_WRITE = HAL_MEM_PROT_READ | HAL_MEM_PROT_WRITE,
    HAL_MEM_PROT_READ_EXECUTE = HAL_MEM_PROT_READ | HAL_MEM_PROT_EXECUTE,
    HAL_MEM_PROT_READ_WRITE_EXECUTE = HAL_MEM_PROT_READ | HAL_MEM_PROT_WRITE | HAL_MEM_PROT_EXECUTE
} hal_memory_prot_t;

// Interrupt information structure
typedef struct {
    uint32_t total_interrupts;
    uint32_t available_interrupts;
    uint32_t max_interrupt_priority;
    bool has_apic;
    bool has_pic;
    bool has_msi;
    bool has_msix;
} hal_interrupt_info_t;

// Timer information structure
typedef struct {
    uint32_t frequency;
    uint32_t resolution;
    uint32_t min_frequency;
    uint32_t max_frequency;
    bool has_pit;
    bool has_apic_timer;
    bool has_tsc;
    bool has_hpet;
    uint64_t tsc_frequency;
} hal_timer_info_t;

// Platform information structure
typedef struct {
    char platform_name[64];
    char manufacturer[64];
    char product_name[64];
    char version[32];
    char serial_number[32];
    char uuid[37];
    char bios_vendor[64];
    char bios_version[32];
    char bios_release_date[16];
    char system_manufacturer[64];
    char system_product_name[64];
    char system_version[32];
    char system_serial_number[32];
    char baseboard_manufacturer[64];
    char baseboard_product_name[64];
    char baseboard_version[32];
    char baseboard_serial_number[32];
    char chassis_manufacturer[64];
    char chassis_type[32];
    char chassis_version[32];
    char chassis_serial_number[32];
    char chassis_asset_tag[32];
    hal_architecture_t architecture;
    uint32_t architecture_bits;
    bool is_virtualized;
    bool is_debug;
    bool is_secure_boot;
    bool has_acpi;
    bool has_smbios;
    bool has_dmi;
    bool has_uefi;
    uint32_t cpu_count;
    uint32_t memory_size;
    uint32_t disk_count;
    uint32_t network_count;
    uint32_t display_count;
    uint32_t usb_count;
    uint32_t audio_count;
    uint32_t input_count;
} hal_platform_info_t;

// Forward declarations for operation interfaces
typedef struct hal_cpu_ops hal_cpu_ops_t;
typedef struct hal_memory_ops hal_memory_ops_t;
typedef struct hal_interrupt_ops hal_interrupt_ops_t;
typedef struct hal_timer_ops hal_timer_ops_t;
typedef struct hal_platform_ops hal_platform_ops_t;

// Forward declarations for stats structures
typedef struct hal_memory_stats {
    uint64_t total_memory;
    uint64_t available_memory;
    uint64_t used_memory;
    uint64_t free_memory;
    uint32_t total_blocks;
    uint32_t used_blocks;
    uint32_t free_blocks;
    uint32_t fragmentation_count;
    uint64_t total_allocations;
    uint64_t total_frees;
    uint64_t total_alloc_time;
    uint64_t total_free_time;
    uint64_t max_alloc_time;
    uint64_t max_free_time;
    uint64_t min_alloc_time;
    uint64_t min_free_time;
    uint32_t alloc_count;
    uint32_t free_count;
    uint32_t fail_count;
} hal_memory_stats_t;

typedef struct hal_interrupt_stats {
    uint32_t total_interrupts;
    uint32_t handled_interrupts;
    uint32_t failed_interrupts;
    uint32_t nested_interrupts;
    uint64_t total_interrupt_time;
    uint64_t max_interrupt_time;
    uint64_t min_interrupt_time;
    uint32_t interrupt_counts[256];
} hal_interrupt_stats_t;

typedef struct hal_timer_stats {
    uint64_t total_timer_calls;
    uint64_t total_timer_time;
    uint64_t max_timer_time;
    uint64_t min_timer_time;
    uint32_t timer_count;
    uint32_t delay_count;
    uint64_t total_delay_time;
} hal_timer_stats_t;

typedef struct hal_platform_stats {
    uint32_t power_events;
    uint32_t thermal_events;
    uint32_t device_events;
    uint64_t uptime;
    uint32_t sleep_count;
    uint64_t total_sleep_time;
    uint32_t cpu_usage;
    uint32_t memory_usage;
    uint32_t temperature;
} hal_platform_stats_t;

// CPU statistics structure
typedef struct hal_cpu_stats {
    uint64_t total_instructions;
    uint64_t total_cycles;
    uint64_t cache_hits;
    uint64_t cache_misses;
    uint64_t branch_predictions;
    uint64_t branch_mispredictions;
    uint32_t temperature;
    uint32_t utilization;
    uint64_t idle_time;
    uint64_t busy_time;
} hal_cpu_stats_t;

// Interrupt handler function type
typedef void (*hal_interrupt_handler_t)(uint32_t vector, void* context);

// Forward declarations for other structures
typedef struct hal_virtualization_info {
    bool present;
    char name[16];
    uint32_t version;
    uint64_t features;
} hal_virtualization_info_t;

typedef struct hal_hypervisor_info {
    bool present;
    char name[16];
    uint32_t version;
    uint64_t features;
} hal_hypervisor_info_t;

typedef struct hal_battery_info hal_battery_info_t;
typedef struct hal_acpi_table hal_acpi_table_t;
typedef struct hal_smbios_info hal_smbios_info_t;
typedef struct hal_smbios_structure hal_smbios_structure_t;
typedef struct hal_dmi_info hal_dmi_info_t;
typedef struct hal_dmi_structure hal_dmi_structure_t;
typedef struct hal_uefi_info hal_uefi_info_t;
typedef struct hal_uefi_variable hal_uefi_variable_t;
typedef struct hal_secure_boot_info hal_secure_boot_info_t;
typedef struct hal_tpm_info hal_tpm_info_t;
typedef struct hal_device_info hal_device_info_t;
typedef struct hal_platform_config hal_platform_config_t;

// Main HAL interface structure
typedef struct {
    const char* arch_name;
    uint32_t arch_bits;
    hal_architecture_t architecture;
    hal_cpu_ops_t* cpu_ops;
    hal_memory_ops_t* memory_ops;
    hal_interrupt_ops_t* interrupt_ops;
    hal_timer_ops_t* timer_ops;
    hal_platform_ops_t* platform_ops;
    bool initialized;
    hal_platform_info_t platform_info;
} hal_interface_t;

// HAL initialization and management
hal_result_t hal_init(void);
hal_result_t hal_shutdown(void);
hal_result_t hal_reset(void);
bool hal_is_initialized(void);

// Architecture detection and information
hal_architecture_t hal_detect_architecture(void);
const char* hal_get_architecture_name(hal_architecture_t arch);
const char* hal_get_architecture_description(hal_architecture_t arch);
bool hal_is_architecture_supported(hal_architecture_t arch);
hal_architecture_t hal_get_current_architecture(void);
hal_interface_t* hal_get_interface(void);

// Platform information
hal_result_t hal_get_platform_info(hal_platform_info_t* info);
const char* hal_get_platform_name(void);
const char* hal_get_manufacturer(void);
const char* hal_get_product_name(void);
const char* hal_get_version(void);
const char* hal_get_serial_number(void);
hal_result_t hal_get_interrupt_info(hal_interrupt_info_t* info);
hal_result_t hal_get_timer_info(hal_timer_info_t* info);

// HAL utility functions
uint64_t hal_get_timestamp(void);
uint64_t hal_get_cpu_cycles(void);
void hal_delay(uint32_t milliseconds);
void hal_halt(void);
void hal_nop(void);

// HAL debugging and logging
void hal_dump_platform_info(void);
void hal_dump_cpu_info(void);
void hal_dump_memory_info(void);
void hal_dump_interrupt_info(void);
void hal_dump_timer_info(void);

// HAL error handling
const char* hal_get_error_string(hal_result_t error);
void hal_panic(const char* message);
void hal_assert(bool condition, const char* message);

// HAL configuration
typedef struct {
    bool enable_debug_logging;
    bool enable_performance_monitoring;
    bool enable_error_recovery;
    uint32_t max_recovery_attempts;
    uint32_t interrupt_stack_size;
} hal_config_t;

hal_result_t hal_set_config(const hal_config_t* config);
hal_config_t hal_get_config(void);
hal_result_t hal_validate_config(const hal_config_t* config);

// HAL macros for convenience
#define HAL_SUCCESS_CHECK(result) \
    do { \
        hal_result_t _result = (result); \
        if (_result != HAL_SUCCESS) { \
            hal_panic(hal_get_error_string(_result)); \
        } \
    } while(0)

#define HAL_ASSERT(condition) hal_assert(condition, #condition)
#define HAL_PANIC(message) hal_panic(message)

// HAL feature detection macros
#define HAL_HAS_FEATURE(info, feature) ((info)->features & (feature))
#define HAL_CPU_HAS_FPU(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_FPU)
#define HAL_CPU_HAS_SSE(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_SSE)
#define HAL_CPU_HAS_SSE2(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_SSE2)
#define HAL_CPU_HAS_AVX(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_AVX)
#define HAL_CPU_HAS_AVX2(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_AVX2)

// HAL architecture detection macros
#define HAL_IS_X86_64() (hal_get_current_architecture() == HAL_ARCH_X86_64)
#define HAL_IS_I386() (hal_get_current_architecture() == HAL_ARCH_I386)
#define HAL_IS_ARM64() (hal_get_current_architecture() == HAL_ARCH_ARM64)
#define HAL_IS_RISCV64() (hal_get_current_architecture() == HAL_ARCH_RISCV64)

// HAL bit-width macros
#define HAL_IS_64BIT() (hal_get_interface()->arch_bits == 64)
#define HAL_IS_32BIT() (hal_get_interface()->arch_bits == 32)

// HAL validation macros
#define HAL_VALIDATE_PARAM(param) \
    do { \
        if (!(param)) { \
            return HAL_ERROR_INVALID_PARAM; \
        } \
    } while(0)

#define HAL_VALIDATE_STATE(condition) \
    do { \
        if (!(condition)) { \
            return HAL_ERROR_INVALID_STATE; \
        } \
    } while(0)
