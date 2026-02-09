#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../types.h"
#include "exception.h"

// =============================================================================
// KOS - Interrupt Management Interface
// =============================================================================

// Interrupt types
typedef enum {
    KOS_INTERRUPT_HARDWARE = 0,
    KOS_INTERRUPT_SOFTWARE = 1,
    KOS_INTERRUPT_EXCEPTION = 2,
    KOS_INTERRUPT_NMI = 3
} kos_interrupt_type_t;

// Interrupt priority levels
typedef enum {
    KOS_INTERRUPT_PRIORITY_LOW = 0,
    KOS_INTERRUPT_PRIORITY_NORMAL = 1,
    KOS_INTERRUPT_PRIORITY_HIGH = 2,
    KOS_INTERRUPT_PRIORITY_CRITICAL = 3,
    KOS_INTERRUPT_PRIORITY_NMI = 4
} kos_interrupt_priority_t;

// Interrupt states
typedef enum {
    KOS_INTERRUPT_DISABLED = 0,
    KOS_INTERRUPT_ENABLED = 1,
    KOS_INTERRUPT_MASKED = 2,
    KOS_INTERRUPT_NESTED = 3,
    KOS_INTERRUPT_ACTIVE = 4
} kos_interrupt_state_t;

// IRQ numbers (x86_64)
typedef enum {
    KOS_IRQ_TIMER = 0,
    KOS_IRQ_KEYBOARD = 1,
    KOS_IRQ_CASCADE = 2,
    KOS_IRQ_COM2 = 3,
    KOS_IRQ_COM1 = 4,
    KOS_IRQ_LPT2 = 5,
    KOS_IRQ_FLOPPY = 6,
    KOS_IRQ_LPT1 = 7,
    KOS_IRQ_CMOS_RTC = 8,
    KOS_IRQ_FREE1 = 9,
    KOS_IRQ_FREE2 = 10,
    KOS_IRQ_FREE3 = 11,
    KOS_IRQ_MOUSE = 12,
    KOS_IRQ_MATH_COPROCESSOR = 13,
    KOS_IRQ_PRIMARY_ATA = 14,
    KOS_IRQ_SECONDARY_ATA = 15,
    KOS_IRQ_COUNT = 16
} kos_irq_t;

// Interrupt handler function type
typedef void (*kos_interrupt_handler_t)(uint32_t irq, void* context);

// Interrupt context structure
typedef struct {
    uint32_t irq;
    uint32_t vector;
    kos_interrupt_type_t type;
    kos_interrupt_priority_t priority;
    kos_interrupt_state_t state;
    uint64_t timestamp;
    void* device_context;
    uint32_t nesting_level;
    bool is_nested;
} kos_interrupt_context_t;

// Interrupt descriptor
typedef struct {
    uint32_t irq;
    uint32_t vector;
    kos_interrupt_handler_t handler;
    void* device_context;
    kos_interrupt_priority_t priority;
    bool enabled;
    uint32_t count;
    uint64_t last_time;
    uint64_t total_time;
    uint64_t max_time;
    uint64_t min_time;
    char name[32];
} kos_interrupt_descriptor_t;

// Interrupt management configuration
typedef struct {
    bool enable_nesting;
    bool enable_priority;
    bool enable_statistics;
    bool enable_debug;
    uint32_t max_nesting_level;
    uint32_t interrupt_stack_size;
} kos_interrupt_config_t;

// Interrupt statistics
typedef struct {
    uint32_t total_interrupts;
    uint32_t interrupt_counts[KOS_IRQ_COUNT];
    uint32_t nested_interrupts;
    uint32_t masked_interrupts;
    uint32_t failed_interrupts;
    uint64_t total_time;
    uint64_t max_interrupt_time;
    uint64_t min_interrupt_time;
    uint32_t last_interrupt_time;
    kos_irq_t last_irq;
} kos_interrupt_stats_t;

// Interrupt stack management
typedef struct {
    void* stack_base;
    void* stack_top;
    size_t stack_size;
    void* current_sp;
    uint32_t nesting_level;
    bool in_interrupt;
} kos_interrupt_stack_t;

