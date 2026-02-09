#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// =============================================================================
// KOS - HAL Core Types (No Dependencies)
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

// Boolean type
typedef bool hal_bool_t;

// Architecture types
typedef enum {
    HAL_ARCH_UNKNOWN = 0,
    HAL_ARCH_X86_64 = 1,
    HAL_ARCH_I386 = 2,
    HAL_ARCH_ARM64 = 3,
    HAL_ARCH_RISCV64 = 4
} hal_architecture_t;

// CPU feature flags
typedef enum {
    HAL_CPU_FEATURE_NONE = 0x00000000,
    HAL_CPU_FEATURE_FPU = 0x00000001,
    HAL_CPU_FEATURE_MMX = 0x00000002,
    HAL_CPU_FEATURE_SSE = 0x00000004,
    HAL_CPU_FEATURE_SSE2 = 0x00000008,
    HAL_CPU_FEATURE_SSE3 = 0x00000010,
    HAL_CPU_FEATURE_SSSE3 = 0x00000020,
    HAL_CPU_FEATURE_SSE4_1 = 0x00000040,
    HAL_CPU_FEATURE_SSE4_2 = 0x00000080,
    HAL_CPU_FEATURE_AVX = 0x00000100,
    HAL_CPU_FEATURE_AVX2 = 0x00000200,
    HAL_CPU_FEATURE_AVX512 = 0x00000400,
    HAL_CPU_FEATURE_VMX = 0x00000800,
    HAL_CPU_FEATURE_SVM = 0x00001000,
    HAL_CPU_FEATURE_AES = 0x00002000,
    HAL_CPU_FEATURE_SHA = 0x00004000,
    HAL_CPU_FEATURE_RDRAND = 0x00008000,
    HAL_CPU_FEATURE_RDSEED = 0x00010000,
    HAL_CPU_FEATURE_TSC = 0x00020000,
    HAL_CPU_FEATURE_RDTSCP = 0x00040000,
    HAL_CPU_FEATURE_INVPCID = 0x00080000,
    HAL_CPU_FEATURE_SMAP = 0x00100000,
    HAL_CPU_FEATURE_SMEP = 0x00200000,
    HAL_CPU_FEATURE_FSGSBASE = 0x00400000,
    HAL_CPU_FEATURE_CET = 0x00800000
} hal_cpu_feature_t;

// Memory types
typedef enum {
    HAL_MEMORY_TYPE_UNKNOWN = 0,
    HAL_MEMORY_TYPE_RAM = 1,
    HAL_MEMORY_TYPE_ROM = 2,
    HAL_MEMORY_TYPE_FLASH = 3,
    HAL_MEMORY_TYPE_MMIO = 4,
    HAL_MEMORY_TYPE_FRAMEBUFFER = 5,
    HAL_MEMORY_TYPE_ACPI = 6,
    HAL_MEMORY_TYPE_RESERVED = 7
} hal_memory_type_t;

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

// Interrupt types
typedef enum {
    HAL_INTERRUPT_TYPE_UNKNOWN = 0,
    HAL_INTERRUPT_TYPE_HARDWARE = 1,
    HAL_INTERRUPT_TYPE_SOFTWARE = 2,
    HAL_INTERRUPT_TYPE_EXCEPTION = 3,
    HAL_INTERRUPT_TYPE_NMI = 4
} hal_interrupt_type_t;

// Interrupt priority levels
typedef enum {
    HAL_INTERRUPT_PRIORITY_LOW = 0,
    HAL_INTERRUPT_PRIORITY_NORMAL = 1,
    HAL_INTERRUPT_PRIORITY_HIGH = 2,
    HAL_INTERRUPT_PRIORITY_CRITICAL = 3
} hal_interrupt_priority_t;

// Timer types
typedef enum {
    HAL_TIMER_TYPE_UNKNOWN = 0,
    HAL_TIMER_TYPE_PIT = 1,
    HAL_TIMER_TYPE_APIC = 2,
    HAL_TIMER_TYPE_TSC = 3,
    HAL_TIMER_TYPE_HPET = 4,
    HAL_TIMER_TYPE_LOCAL_APIC = 5
} hal_timer_type_t;

