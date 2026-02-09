#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// =============================================================================
// KOS - HAL Types (Utility Only)
// =============================================================================

// Include main interface first to get all types
#include "hal_interface.h"

// Additional utility macros only
#define HAL_STRLEN(s) hal_strlen((s))
#define HAL_MEMORY_BARRIER() __sync_synchronize()
#define HAL_COMPILER_BARRIER() __asm__ volatile("" ::: "memory")

// Utility function declarations
size_t hal_strlen(const char* str);

// Handle types
typedef uint32_t hal_handle_t;
typedef uint32_t hal_device_id_t;
typedef uint32_t hal_irq_t;
typedef uint32_t hal_vector_t;

// Time types
typedef uint64_t hal_time_t;
typedef uint64_t hal_timestamp_t;
typedef uint32_t hal_frequency_t;

// Memory alignment types
typedef uint32_t hal_align_t;
typedef uint32_t hal_offset_t;

// Status and result types
typedef uint32_t hal_status_t;
typedef uint32_t hal_error_t;

// Generic pointer types
typedef void* hal_ptr_t;
typedef const void* hal_const_ptr_t;

// Function pointer types
typedef void (*hal_func_t)(void);
typedef hal_result_t (*hal_result_func_t)(void);
typedef void (*hal_callback_t)(void* context);

// Generic data types
typedef uint8_t hal_u8_t;
typedef uint16_t hal_u16_t;
typedef uint32_t hal_u32_t;
typedef uint64_t hal_u64_t;
typedef int8_t hal_i8_t;
typedef int16_t hal_i16_t;
typedef int32_t hal_i32_t;
typedef int64_t hal_i64_t;

// Boolean type
typedef bool hal_bool_t;

// Common constants
#define HAL_NULL ((void*)0)
#define HAL_NULL_HANDLE ((hal_handle_t)0)
#define HAL_INVALID_HANDLE ((hal_handle_t)-1)
#define HAL_INVALID_ID ((hal_device_id_t)-1)
#define HAL_INVALID_IRQ ((hal_irq_t)-1)
#define HAL_INVALID_VECTOR ((hal_vector_t)-1)

// Common bit manipulation macros
#define HAL_BIT(x) (1U << (x))
#define HAL_BITMASK(bits) ((1U << (bits)) - 1)
#define HAL_BIT_SET(var, bit) ((var) |= (bit))
#define HAL_BIT_CLEAR(var, bit) ((var) &= ~(bit))
#define HAL_BIT_TOGGLE(var, bit) ((var) ^= (bit))
#define HAL_BIT_TEST(var, bit) (((var) & (bit)) != 0)

// Common alignment macros
#define HAL_ALIGN_UP(size, align) (((size) + (align) - 1) & ~((align) - 1))
#define HAL_ALIGN_DOWN(size, align) ((size) & ~((align) - 1))
#define HAL_IS_ALIGNED(addr, align) (((uintptr_t)(addr) & ((align) - 1)) == 0)

// Common size macros
#define HAL_KB (1024)
#define HAL_MB (1024 * HAL_KB)
#define HAL_GB (1024 * HAL_MB)
#define HAL_TB (1024 * HAL_GB)

// Common time macros
#define HAL_MS_TO_NS(ms) ((ms) * 1000000ULL)
#define HAL_US_TO_NS(us) ((us) * 1000ULL)
#define HAL_NS_TO_MS(ns) ((ns) / 1000000ULL)
#define HAL_NS_TO_US(ns) ((ns) / 1000ULL)

// Common math macros
#define HAL_MIN(a, b) ((a) < (b) ? (a) : (b))
#define HAL_MAX(a, b) ((a) > (b) ? (a) : (b))
#define HAL_ABS(x) ((x) < 0 ? -(x) : (x))
#define HAL_CLAMP(x, min, max) (((x) < (min)) ? (min) : ((x) > (max) ? (max) : (x)))

// Common string macros
#define HAL_STRCPY(dest, src) hal_strcpy((dest), (src))
#define HAL_STRNCPY(dest, src, n) hal_strncpy((dest), (src), (n))
#define HAL_STRCMP(a, b) hal_strcmp((a), (b))
#define HAL_STRNCMP(a, b, n) hal_strncmp((a), (b), (n))

// Common array macros
#define HAL_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define HAL_ARRAY_END(arr) ((arr) + HAL_ARRAY_SIZE(arr))

// Common container macros
#define HAL_CONTAINER_OF(ptr, type, member) \
    ((type*)((char*)(ptr) - offsetof(type, member)))