// Interrupt management functions
kos_result_t kos_interrupt_init(const kos_interrupt_config_t* config);
kos_result_t kos_interrupt_register_handler(kos_irq_t irq, kos_interrupt_handler_t handler, void* context, const char* name);
kos_result_t kos_interrupt_unregister_handler(kos_irq_t irq);
kos_result_t kos_interrupt_enable(kos_irq_t irq);
kos_result_t kos_interrupt_disable(kos_irq_t irq);
kos_result_t kos_interrupt_mask(kos_irq_t irq);
kos_result_t kos_interrupt_unmask(kos_irq_t irq);

// Interrupt control functions
kos_interrupt_state_t kos_interrupt_get_state(void);
kos_result_t kos_interrupt_set_state(kos_interrupt_state_t state);
kos_result_t kos_interrupt_disable_all(void);
kos_result_t kos_interrupt_enable_all(void);
kos_result_t kos_interrupt_save_disable(void);
kos_result_t kos_interrupt_restore_enable(kos_interrupt_state_t state);

// Interrupt nesting functions
kos_result_t kos_interrupt_enter_nest(kos_irq_t irq);
kos_result_t kos_interrupt_exit_nest(kos_irq_t irq);
uint32_t kos_interrupt_get_nesting_level(void);
bool kos_interrupt_is_nested(void);
kos_result_t kos_interrupt_set_max_nesting(uint32_t max_level);

// Interrupt priority functions
kos_interrupt_priority_t kos_interrupt_get_priority(kos_irq_t irq);
kos_result_t kos_interrupt_set_priority(kos_irq_t irq, kos_interrupt_priority_t priority);
kos_result_t kos_interrupt_set_global_priority(kos_interrupt_priority_t priority);

// Interrupt handler functions
void kos_interrupt_handle(uint32_t irq);
void kos_interrupt_handle_nested(uint32_t irq);
kos_result_t kos_interrupt_dispatch(uint32_t irq);
kos_interrupt_handler_t kos_interrupt_get_handler(kos_irq_t irq);

// Interrupt statistics functions
const kos_interrupt_stats_t* kos_interrupt_get_stats(void);
void kos_interrupt_reset_stats(void);
kos_result_t kos_interrupt_dump_stats(void);
kos_result_t kos_interrupt_get_irq_stats(kos_irq_t irq, uint32_t* count, uint64_t* total_time);

// Interrupt debugging functions
void kos_interrupt_dump_context(const kos_interrupt_context_t* context);
void kos_interrupt_dump_descriptors(void);
void kos_interrupt_dump_stack(void);
kos_result_t kos_interrupt_trace_enable(kos_irq_t irq);
kos_result_t kos_interrupt_trace_disable(kos_irq_t irq);

// Interrupt stack management functions
kos_result_t kos_interrupt_stack_init(size_t stack_size);
kos_interrupt_stack_t* kos_interrupt_stack_get(void);
void* kos_interrupt_stack_allocate(size_t size);
void kos_interrupt_stack_free(void* ptr);
bool kos_interrupt_stack_is_valid(void* ptr);

// Interrupt utility functions
const char* kos_interrupt_get_name(kos_irq_t irq);
const char* kos_interrupt_get_description(kos_irq_t irq);
kos_interrupt_type_t kos_interrupt_get_type(kos_irq_t irq);
bool kos_interrupt_is_valid(kos_irq_t irq);
uint32_t kos_interrupt_get_vector(kos_irq_t irq);
kos_irq_t kos_interrupt_get_irq_from_vector(uint32_t vector);

// Interrupt timing functions
uint64_t kos_interrupt_get_timestamp(void);
kos_result_t kos_interrupt_start_timing(kos_irq_t irq);
kos_result_t kos_interrupt_end_timing(kos_irq_t irq);
uint64_t kos_interrupt_get_last_time(kos_irq_t irq);