// Platform power states
typedef enum {
    HAL_POWER_STATE_UNKNOWN = 0,
    HAL_POWER_STATE_WORKING = 1,
    HAL_POWER_STATE_SLEEPING = 2,
    HAL_POWER_STATE_HIBERNATING = 3,
    HAL_POWER_STATE_SOFT_OFF = 4,
    HAL_POWER_STATE_MECHANICAL_OFF = 5
} hal_power_state_t;

// Platform reset types
typedef enum {
    HAL_RESET_TYPE_UNKNOWN = 0,
    HAL_RESET_TYPE_COLD = 1,
    HAL_RESET_TYPE_WARM = 2,
    HAL_RESET_TYPE_HARD = 3,
    HAL_RESET_TYPE_SOFT = 4
} hal_reset_type_t;

// Virtualization types
typedef enum {
    HAL_VIRTUALIZATION_NONE = 0,
    HAL_VIRTUALIZATION_HVM = 1,
    HAL_VIRTUALIZATION_PVM = 2,
    HAL_VIRTUALIZATION_CONTAINER = 3
} hal_virtualization_type_t;

// Forward declarations for structures
struct hal_cpu_info;
struct hal_memory_info;
struct hal_interrupt_info;
struct hal_timer_info;
struct hal_platform_info;

// CPU information structure
typedef struct hal_cpu_info {
    hal_architecture_t architecture;
    char vendor[16];
    char cpu_model[64];
    char cpu_stepping[8];
    hal_u32_t family;
    hal_u32_t cpu_model_num;
    hal_u32_t cpu_stepping_num;
    hal_u64_t features;
    hal_frequency_t frequency;
    hal_u32_t cache_line_size;
    hal_u32_t cache_size;
    hal_u32_t tlb_size;
    hal_bool_t is_virtualized;
} hal_cpu_info_t;

// Memory information structure
typedef struct hal_memory_info {
    hal_virt_addr_t heap_base;
    hal_virt_addr_t heap_end;
    hal_size_t heap_size;
    hal_virt_addr_t kernel_base;
    hal_virt_addr_t kernel_end;
    hal_virt_addr_t user_base;
    hal_virt_addr_t user_end;
    hal_virt_addr_t direct_map_base;
    hal_virt_addr_t direct_map_end;
    hal_size_t page_size;
    hal_u32_t total_pages;
    hal_u32_t free_pages;
    hal_u32_t used_pages;
} hal_memory_info_t;

// Interrupt information structure
typedef struct hal_interrupt_info {
    hal_u32_t total_vectors;
    hal_u32_t enabled_vectors;
    hal_u32_t max_priority;
    hal_u32_t nesting_level;
    hal_u64_t total_interrupts;
    hal_u64_t failed_interrupts;
    hal_u64_t nested_interrupts;
} hal_interrupt_info_t;

// Timer information structure
typedef struct hal_timer_info {
    hal_timer_type_t type;
    hal_frequency_t frequency;
    hal_u64_t tick_count;
    hal_u64_t cycle_count;
    hal_u32_t resolution;
    hal_bool_t is_high_resolution;
} hal_timer_info_t;

// Platform information structure
typedef struct hal_platform_info {
    char platform_name[64];
    char manufacturer[64];
    char product_name[64];
    char version[32];
    char serial_number[32];
    hal_u32_t architecture_bits;
    hal_bool_t is_virtualized;
    hal_virtualization_type_t virtualization_type;
    hal_bool_t is_debug;
    hal_bool_t has_acpi;
    hal_bool_t has_mp;
} hal_platform_info_t;

// Statistics structures
typedef struct hal_cpu_stats {
    hal_u64_t total_instructions;
    hal_u64_t total_cycles;
    hal_u64_t cache_hits;
    hal_u64_t cache_misses;
    hal_u64_t branch_predictions;
    hal_u64_t branch_mispredictions;
    hal_u32_t temperature;
    hal_u32_t utilization;
    hal_u64_t idle_time;
    hal_u64_t busy_time;
} hal_cpu_stats_t;

typedef struct hal_memory_stats {
    hal_u64_t total_memory;
    hal_u64_t available_memory;
    hal_u64_t used_memory;
    hal_u64_t free_memory;
    hal_u32_t total_blocks;
    hal_u32_t used_blocks;
    hal_u32_t free_blocks;
    hal_u32_t fragmentation_count;
    hal_u64_t total_allocations;
    hal_u64_t total_frees;
    hal_u64_t total_alloc_time;
    hal_u64_t total_free_time;
    hal_u64_t max_alloc_time;
    hal_u64_t max_free_time;
    hal_u64_t min_alloc_time;
    hal_u64_t min_free_time;
    hal_u32_t alloc_count;
    hal_u32_t free_count;
    hal_u32_t fail_count;
} hal_memory_stats_t;

