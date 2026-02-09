#pragma once

#include <stdint.h>
#include <stddef.h>
#include "../types.h"

// =============================================================================
// KOS - Exception Handling Interface
// =============================================================================

// Exception numbers (x86_64)
typedef enum {
    KOS_EXCEPTION_DIVIDE_BY_ZERO = 0,
    KOS_EXCEPTION_DEBUG = 1,
    KOS_EXCEPTION_NMI = 2,
    KOS_EXCEPTION_BREAKPOINT = 3,
    KOS_EXCEPTION_OVERFLOW = 4,
    KOS_EXCEPTION_BOUND_RANGE = 5,
    KOS_EXCEPTION_INVALID_OPCODE = 6,
    KOS_EXCEPTION_DEVICE_NOT_AVAILABLE = 7,
    KOS_EXCEPTION_DOUBLE_FAULT = 8,
    KOS_EXCEPTION_INVALID_TSS = 10,
    KOS_EXCEPTION_SEGMENT_NOT_PRESENT = 11,
    KOS_EXCEPTION_STACK_SEGMENT_FAULT = 12,
    KOS_EXCEPTION_GENERAL_PROTECTION_FAULT = 13,
    KOS_EXCEPTION_PAGE_FAULT = 14,
    KOS_EXCEPTION_X87_FPU_ERROR = 16,
    KOS_EXCEPTION_ALIGNMENT_CHECK = 17,
    KOS_EXCEPTION_MACHINE_CHECK = 18,
    KOS_EXCEPTION_SIMD_FP_EXCEPTION = 19,
    KOS_EXCEPTION_VIRTUALIZATION = 20,
    KOS_EXCEPTION_SECURITY = 30,
    KOS_EXCEPTION_COUNT = 32
} kos_exception_t;

// Exception error codes
typedef enum {
    KOS_EXCEPTION_ERROR_NONE = 0,
    KOS_EXCEPTION_ERROR_SELECTOR = 1,
    KOS_EXCEPTION_ERROR_IDT = 2,
    KOS_EXCEPTION_ERROR_LDT = 3,
    KOS_EXCEPTION_ERROR_GDT = 4,
    KOS_EXCEPTION_ERROR_IDT_LIMIT = 5,
    KOS_EXCEPTION_ERROR_GDT_LIMIT = 6,
    KOS_EXCEPTION_ERROR_LDT_LIMIT = 7,
    KOS_EXCEPTION_ERROR_STACK = 8,
    KOS_EXCEPTION_ERROR_MEMORY = 9
} kos_exception_error_t;

// Exception context structure
typedef struct {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    
    uint64_t vector_number;
    uint64_t error_code;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) kos_exception_context_t;

// Exception handler function type
typedef void (*kos_exception_handler_t)(const kos_exception_context_t* context);

// Exception handling configuration
typedef struct {
    bool enable_logging;
    bool enable_recovery;
    bool enable_stack_trace;
    bool enable_debug_info;
    uint32_t max_recovery_attempts;
} kos_exception_config_t;

// Exception statistics
typedef struct {
    uint32_t exception_counts[KOS_EXCEPTION_COUNT];
    uint32_t total_exceptions;
    uint32_t recovery_attempts;
    uint32_t successful_recoveries;
    uint32_t failed_recoveries;
    uint32_t last_exception_time;
    kos_exception_t last_exception_type;
} kos_exception_stats_t;

// Exception handling functions
kos_result_t kos_exception_init(const kos_exception_config_t* config);
kos_result_t kos_exception_register_handler(kos_exception_t exception, kos_exception_handler_t handler);
kos_result_t kos_exception_unregister_handler(kos_exception_t exception);
void kos_exception_handle(const kos_exception_context_t* context);
kos_result_t kos_exception_try_recover(kos_exception_t exception, const kos_exception_context_t* context);
void kos_exception_panic(kos_exception_t exception, const kos_exception_context_t* context, const char* message);

// Exception information functions
const char* kos_exception_get_name(kos_exception_t exception);
const char* kos_exception_get_description(kos_exception_t exception);
bool kos_exception_has_error_code(kos_exception_t exception);
kos_exception_error_t kos_exception_get_error_type(uint64_t error_code);

// Exception statistics functions
const kos_exception_stats_t* kos_exception_get_stats(void);
void kos_exception_reset_stats(void);
kos_result_t kos_exception_dump_stats(void);

// Exception debugging functions
void kos_exception_dump_context(const kos_exception_context_t* context);
void kos_exception_dump_stack_trace(const kos_exception_context_t* context);
void kos_exception_dump_registers(const kos_exception_context_t* context);

// Exception recovery functions
kos_result_t kos_exception_recover_divide_by_zero(const kos_exception_context_t* context);
kos_result_t kos_exception_recover_page_fault(const kos_exception_context_t* context);
kos_result_t kos_exception_recover_general_protection_fault(const kos_exception_context_t* context);
kos_result_t kos_exception_recover_stack_fault(const kos_exception_context_t* context);

// Exception utility functions
bool kos_exception_is_fatal(kos_exception_t exception);
bool kos_exception_is_recoverable(kos_exception_t exception);
uint32_t kos_exception_get_priority(kos_exception_t exception);

// Exception stack management
kos_result_t kos_exception_stack_init(void);
void* kos_exception_get_stack(void);
size_t kos_exception_get_stack_size(void);
bool kos_exception_stack_is_valid(void* ptr);

// Exception logging functions
void kos_exception_log(kos_exception_t exception, const kos_exception_context_t* context, const char* message);
void kos_exception_log_error(kos_exception_t exception, const kos_exception_context_t* context, kos_result_t error);
void kos_exception_log_recovery(kos_exception_t exception, const kos_exception_context_t* context, kos_result_t result);

// Exception macros for convenience
#define KOS_EXCEPTION_HANDLER(name) static void name(const kos_exception_context_t* context)
#define KOS_EXCEPTION_REGISTER(exception, handler) kos_exception_register_handler(exception, handler)
#define KOS_EXCEPTION_UNREGISTER(exception) kos_exception_unregister_handler(exception)
#define KOS_EXCEPTION_PANIC(context, message) kos_exception_panic(0, context, message)

// Exception error checking macros
#define KOS_EXCEPTION_CHECK_ERROR(exception) (kos_exception_has_error_code(exception))
#define KOS_EXCEPTION_GET_ERROR(context) ((context)->error_code)
#define KOS_EXCEPTION_GET_VECTOR(context) ((context)->vector_number)

// Exception context access macros
#define KOS_EXCEPTION_RIP(context) ((context)->rip)
#define KOS_EXCEPTION_RSP(context) ((context)->rsp)
#define KOS_EXCEPTION_RAX(context) ((context)->rax)
#define KOS_EXCEPTION_RBX(context) ((context)->rbx)
#define KOS_EXCEPTION_RCX(context) ((context)->rcx)
#define KOS_EXCEPTION_RDX(context) ((context)->rdx)
#define KOS_EXCEPTION_RSI(context) ((context)->rsi)
#define KOS_EXCEPTION_RDI(context) ((context)->rdi)