// Interrupt configuration functions
kos_result_t kos_interrupt_set_config(const kos_interrupt_config_t* config);
kos_interrupt_config_t kos_interrupt_get_config(void);
kos_result_t kos_interrupt_validate_config(const kos_interrupt_config_t* config);

// Interrupt macros for convenience
#define KOS_INTERRUPT_HANDLER(name) static void name(uint32_t irq, void* context)
#define KOS_INTERRUPT_REGISTER(irq, handler, context) kos_interrupt_register_handler(irq, handler, context, #handler)
#define KOS_INTERRUPT_UNREGISTER(irq) kos_interrupt_unregister_handler(irq)
#define KOS_INTERRUPT_ENABLE(irq) kos_interrupt_enable(irq)
#define KOS_INTERRUPT_DISABLE(irq) kos_interrupt_disable(irq)

// Interrupt control macros
#define KOS_INTERRUPT_DISABLE_ALL() kos_interrupt_disable_all()
#define KOS_INTERRUPT_ENABLE_ALL() kos_interrupt_enable_all()
#define KOS_INTERRUPT_SAVE_DISABLE() kos_interrupt_save_disable()
#define KOS_INTERRUPT_RESTORE_ENABLE(state) kos_interrupt_restore_enable(state)

// Interrupt nesting macros
#define KOS_INTERRUPT_ENTER_NEST(irq) kos_interrupt_enter_nest(irq)
#define KOS_INTERRUPT_EXIT_NEST(irq) kos_interrupt_exit_nest(irq)
#define KOS_INTERRUPT_GET_NESTING() kos_interrupt_get_nesting_level()
#define KOS_INTERRUPT_IS_NESTED() kos_interrupt_is_nested()

// Interrupt priority macros
#define KOS_INTERRUPT_SET_PRIORITY(irq, priority) kos_interrupt_set_priority(irq, priority)
#define KOS_INTERRUPT_GET_PRIORITY(irq) kos_interrupt_get_priority(irq)

// Interrupt statistics macros
#define KOS_INTERRUPT_GET_STATS() kos_interrupt_get_stats()
#define KOS_INTERRUPT_RESET_STATS() kos_interrupt_reset_stats()
#define KOS_INTERRUPT_DUMP_STATS() kos_interrupt_dump_stats()

// Interrupt debugging macros
#define KOS_INTERRUPT_DUMP_CONTEXT(context) kos_interrupt_dump_context(context)
#define KOS_INTERRUPT_DUMP_DESCRIPTORS() kos_interrupt_dump_descriptors()
#define KOS_INTERRUPT_DUMP_STACK() kos_interrupt_dump_stack()

// Interrupt timing macros
#define KOS_INTERRUPT_START_TIME(irq) kos_interrupt_start_timing(irq)
#define KOS_INTERRUPT_END_TIME(irq) kos_interrupt_end_timing(irq)
#define KOS_INTERRUPT_GET_TIME(irq) kos_interrupt_get_last_time(irq)

// Interrupt validation macros
#define KOS_INTERRUPT_IS_VALID(irq) kos_interrupt_is_valid(irq)
#define KOS_INTERRUPT_HAS_ERROR_CODE(irq) kos_interrupt_has_error_code(irq)
#define KOS_INTERRUPT_GET_VECTOR(irq) kos_interrupt_get_vector(irq)

// Interrupt context access macros
#define KOS_INTERRUPT_IRQ(context) ((context)->irq)
#define KOS_INTERRUPT_VECTOR(context) ((context)->vector)
#define KOS_INTERRUPT_TYPE(context) ((context)->type)
#define KOS_INTERRUPT_PRIORITY(context) ((context)->priority)
#define KOS_INTERRUPT_STATE(context) ((context)->state)
#define KOS_INTERRUPT_TIMESTAMP(context) ((context)->timestamp)
#define KOS_INTERRUPT_DEVICE_CONTEXT(context) ((context)->device_context)
#define KOS_INTERRUPT_NESTING_LEVEL(context) ((context)->nesting_level)
#define KOS_INTERRUPT_IS_NESTED_CONTEXT(context) ((context)->is_nested)