typedef struct hal_interrupt_stats {
    hal_u32_t total_interrupts;
    hal_u32_t handled_interrupts;
    hal_u32_t failed_interrupts;
    hal_u32_t nested_interrupts;
    hal_u64_t total_interrupt_time;
    hal_u64_t max_interrupt_time;
    hal_u64_t min_interrupt_time;
    hal_u32_t interrupt_counts[256];
} hal_interrupt_stats_t;

typedef struct hal_timer_stats {
    hal_u64_t total_timer_calls;
    hal_u64_t total_timer_time;
    hal_u64_t max_timer_time;
    hal_u64_t min_timer_time;
    hal_u32_t timer_count;
    hal_u32_t delay_count;
    hal_u64_t total_delay_time;
} hal_timer_stats_t;

typedef struct hal_platform_stats {
    hal_u32_t power_events;
    hal_u32_t thermal_events;
    hal_u32_t device_events;
    hal_u64_t uptime;
    hal_u32_t sleep_count;
    hal_u64_t total_sleep_time;
    hal_u32_t cpu_usage;
    hal_u32_t memory_usage;
    hal_u32_t temperature;
} hal_platform_stats_t;

// Virtualization information
typedef struct hal_virtualization_info {
    hal_bool_t present;
    char name[16];
    hal_u32_t version;
    hal_u64_t features;
} hal_virtualization_info_t;

// Hypervisor information
typedef struct hal_hypervisor_info {
    hal_bool_t present;
    char name[16];
    hal_u32_t version;
    hal_u32_t features;
} hal_hypervisor_info_t;

// CPU context structure
typedef struct hal_cpu_context {
    hal_u64_t rax, rbx, rcx, rdx;
    hal_u64_t rsi, rdi, rbp, rsp;
    hal_u64_t r8, r9, r10, r11, r12, r13, r14, r15;
    hal_u64_t rip;
    hal_u64_t rflags;
    hal_u64_t cr0, cr2, cr3, cr4;
    hal_u64_t dr0, dr1, dr2, dr3, dr6, dr7;
    hal_u64_t mxcsr, xcr0;
} hal_cpu_context_t;

// Interrupt context structure
typedef struct hal_interrupt_context {
    hal_u32_t vector;
    hal_u32_t error_code;
    hal_u64_t rip;
    hal_u64_t cs;
    hal_u64_t rflags;
    hal_u64_t rsp;
    hal_u64_t ss;
    hal_cpu_context_t cpu_context;
} hal_interrupt_context_t;

// Function pointer types
typedef void (*hal_interrupt_handler_t)(hal_u32_t vector, void* context);
typedef void (*hal_timer_handler_t)(hal_u32_t timer_id);
typedef void (*hal_cpu_handler_t)(hal_u32_t exception_type, hal_cpu_context_t* context);

// Utility macros
#define HAL_ALIGN_UP(x, a) (((x) + (a) - 1) & ~((a) - 1))
#define HAL_ALIGN_DOWN(x, a) ((x) & ~((a) - 1))
#define HAL_IS_ALIGNED(x, a) (((x) & ((a) - 1)) == 0)

#define HAL_KB(x) ((x) * 1024)
#define HAL_MB(x) ((x) * 1024 * 1024)
#define HAL_GB(x) ((x) * 1024 * 1024 * 1024)

#define HAL_BIT(x) (1ULL << (x))
#define HAL_BIT_MASK(x) ((1ULL << (x)) - 1)
#define HAL_BIT_SET(x, b) ((x) |= (b))
#define HAL_BIT_CLEAR(x, b) ((x) &= ~(b))
#define HAL_BIT_TEST(x, b) (((x) & (b)) != 0)

// Feature testing macros
#define HAL_HAS_FEATURE(info, feature) (((info)->features & (feature)) != 0)
#define HAL_CPU_HAS_FPU(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_FPU)
#define HAL_CPU_HAS_SSE(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_SSE)
#define HAL_CPU_HAS_SSE2(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_SSE2)
#define HAL_CPU_HAS_AVX(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_AVX)
#define HAL_CPU_HAS_AVX2(info) HAL_HAS_FEATURE(info, HAL_CPU_FEATURE_AVX2)

