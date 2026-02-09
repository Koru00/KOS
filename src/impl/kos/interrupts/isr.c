#include "kos/interrupts/isr.h"
#include "kos/utils/string.h"
#include "kos/memory/memory.h"
#include "debug/debug.h"

// =============================================================================
// KOS - Interrupt Service Routines Implementation
// =============================================================================

// Forward declarations
extern memory_manager_t g_memory_manager;
extern uint64_t kos_get_timestamp(void);

// Global ISR state
static kos_isr_config_t g_isr_config = {0};
static kos_isr_stats_t g_isr_stats = {0};
static kos_isr_table_t g_isr_table = {0};
static bool g_isr_initialized = false;
static uint32_t g_isr_nesting_level = 0;
static void* g_isr_stack = NULL;
static size_t g_isr_stack_size = 0;

// ISR initialization
kos_result_t kos_isr_init(const kos_isr_config_t* config) {
    if (!config) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    // Copy configuration
    g_isr_config = *config;
    
    // Initialize ISR table
    kos_memset(&g_isr_table, 0, sizeof(kos_isr_table_t));
    g_isr_table.max_vector = 255;
    
    // Initialize statistics
    kos_memset(&g_isr_stats, 0, sizeof(kos_isr_stats_t));
    g_isr_stats.min_isr_time = UINT64_MAX;
    
    // Initialize ISR stack
    if (config->interrupt_stack_size > 0) {
        g_isr_stack_size = config->interrupt_stack_size;
        g_isr_stack = kos_memory_alloc(&g_memory_manager, g_isr_stack_size);
        if (!g_isr_stack) {
            log_error("Failed to allocate ISR stack");
            return KOS_ERROR_OUT_OF_MEMORY;
        }
    }
    
    // Set up default exception handlers
    for (int i = 0; i < 32; i++) {
        g_isr_table.descriptors[i].vector = i;
        g_isr_table.descriptors[i].type = KOS_ISR_TYPE_EXCEPTION;
        g_isr_table.descriptors[i].state = KOS_ISR_STATE_INACTIVE;
        g_isr_table.descriptors[i].priority = 3; // High priority for exceptions
        g_isr_table.descriptors[i].enabled = true;
        g_isr_table.descriptors[i].min_time = UINT64_MAX;
        kos_strcpy(g_isr_table.descriptors[i].name, "Exception");
    }
    
    // Set up default IRQ handlers
    for (int i = 32; i < 48; i++) {
        g_isr_table.descriptors[i].vector = i;
        g_isr_table.descriptors[i].type = KOS_ISR_TYPE_IRQ;
        g_isr_table.descriptors[i].state = KOS_ISR_STATE_INACTIVE;
        g_isr_table.descriptors[i].priority = 1; // Normal priority for IRQs
        g_isr_table.descriptors[i].enabled = false;
        g_isr_table.descriptors[i].min_time = UINT64_MAX;
        kos_strcpy(g_isr_table.descriptors[i].name, "IRQ");
    }
    
    g_isr_initialized = true;
    
    log_info("ISR system initialized");
    return KOS_SUCCESS;
}