// Common validation macros
#define HAL_IS_POWER_OF_TWO(x) (((x) & ((x) - 1)) == 0)
#define HAL_IS_ALIGNED(addr, align) (((uintptr_t)(addr) & ((align) - 1)) == 0)

// Common debug macros
#ifdef HAL_DEBUG
    #define HAL_DEBUG_LOG(msg, ...) hal_debug_log(msg, ##__VA_ARGS__)
    #define HAL_DEBUG_ASSERT(condition) HAL_ASSERT(condition)
#else
    #define HAL_DEBUG_LOG(msg, ...) ((void)0)
    #define HAL_DEBUG_ASSERT(condition) ((void)0)
#endif

// Common performance macros
#ifdef HAL_PERFORMANCE
    #define HAL_PERF_START() hal_perf_start()
    #define HAL_PERF_END(name) hal_perf_end(name)
    #define HAL_PERF_COUNT(name) hal_perf_count(name)
#else
    #define HAL_PERF_START() ((void)0)
    #define HAL_PERF_END(name) ((void)0)
    #define HAL_PERF_COUNT(name) ((void)0)
#endif

// Common error macros
#define HAL_RETURN_IF_ERROR(result) \
    do { \
        hal_result_t _result = (result); \
        if (_result != HAL_SUCCESS) { \
            return _result; \
        } \
    } while(0)

#define HAL_GOTO_IF_ERROR(result, label) \
    do { \
        hal_result_t _result = (result); \
        if (_result != HAL_SUCCESS) { \
            goto label; \
        } \
    } while(0)

// Common loop macros
#define HAL_FOR_EACH(item, array) \
    for (size_t _i = 0; _i < HAL_ARRAY_SIZE(array); _i++) \
        if ((item) = &(array)[_i], true)

#define HAL_FOR_EACH_PTR(ptr, start, end) \
    for (hal_ptr_t _ptr = (start); _ptr < (end); _ptr++)

// Common memory macros
#define HAL_MEMCPY(dest, src, n) hal_memcpy((dest), (src), (n))
#define HAL_MEMSET(dest, c, n) hal_memset((dest), (c), (n))
#define HAL_MEMCMP(a, b, n) hal_memcmp((a), (b), (n))
#define HAL_MEMMOVE(dest, src, n) hal_memmove((dest), (src), (n))

// Common conversion macros
#define HAL_PTR_TO_INT(ptr) ((intptr_t)(ptr))
#define HAL_INT_TO_PTR(int) ((void*)(intptr_t)(int))
#define HAL_PTR_TO_UINT64(ptr) ((uint64_t)(uintptr_t)(ptr))
#define HAL_UINT64_TO_PTR(uint64) ((void*)(uintptr_t)(uint64_t))

// Common endian macros
#define HAL_IS_LITTLE_ENDIAN() (hal_is_little_endian())
#define HAL_IS_BIG_ENDIAN() (!hal_is_little_endian())
#define HAL_SWAP16(x) hal_swap16((x))
#define HAL_SWAP32(x) hal_swap32((x))
#define HAL_SWAP64(x) hal_swap64((x))

// Common I/O port macros
#define HAL_INB(port) hal_inb((port))
#define HAL_OUTB(port, val) hal_outb((port), (val))
#define HAL_INW(port) hal_inw((port))
#define HAL_OUTW(port, val) hal_outw((port), (val))
#define HAL_INL(port) hal_inl((port))
#define HAL_OUTL(port, val) hal_outl((port), (val))

// Common atomic operations (simplified)
#define HAL_ATOMIC_LOAD(ptr) __sync_fetch_add((ptr), 0)
#define HAL_ATOMIC_STORE(ptr, val) __sync_lock_test_and_set((ptr), (val))
#define HAL_ATOMIC_ADD(ptr, val) __sync_fetch_add((ptr), (val))
#define HAL_ATOMIC_SUB(ptr, val) __sync_fetch_sub((ptr), (val))
#define HAL_ATOMIC_AND(ptr, val) __sync_fetch_and((ptr), (val))
#define HAL_ATOMIC_OR(ptr, val) __sync_fetch_or((ptr), (val))
#define HAL_ATOMIC_XOR(ptr, val) __sync_fetch_xor((ptr), (val))
#define HAL_ATOMIC_CAS(ptr, expected, desired) __sync_val_compare_and_swap((ptr), (expected), (desired))

// Common memory barriers
#define HAL_CPU_BARRIER() __asm__ volatile("mfence" ::: "memory")

// Common prefetch macros
#define HAL_PREFETCH_READ(addr) __builtin_prefetch((addr), 0, 3)
#define HAL_PREFETCH_WRITE(addr) __builtin_prefetch((addr), 1, 3)
#define HAL_PREFETCH_LOCALITY(addr, locality) __builtin_prefetch((addr), (locality), 3)