// Memory barrier macros
#define HAL_MEMORY_BARRIER() __sync_synchronize()
#define HAL_COMPILER_BARRIER() __asm__ volatile("" ::: "memory")

// String macros
#define HAL_STRLEN(s) (sizeof(s) - 1)

// Common macros
#define HAL_MIN(a, b) ((a) < (b) ? (a) : (b))
#define HAL_MAX(a, b) ((a) > (b) ? (a) : (b))
#define HAL_ABS(x) ((x) < 0 ? -(x) : (x))
#define HAL_CLAMP(x, min, max) (((x) < (min)) ? (min) : ((x) > (max)) ? (max) : (x))

// Array macros
#define HAL_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define HAL_ARRAY_END(arr) ((arr) + HAL_ARRAY_SIZE(arr))

// Container macros
#define HAL_CONTAINER_OF(ptr, type, member) \
    ((type*)((char*)(ptr) - offsetof(type, member)))

// Validation macros
#define HAL_IS_VALID_POINTER(ptr) ((ptr) != NULL)
#define HAL_IS_VALID_HANDLE(handle) ((handle) != 0)

// Common attributes
#define HAL_PACKED __attribute__((packed))
#define HAL_ALIGNED(n) __attribute__((aligned(n)))
#define HAL_NORETURN __attribute__((noreturn))
#define HAL_UNUSED __attribute__((unused))
#define HAL_WEAK __attribute__((weak))
#define HAL_INLINE inline
#define HAL_ALWAYS_INLINE __attribute__((always_inline))
#define HAL_NOINLINE __attribute__((noinline))

// Section attributes
#define HAL_SECTION(name) __attribute__((section(#name)))
#define HAL_INIT_TEXT HAL_SECTION(".init_text")
#define HAL_INIT_DATA HAL_SECTION(".init_data")

// Calling conventions
#define HAL_CDECL __attribute__((cdecl))
#define HAL_STDCALL __attribute__((stdcall))
#define HAL_FASTCALL __attribute__((fastcall))

// Atomic operations
#define HAL_ATOMIC_LOAD(ptr) __sync_fetch_add((ptr), 0)
#define HAL_ATOMIC_STORE(ptr, val) __sync_lock_test_and_set((ptr), (val))
#define HAL_ATOMIC_ADD(ptr, val) __sync_fetch_add((ptr), (val))
#define HAL_ATOMIC_SUB(ptr, val) __sync_fetch_sub((ptr), (val))
#define HAL_ATOMIC_AND(ptr, val) __sync_fetch_and((ptr), (val))
#define HAL_ATOMIC_OR(ptr, val) __sync_fetch_or((ptr), (val))
#define HAL_ATOMIC_XOR(ptr, val) __sync_fetch_xor((ptr), (val))
#define HAL_ATOMIC_CAS(ptr, expected, desired) __sync_val_compare_and_swap((ptr), (expected), (desired))

// Prefetch macros
#define HAL_PREFETCH_READ(addr) __builtin_prefetch((addr), 0, 3)
#define HAL_PREFETCH_WRITE(addr) __builtin_prefetch((addr), 1, 3)
#define HAL_PREFETCH_LOCALITY(addr, locality) __builtin_prefetch((addr), (locality), 3)

// Likely/unlikely macros
#define HAL_LIKELY(x) __builtin_expect(!!(x), 1)
#define HAL_UNLIKELY(x) __builtin_expect(!!(x), 0)

// Utility functions
hal_result_t hal_memory_barrier(void);
hal_result_t hal_compiler_barrier(void);
size_t hal_strlen(const char* str);
char* hal_strcpy(char* dest, const char* src);
int hal_strcmp(const char* str1, const char* str2);
char* hal_strncpy(char* dest, const char* src, size_t n);
int hal_strncmp(const char* str1, const char* str2, size_t n);
void* hal_memcpy(void* dest, const void* src, size_t n);
void* hal_memset(void* dest, int c, size_t n);
int hal_memcmp(const void* ptr1, const void* ptr2, size_t n);
void* hal_memmove(void* dest, const void* src, size_t n);
