#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../types.h"
#include "exception.h"
#include "interrupt.h"

// =============================================================================
// KOS - Interrupt Service Routines Interface
// =============================================================================

// ISR types
typedef enum {
    KOS_ISR_TYPE_EXCEPTION = 0,
    KOS_ISR_TYPE_IRQ = 1,
    KOS_ISR_TYPE_SOFTWARE = 2,
    KOS_ISR_TYPE_NMI = 3
} kos_isr_type_t;

// ISR states
typedef enum {
    KOS_ISR_STATE_INACTIVE = 0,
    KOS_ISR_STATE_ACTIVE = 1,
    KOS_ISR_STATE_HANDLING = 2,
    KOS_ISR_STATE_COMPLETED = 3,
    KOS_ISR_STATE_ERROR = 4
} kos_isr_state_t;

// ISR return codes
typedef enum {
    KOS_ISR_SUCCESS = 0,
    KOS_ISR_ERROR_INVALID_ISR = -1,
    KOS_ISR_ERROR_ALREADY_HANDLED = -2,
    KOS_ISR_ERROR_NESTING_TOO_DEEP = -3,
    KOS_ISR_ERROR_STACK_OVERFLOW = -4,
    KOS_ISR_ERROR_HANDLER_NULL = -5
} kos_isr_result_t;

// ISR descriptor structure
typedef struct {
    uint32_t vector;
    kos_isr_type_t type;
    kos_isr_state_t state;
    kos_interrupt_handler_t handler;
    void* context;
    uint32_t priority;
    uint32_t count;
    uint64_t total_time;
    uint64_t last_time;
    uint64_t max_time;
    uint64_t min_time;
    char name[32];
    bool enabled;
    bool trace_enabled;
} kos_isr_descriptor_t;

// ISR context structure
typedef struct {
    uint32_t vector;
    kos_isr_type_t type;
    kos_isr_state_t state;
    uint64_t timestamp;
    uint64_t start_time;
    uint32_t nesting_level;
    void* register_context;
    void* stack_pointer;
    bool is_nested;
    bool has_error_code;
    uint32_t error_code;
} kos_isr_context_t;

// ISR table structure
typedef struct {
    kos_isr_descriptor_t descriptors[256];
    uint32_t count;
    uint32_t max_vector;
    bool initialized;
} kos_isr_table_t;

// ISR configuration
typedef struct {
    bool enable_statistics;
    bool enable_timing;
    bool enable_tracing;
    bool enable_debug;
    uint32_t max_nesting_level;
    uint32_t interrupt_stack_size;
    bool auto_enable_handlers;
} kos_isr_config_t;

// ISR statistics
typedef struct {
    uint32_t total_isr_calls;
    uint32_t exception_isr_calls;
    uint32_t irq_isr_calls;
    uint32_t software_isr_calls;
    uint32_t nmi_isr_calls;
    uint32_t nested_isr_calls;
    uint32_t failed_isr_calls;
    uint64_t total_isr_time;
    uint64_t max_isr_time;
    uint64_t min_isr_time;
    uint32_t isr_counts[256];
} kos_isr_stats_t;

// ISR management functions
kos_result_t kos_isr_init(const kos_isr_config_t* config);
kos_result_t kos_isr_register(uint32_t vector, kos_isr_type_t type, kos_interrupt_handler_t handler, void* context, const char* name);
kos_result_t kos_isr_unregister(uint32_t vector);
kos_result_t kos_isr_enable(uint32_t vector);
kos_result_t kos_isr_disable(uint32_t vector);
kos_result_t kos_isr_set_priority(uint32_t vector, uint32_t priority);

// ISR execution functions
kos_isr_result_t kos_isr_execute(uint32_t vector, void* register_context);
kos_isr_result_t kos_isr_execute_exception(uint32_t vector, void* register_context, uint32_t error_code);
kos_isr_result_t kos_isr_execute_irq(uint32_t vector, void* register_context);
kos_isr_result_t kos_isr_execute_nested(uint32_t vector, void* register_context);