// Common likely/unlikely macros
#define HAL_LIKELY(x) __builtin_expect(!!(x), 1)
#define HAL_UNLIKELY(x) __builtin_expect(!!(x), 0)

// Common no-return macro
#define HAL_NORETURN __attribute__((noreturn))

// Common packed macro
#define HAL_PACKED __attribute__((packed))

// Common aligned macro
#define HAL_ALIGNED(n) __attribute__((aligned(n)))

// Common weak macro
#define HAL_WEAK __attribute__((weak))

// Common constructor/destructor macros
#define HAL_CONSTRUCTOR __attribute__((constructor))
#define HAL_DESTRUCTOR __attribute__((destructor))

// Common section macros
#define HAL_SECTION(name) __attribute__((section(#name)))
#define HAL_INIT_TEXT HAL_SECTION(".init_text")
#define HAL_INIT_DATA HAL_SECTION(".init_data")

// Common inline macro
#define HAL_ALWAYS_INLINE __attribute__((always_inline))
#define HAL_INLINE inline

// Common no-inline macro
#define HAL_NOINLINE __attribute__((noinline))

// Common used macro
#define HAL_USED __attribute__((used))

// Common unused macro
#define HAL_UNUSED __attribute__((unused))

// Common deprecated macro
#define HAL_DEPRECATED __attribute__((deprecated))

// Common fallthrough macro
#define HAL_FALLTHROUGH __attribute__((fallthrough))

// Common sentinel macros
#define HAL_SENTINEL __attribute__((sentinel))

// Common packed structures
#define HAL_PACK_BEGIN HAL_PACKED
#define HAL_PACK_END

// Common forward declarations
#define HAL_FORWARD_DECLARE(type) typedef struct type type; typedef struct type* type##_p

// Common function attributes
#define HAL_EXPORT __attribute__((visibility("default")))
#define HAL_IMPORT __attribute__((visibility("default")))
#define HAL_LOCAL __attribute__((visibility("hidden")))
#define HAL_INTERNAL __attribute__((visibility("hidden")))

// Common calling conventions
#define HAL_CDECL __attribute__((cdecl))
#define HAL_STDCALL __attribute__((stdcall))
#define HAL_FASTCALL __attribute__((fastcall))
#define HAL_THISCALL __attribute__((thiscall))

// Common register preservation
#define HAL_PRESERVE_ALL __attribute__((preserve_all))
#define HAL_PRESERVE_NONE __attribute__((preserve_none))

// Common no-return functions
#define HAL_NO_RETURN __attribute__((noreturn))

// Common const functions
#define HAL_CONST __attribute__((const))

// Common pure functions
#define HAL_PURE __attribute__((pure))

// Common malloc/free attributes
#define HAL_MALLOC __attribute__((malloc))
#define HAL_FREE __attribute__((free))
#define HAL_ALLOC_SIZE(x) __attribute__((alloc_size(x)))
#define HAL_ALLOC_ALIGN(x) __attribute__((alloc_align(x)))
#define HAL_DEALLOC_SIZE(x) __attribute__((dealloc_size(x)))
#define HAL_DEALLOC_ALIGN(x) __attribute__((dealloc_align(x)))

// Common format attributes
#define HAL_PRINTF_FORMAT(fmt, args) __attribute__((format(printf, fmt, args)))
#define HAL_SCANF_FORMAT(fmt, args) __attribute__((format(scanf, fmt, args)))

// Common cleanup attribute
#define HAL_CLEANUP(func) __attribute__((cleanup(func)))

// Common warning suppression
#define HAL_SUPPRESS_WARNING(warning) __pragma(warning(suppress: warning))

// Common error suppression
#define HAL_SUPPRESS_ERROR(error) __pragma(error(suppress: error))

// Common unused parameter suppression
#define HAL_SUPPRESS_UNUSED_PARAM __attribute__((unused))

// Common unused variable suppression
#define HAL_SUPPRESS_UNUSED_VAR __attribute__((unused))

// Common unused function suppression
#define HAL_SUPPRESS_UNUSED_FUNC __attribute__((unused))

// Common fallthrough suppression
#define HAL_SUPPRESS_FALLTHROUGH __pragma(GCC diagnostic ignored "-Wfallthrough"))

// Common shadow suppression
#define HAL_SUPPRESS_SHADOW __pragma(GCC diagnostic ignored "-Wshadow"))

// Common unused-but-set-variable suppression
#define HAL_SUPPRESS_UNSET_VAR __pragma(GCC diagnostic ignored "-Wunused-but-set-variable"))