// Register ISR
kos_result_t kos_isr_register(uint32_t vector, kos_isr_type_t type, kos_interrupt_handler_t handler, void* context, const char* name) {
    if (!g_isr_initialized) {
        log_error("ISR system not initialized");
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (vector > 255) {
        log_error("Invalid vector number: %d", vector);
        return KOS_ERROR_INVALID_PARAM;
    }
    
    if (!handler) {
        log_error("Null handler for vector %d", vector);
        return KOS_ERROR_INVALID_PARAM;
    }
    
    kos_isr_descriptor_t* desc = &g_isr_table.descriptors[vector];
    
    // Check if ISR is already registered
    if (desc->handler != NULL) {
        log_warn("ISR already registered for vector %d, replacing", vector);
    }
    
    // Set up descriptor
    desc->vector = vector;
    desc->type = type;
    desc->handler = handler;
    desc->context = context;
    desc->state = KOS_ISR_STATE_INACTIVE;
    desc->priority = 1; // Default priority
    desc->count = 0;
    desc->total_time = 0;
    desc->last_time = 0;
    desc->max_time = 0;
    desc->min_time = UINT64_MAX;
    desc->enabled = g_isr_config.auto_enable_handlers;
    desc->trace_enabled = false;
    
    if (name) {
        kos_strncpy(desc->name, name, sizeof(desc->name) - 1);
        desc->name[sizeof(desc->name) - 1] = '\0';
    } else {
        kos_strcpy(desc->name, "Unknown");
    }
    
    // Update table count
    g_isr_table.count++;
    if (vector > g_isr_table.max_vector) {
        g_isr_table.max_vector = vector;
    }
    
    log_debug("Registered ISR for vector %d: %s", vector, desc->name);
    return KOS_SUCCESS;
}

// Unregister ISR
kos_result_t kos_isr_unregister(uint32_t vector) {
    if (!g_isr_initialized) {
        log_error("ISR system not initialized");
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (vector > 255) {
        log_error("Invalid vector number: %d", vector);
        return KOS_ERROR_INVALID_PARAM;
    }
    
    kos_isr_descriptor_t* desc = &g_isr_table.descriptors[vector];
    
    // Clear descriptor
    kos_memset(desc, 0, sizeof(kos_isr_descriptor_t));
    desc->vector = vector;
    desc->state = KOS_ISR_STATE_INACTIVE;
    desc->min_time = UINT64_MAX;
    kos_strcpy(desc->name, "Unregistered");
    
    // Update table count
    if (g_isr_table.count > 0) {
        g_isr_table.count--;
    }
    
    log_debug("Unregistered ISR for vector %d", vector);
    return KOS_SUCCESS;
}

// Execute ISR
kos_isr_result_t kos_isr_execute(uint32_t vector, void* register_context) {
    if (!g_isr_initialized) {
        log_error("ISR system not initialized");
        return KOS_ISR_ERROR_INVALID_ISR;
    }
    
    if (vector > 255) {
        log_error("Invalid vector number: %d", vector);
        return KOS_ISR_ERROR_INVALID_ISR;
    }
    
    kos_isr_descriptor_t* desc = &g_isr_table.descriptors[vector];
    
    // Check if ISR is registered and enabled
    if (!desc->handler) {
        log_error("No handler registered for vector %d", vector);
        return KOS_ISR_ERROR_HANDLER_NULL;
    }
    
    if (!desc->enabled) {
        log_warn("ISR disabled for vector %d", vector);
        return KOS_ISR_ERROR_ALREADY_HANDLED;
    }
    
    // Check nesting level
    if (g_isr_nesting_level >= g_isr_config.max_nesting_level) {
        log_error("ISR nesting level too deep: %d", g_isr_nesting_level);
        return KOS_ISR_ERROR_NESTING_TOO_DEEP;
    }
    
    // Create ISR context
    kos_isr_context_t context = {
        .vector = vector,
        .type = desc->type,
        .state = KOS_ISR_STATE_HANDLING,
        .timestamp = kos_get_timestamp(),
        .start_time = kos_get_timestamp(),
        .nesting_level = g_isr_nesting_level,
        .register_context = register_context,
        .stack_pointer = g_isr_stack ? (void*)((uintptr_t)g_isr_stack + g_isr_stack_size) : NULL,
        .is_nested = g_isr_nesting_level > 0,
        .has_error_code = false,
        .error_code = 0
    };
    
    // Update descriptor state
    desc->state = KOS_ISR_STATE_HANDLING;
    
    // Update statistics
    g_isr_stats.total_isr_calls++;
    g_isr_stats.isr_counts[vector]++;
    
    switch (desc->type) {
        case KOS_ISR_TYPE_EXCEPTION:
            g_isr_stats.exception_isr_calls++;
            break;
        case KOS_ISR_TYPE_IRQ:
            g_isr_stats.irq_isr_calls++;
            break;
        case KOS_ISR_TYPE_SOFTWARE:
            g_isr_stats.software_isr_calls++;
            break;
        case KOS_ISR_TYPE_NMI:
            g_isr_stats.nmi_isr_calls++;
            break;
    }
    
    // Handle nesting
    if (g_isr_nesting_level > 0) {
        g_isr_stats.nested_isr_calls++;
        return kos_isr_execute_nested(vector, register_context);
    }
    
    // Enter ISR
    g_isr_nesting_level++;
    
    // Call the handler
    desc->handler(vector, desc->context);
    
    // Exit ISR
    g_isr_nesting_level--;
    
    // Update timing statistics
    uint64_t end_time = kos_get_timestamp();
    uint64_t elapsed = end_time - context.start_time;
    
    desc->count++;
    desc->total_time += elapsed;
    desc->last_time = elapsed;
    
    if (elapsed > desc->max_time) {
        desc->max_time = elapsed;
    }
    if (elapsed < desc->min_time) {
        desc->min_time = elapsed;
    }
    
    // Update global statistics
    g_isr_stats.total_isr_time += elapsed;
    if (elapsed > g_isr_stats.max_isr_time) {
        g_isr_stats.max_isr_time = elapsed;
    }
    if (elapsed < g_isr_stats.min_isr_time) {
        g_isr_stats.min_isr_time = elapsed;
    }
    
    // Update descriptor state
    desc->state = KOS_ISR_STATE_COMPLETED;
    
    // Trace if enabled
    if (desc->trace_enabled) {
        log_debug("ISR %d (%s) executed in %llu cycles", vector, desc->name, elapsed);
    }
    
    return KOS_ISR_SUCCESS;
}

// Execute exception ISR
kos_isr_result_t kos_isr_execute_exception(uint32_t vector, void* register_context, uint32_t error_code) {
    if (!g_isr_initialized) {
        return KOS_ISR_ERROR_INVALID_ISR;
    }
    
    kos_isr_descriptor_t* desc = &g_isr_table.descriptors[vector];
    
    // Create ISR context with error code
    kos_isr_context_t context = {
        .vector = vector,
        .type = KOS_ISR_TYPE_EXCEPTION,
        .state = KOS_ISR_STATE_HANDLING,
        .timestamp = kos_get_timestamp(),
        .start_time = kos_get_timestamp(),
        .nesting_level = g_isr_nesting_level,
        .register_context = register_context,
        .stack_pointer = g_isr_stack ? (void*)((uintptr_t)g_isr_stack + g_isr_stack_size) : NULL,
        .is_nested = g_isr_nesting_level > 0,
        .has_error_code = true,
        .error_code = error_code
    };
    
    // Execute the ISR
    return kos_isr_execute(vector, register_context);
}

// Execute IRQ ISR
kos_isr_result_t kos_isr_execute_irq(uint32_t vector, void* register_context) {
    if (!g_isr_initialized) {
        return KOS_ISR_ERROR_INVALID_ISR;
    }
    
    // For IRQs, we need to handle the PIC EOI
    kos_isr_result_t result = kos_isr_execute(vector, register_context);
    
    // Send EOI to PIC if successful
    if (result == KOS_ISR_SUCCESS) {
        // This would involve writing to the PIC registers
        // For now, we just log it
        log_debug("Sent EOI for IRQ %d", vector - 32);
    }
    
    return result;
}

// Execute nested ISR
kos_isr_result_t kos_isr_execute_nested(uint32_t vector, void* register_context) {
    if (!g_isr_config.enable_statistics) {
        return KOS_ISR_ERROR_NESTING_TOO_DEEP;
    }
    
    log_debug("Executing nested ISR %d at level %d", vector, g_isr_nesting_level);
    
    // For nested ISRs, we need to save the current context and switch to a new stack
    // This is a simplified implementation
    
    return kos_isr_execute(vector, register_context);
}

// Get ISR state
kos_isr_state_t kos_isr_get_state(uint32_t vector) {
    if (vector > 255) {
        return KOS_ISR_STATE_ERROR;
    }
    
    return g_isr_table.descriptors[vector].state;
}

// Set ISR state
kos_result_t kos_isr_set_state(uint32_t vector, kos_isr_state_t state) {
    if (vector > 255) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    g_isr_table.descriptors[vector].state = state;
    return KOS_SUCCESS;
}

// Check if ISR is active
bool kos_isr_is_active(uint32_t vector) {
    if (vector > 255) {
        return false;
    }
    
    return g_isr_table.descriptors[vector].state == KOS_ISR_STATE_HANDLING;
}

// Check if ISR is enabled
bool kos_isr_is_enabled(uint32_t vector) {
    if (vector > 255) {
        return false;
    }
    
    return g_isr_table.descriptors[vector].enabled;
}

// Get ISR descriptor
const kos_isr_descriptor_t* kos_isr_get_descriptor(uint32_t vector) {
    if (vector > 255) {
        return NULL;
    }
    
    return &g_isr_table.descriptors[vector];
}

// Get ISR statistics
const kos_isr_stats_t* kos_isr_get_stats(void) {
    return &g_isr_stats;
}

// Reset ISR statistics
void kos_isr_reset_stats(void) {
    kos_memset(&g_isr_stats, 0, sizeof(kos_isr_stats_t));
    g_isr_stats.min_isr_time = UINT64_MAX;
    
    // Reset per-ISR statistics
    for (int i = 0; i < 256; i++) {
        kos_isr_descriptor_t* desc = &g_isr_table.descriptors[i];
        desc->count = 0;
        desc->total_time = 0;
        desc->last_time = 0;
        desc->max_time = 0;
        desc->min_time = UINT64_MAX;
    }
}

// Dump ISR statistics
kos_result_t kos_isr_dump_stats(void) {
    log_info("=== ISR Statistics ===");
    log_info("Total ISR calls: %u", g_isr_stats.total_isr_calls);
    log_info("Exception ISR calls: %u", g_isr_stats.exception_isr_calls);
    log_info("IRQ ISR calls: %u", g_isr_stats.irq_isr_calls);
    log_info("Software ISR calls: %u", g_isr_stats.software_isr_calls);
    log_info("NMI ISR calls: %u", g_isr_stats.nmi_isr_calls);
    log_info("Nested ISR calls: %u", g_isr_stats.nested_isr_calls);
    log_info("Failed ISR calls: %u", g_isr_stats.failed_isr_calls);
    log_info("Total ISR time: %llu cycles", g_isr_stats.total_isr_time);
    log_info("Max ISR time: %llu cycles", g_isr_stats.max_isr_time);
    log_info("Min ISR time: %llu cycles", g_isr_stats.min_isr_time);
    
    if (g_isr_stats.total_isr_calls > 0) {
        log_info("ISR counts by vector:");
        for (int i = 0; i < 256; i++) {
            if (g_isr_stats.isr_counts[i] > 0) {
                kos_isr_descriptor_t* desc = &g_isr_table.descriptors[i];
                log_info("  Vector %d (%s): %u calls, %llu cycles avg",
                         i, desc->name, g_isr_stats.isr_counts[i],
                         desc->count > 0 ? desc->total_time / desc->count : 0);
            }
        }
    }
    
    return KOS_SUCCESS;
}

// Dump ISR descriptor
void kos_isr_dump_descriptor(uint32_t vector) {
    if (vector > 255) {
        log_error("Invalid vector number: %d", vector);
        return;
    }
    
    kos_isr_descriptor_t* desc = &g_isr_table.descriptors[vector];
    
    log_info("=== ISR Descriptor %d ===", vector);
    log_info("Name: %s", desc->name);
    log_info("Type: %d", desc->type);
    log_info("State: %d", desc->state);
    log_info("Priority: %d", desc->priority);
    log_info("Enabled: %s", desc->enabled ? "Yes" : "No");
    log_info("Count: %u", desc->count);
    log_info("Total time: %llu cycles", desc->total_time);
    log_info("Last time: %llu cycles", desc->last_time);
    log_info("Max time: %llu cycles", desc->max_time);
    log_info("Min time: %llu cycles", desc->min_time);
    log_info("Trace enabled: %s", desc->trace_enabled ? "Yes" : "No");
}

// Dump all ISR descriptors
void kos_isr_dump_all_descriptors(void) {
    log_info("=== All ISR Descriptors ===");
    for (int i = 0; i < 256; i++) {
        kos_isr_descriptor_t* desc = &g_isr_table.descriptors[i];
        if (desc->handler) {
            log_info("Vector %d: %s - %s (type %d, priority %d, count %u)",
                     i, desc->name, desc->enabled ? "ENABLED" : "DISABLED",
                     desc->type, desc->priority, desc->count);
        }
    }
}

// Check if vector is valid
bool kos_isr_is_valid_vector(uint32_t vector) {
    return vector <= 255;
}

// Check if vector is exception
bool kos_isr_is_exception_vector(uint32_t vector) {
    return vector < 32;
}

// Check if vector is IRQ
bool kos_isr_is_irq_vector(uint32_t vector) {
    return vector >= 32 && vector < 48;
}

// Get ISR type
kos_isr_type_t kos_isr_get_type(uint32_t vector) {
    if (vector > 255) {
        return KOS_ISR_TYPE_EXCEPTION; // Default
    }
    
    return g_isr_table.descriptors[vector].type;
}

// Initialize ISR table
kos_result_t kos_isr_table_init(void) {
    kos_memset(&g_isr_table, 0, sizeof(kos_isr_table_t));
    g_isr_table.max_vector = 255;
    g_isr_table.initialized = true;
    
    return KOS_SUCCESS;
}

// Get ISR table
kos_isr_table_t* kos_isr_table_get(void) {
    return &g_isr_table;
}

// Find ISR descriptor
kos_isr_descriptor_t* kos_isr_table_find(uint32_t vector) {
    if (vector > 255) {
        return NULL;
    }
    
    return &g_isr_table.descriptors[vector];
}

// Enter ISR nesting
kos_result_t kos_isr_enter_nest(uint32_t vector) {
    if (g_isr_nesting_level >= g_isr_config.max_nesting_level) {
        log_error("ISR nesting level too deep: %d", g_isr_nesting_level);
        return KOS_ISR_ERROR_NESTING_TOO_DEEP;
    }
    
    g_isr_nesting_level++;
    log_debug("Entered ISR nesting level %d for vector %d", g_isr_nesting_level, vector);
    
    return KOS_ISR_SUCCESS;
}

// Exit ISR nesting
kos_result_t kos_isr_exit_nest(uint32_t vector) {
    if (g_isr_nesting_level == 0) {
        log_error("Attempting to exit ISR nesting with level 0");
        return KOS_ISR_ERROR_INVALID_ISR;
    }
    
    g_isr_nesting_level--;
    log_debug("Exited ISR nesting level %d for vector %d", g_isr_nesting_level, vector);
    
    return KOS_ISR_SUCCESS;
}

// Get nesting level
uint32_t kos_isr_get_nesting_level(void) {
    return g_isr_nesting_level;
}

// Check if nested
bool kos_isr_is_nested(void) {
    return g_isr_nesting_level > 0;
}