// ISR state management functions
kos_isr_state_t kos_isr_get_state(uint32_t vector);
kos_result_t kos_isr_set_state(uint32_t vector, kos_isr_state_t state);
bool kos_isr_is_active(uint32_t vector);
bool kos_isr_is_enabled(uint32_t vector);

// ISR descriptor functions
const kos_isr_descriptor_t* kos_isr_get_descriptor(uint32_t vector);
kos_result_t kos_isr_get_all_descriptors(kos_isr_descriptor_t* descriptors, uint32_t* count);
kos_result_t kos_isr_set_context(uint32_t vector, void* context);
void* kos_isr_get_context(uint32_t vector);

// ISR statistics functions
const kos_isr_stats_t* kos_isr_get_stats(void);
void kos_isr_reset_stats(void);
kos_result_t kos_isr_dump_stats(void);
kos_result_t kos_isr_get_vector_stats(uint32_t vector, uint32_t* count, uint64_t* total_time, uint64_t* max_time, uint64_t* min_time);

// ISR debugging functions
void kos_isr_dump_descriptor(uint32_t vector);
void kos_isr_dump_all_descriptors(void);
void kos_isr_dump_context(const kos_isr_context_t* context);
void kos_isr_dump_stack_trace(uint32_t vector);
kos_result_t kos_isr_trace_enable(uint32_t vector);
kos_result_t kos_isr_trace_disable(uint32_t vector);
bool kos_isr_is_trace_enabled(uint32_t vector);

// ISR timing functions
kos_result_t kos_isr_start_timing(uint32_t vector);
kos_result_t kos_isr_end_timing(uint32_t vector);
uint64_t kos_isr_get_last_time(uint32_t vector);
uint64_t kos_isr_get_total_time(uint32_t vector);
uint64_t kos_isr_get_max_time(uint32_t vector);
uint64_t kos_isr_get_min_time(uint32_t vector);

// ISR validation functions
bool kos_isr_is_valid_vector(uint32_t vector);
bool kos_isr_is_exception_vector(uint32_t vector);
bool kos_isr_is_irq_vector(uint32_t vector);
kos_isr_type_t kos_isr_get_type(uint32_t vector);
kos_result_t kos_isr_validate_handler(kos_interrupt_handler_t handler);

// ISR utility functions
const char* kos_isr_get_name(uint32_t vector);
kos_result_t kos_isr_set_name(uint32_t vector, const char* name);
uint32_t kos_isr_get_count(void);
uint32_t kos_isr_get_enabled_count(void);
uint32_t kos_isr_get_active_count(void);

// ISR configuration functions
kos_result_t kos_isr_set_config(const kos_isr_config_t* config);
kos_isr_config_t kos_isr_get_config(void);
kos_result_t kos_isr_validate_config(const kos_isr_config_t* config);

// ISR table management functions
kos_result_t kos_isr_table_init(void);
kos_isr_table_t* kos_isr_table_get(void);
kos_result_t kos_isr_table_add(const kos_isr_descriptor_t* descriptor);
kos_result_t kos_isr_table_remove(uint32_t vector);
kos_isr_descriptor_t* kos_isr_table_find(uint32_t vector);

// ISR stack management functions
kos_result_t kos_isr_stack_init(size_t stack_size);
void* kos_isr_stack_allocate(size_t size);
void kos_isr_stack_free(void* ptr);
bool kos_isr_stack_is_valid(void* ptr);
size_t kos_isr_stack_get_remaining(void);

// ISR nesting management functions
kos_isr_result_t kos_isr_enter_nest(uint32_t vector);
kos_isr_result_t kos_isr_exit_nest(uint32_t vector);
uint32_t kos_isr_get_nesting_level(void);
bool kos_isr_is_nested(void);
kos_result_t kos_isr_set_max_nesting(uint32_t max_level);

// ISR error handling functions
kos_result_t kos_isr_handle_error(uint32_t vector, kos_isr_result_t error);
void kos_isr_log_error(uint32_t vector, kos_isr_result_t error, const char* message);
kos_result_t kos_isr_recover_from_error(uint32_t vector, kos_isr_result_t error);

// ISR macros for convenience
#define KOS_ISR_REGISTER(vector, type, handler, context) kos_isr_register(vector, type, handler, context, #handler)
#define KOS_ISR_UNREGISTER(vector) kos_isr_unregister(vector)
#define KOS_ISR_ENABLE(vector) kos_isr_enable(vector)
#define KOS_ISR_DISABLE(vector) kos_isr_disable(vector)

// ISR execution macros
#define KOS_ISR_EXECUTE(vector, context) kos_isr_execute(vector, context)
#define KOS_ISR_EXECUTE_EXCEPTION(vector, context, error) kos_isr_execute_exception(vector, context, error)
#define KOS_ISR_EXECUTE_IRQ(vector, context) kos_isr_execute_irq(vector, context)

// ISR state macros
#define KOS_ISR_GET_STATE(vector) kos_isr_get_state(vector)
#define KOS_ISR_SET_STATE(vector, state) kos_isr_set_state(vector, state)
#define KOS_ISR_IS_ACTIVE(vector) kos_isr_is_active(vector)
#define KOS_ISR_IS_ENABLED(vector) kos_isr_is_enabled(vector)

// ISR statistics macros
#define KOS_ISR_GET_STATS() kos_isr_get_stats()
#define KOS_ISR_RESET_STATS() kos_isr_reset_stats()
#define KOS_ISR_DUMP_STATS() kos_isr_dump_stats()

// ISR debugging macros
#define KOS_ISR_DUMP_DESCRIPTOR(vector) kos_isr_dump_descriptor(vector)
#define KOS_ISR_DUMP_ALL() kos_isr_dump_all_descriptors()
#define KOS_ISR_TRACE_ENABLE(vector) kos_isr_trace_enable(vector)
#define KOS_ISR_TRACE_DISABLE(vector) kos_isr_trace_disable(vector)

// ISR timing macros
#define KOS_ISR_START_TIME(vector) kos_isr_start_timing(vector)
#define KOS_ISR_END_TIME(vector) kos_isr_end_timing(vector)
#define KOS_ISR_GET_TIME(vector) kos_isr_get_last_time(vector)

// ISR validation macros
#define KOS_ISR_IS_VALID(vector) kos_isr_is_valid_vector(vector)
#define KOS_ISR_IS_EXCEPTION(vector) kos_isr_is_exception_vector(vector)
#define KOS_ISR_IS_IRQ(vector) kos_isr_is_irq_vector(vector)

// ISR context access macros
#define KOS_ISR_VECTOR(context) ((context)->vector)
#define KOS_ISR_TYPE(context) ((context)->type)
#define KOS_ISR_STATE(context) ((context)->state)
#define KOS_ISR_TIMESTAMP(context) ((context)->timestamp)
#define KOS_ISR_START_TIME(context) ((context)->start_time)
#define KOS_ISR_NESTING_LEVEL(context) ((context)->nesting_level)
#define KOS_ISR_REGISTER_CONTEXT(context) ((context)->register_context)
#define KOS_ISR_STACK_POINTER(context) ((context)->stack_pointer)
#define KOS_ISR_IS_NESTED_CONTEXT(context) ((context)->is_nested)
#define KOS_ISR_HAS_ERROR_CODE(context) ((context)->has_error_code)
#define KOS_ISR_ERROR_CODE(context) ((context)->error_code)

// ISR descriptor access macros
#define KOS_ISR_HANDLER(descriptor) ((descriptor)->handler)
#define KOS_ISR_CONTEXT(descriptor) ((descriptor)->context)
#define KOS_ISR_PRIORITY(descriptor) ((descriptor)->priority)
#define KOS_ISR_COUNT(descriptor) ((descriptor)->count)
#define KOS_ISR_TOTAL_TIME(descriptor) ((descriptor)->total_time)
#define KOS_ISR_LAST_TIME(descriptor) ((descriptor)->last_time)
#define KOS_ISR_MAX_TIME(descriptor) ((descriptor)->max_time)
#define KOS_ISR_MIN_TIME(descriptor) ((descriptor)->min_time)
#define KOS_ISR_NAME(descriptor) ((descriptor)->name)
#define KOS_ISR_ENABLED(descriptor) ((descriptor)->enabled)
#define KOS_ISR_TRACE_ENABLED(descriptor) ((descriptor)->trace_enabled)
