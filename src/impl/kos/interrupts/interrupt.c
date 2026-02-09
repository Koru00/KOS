#include "kos/interrupts/interrupt.h"
#include "kos/utils/string.h"
#include "kos/memory/memory.h"
#include "debug/debug.h"

// =============================================================================
// KOS - Interrupt Management Implementation
// =============================================================================

// Forward declarations
extern memory_manager_t g_memory_manager;
extern uint64_t kos_get_timestamp(void);

// Global interrupt state
static kos_interrupt_config_t g_interrupt_config = {0};
static kos_interrupt_stats_t g_interrupt_stats = {0};
static kos_interrupt_descriptor_t g_interrupt_descriptors[KOS_IRQ_COUNT] = {0};
static kos_interrupt_stack_t g_interrupt_stack = {0};
static bool g_interrupt_initialized = false;
static kos_interrupt_state_t g_interrupt_state = KOS_INTERRUPT_ENABLED;

// IRQ names and descriptions
static const char* g_irq_names[KOS_IRQ_COUNT] = {
    "Timer",
    "Keyboard",
    "Cascade",
    "COM2",
    "COM1",
    "LPT2",
    "Floppy",
    "LPT1",
    "CMOS RTC",
    "Free1",
    "Free2",
    "Free3",
    "Mouse",
    "Math Coprocessor",
    "Primary ATA",
    "Secondary ATA"
};

static const char* g_irq_descriptions[KOS_IRQ_COUNT] = {
    "System timer interrupt",
    "Keyboard controller interrupt",
    "Cascade interrupt (secondary PIC)",
    "COM2 serial port interrupt",
    "COM1 serial port interrupt",
    "LPT2 parallel port interrupt",
    "Floppy disk controller interrupt",
    "LPT1 parallel port interrupt",
    "CMOS real-time clock interrupt",
    "Free interrupt line 1",
    "Free interrupt line 2",
    "Free interrupt line 3",
    "PS/2 mouse controller interrupt",
    "x87 math coprocessor interrupt",
    "Primary ATA controller interrupt",
    "Secondary ATA controller interrupt"
};

// IRQ to vector mapping
static uint32_t g_irq_vectors[KOS_IRQ_COUNT] = {
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47
};

// Interrupt initialization
kos_result_t kos_interrupt_init(const kos_interrupt_config_t* config) {
    if (!config) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    // Copy configuration
    g_interrupt_config = *config;
    
    // Initialize statistics
    kos_memset(&g_interrupt_stats, 0, sizeof(kos_interrupt_stats_t));
    
    // Initialize interrupt descriptors
    for (int i = 0; i < KOS_IRQ_COUNT; i++) {
        kos_memset(&g_interrupt_descriptors[i], 0, sizeof(kos_interrupt_descriptor_t));
        g_interrupt_descriptors[i].irq = i;
        g_interrupt_descriptors[i].vector = g_irq_vectors[i];
        g_interrupt_descriptors[i].priority = KOS_INTERRUPT_PRIORITY_NORMAL;
        g_interrupt_descriptors[i].enabled = false;
        g_interrupt_descriptors[i].min_time = UINT64_MAX;
    }
    
    // Initialize interrupt stack
    if (config->interrupt_stack_size > 0) {
        kos_result_t result = kos_interrupt_stack_init(config->interrupt_stack_size);
        if (result != KOS_SUCCESS) {
            log_error("Failed to initialize interrupt stack");
            return result;
        }
    }
    
    // Enable interrupts globally
    g_interrupt_state = KOS_INTERRUPT_ENABLED;
    
    g_interrupt_initialized = true;
    
    log_info("Interrupt management initialized");
    return KOS_SUCCESS;
}

// Register interrupt handler
kos_result_t kos_interrupt_register_handler(kos_irq_t irq, kos_interrupt_handler_t handler, void* context, const char* name) {
    if (!g_interrupt_initialized) {
        log_error("Interrupt system not initialized");
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (irq >= KOS_IRQ_COUNT) {
        log_error("Invalid IRQ number: %d", irq);
        return KOS_ERROR_INVALID_PARAM;
    }
    
    if (!handler) {
        log_error("Null handler for IRQ %d", irq);
        return KOS_ERROR_INVALID_PARAM;
    }
    
    kos_interrupt_descriptor_t* desc = &g_interrupt_descriptors[irq];
    
    // Check if handler is already registered
    if (desc->handler != NULL) {
        log_warn("Handler already registered for IRQ %d, replacing", irq);
    }
    
    // Set up descriptor
    desc->handler = handler;
    desc->device_context = context;
    desc->enabled = true;
    desc->count = 0;
    desc->last_time = 0;
    
    if (name) {
        kos_strncpy(desc->name, name, sizeof(desc->name) - 1);
        desc->name[sizeof(desc->name) - 1] = '\0';
    } else {
        kos_strcpy(desc->name, "Unknown");
    }
    
    log_debug("Registered handler for IRQ %d (%s): %s", irq, kos_interrupt_get_name(irq), desc->name);
    return KOS_SUCCESS;
}

// Unregister interrupt handler
kos_result_t kos_interrupt_unregister_handler(kos_irq_t irq) {
    if (!g_interrupt_initialized) {
        log_error("Interrupt system not initialized");
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (irq >= KOS_IRQ_COUNT) {
        log_error("Invalid IRQ number: %d", irq);
        return KOS_ERROR_INVALID_PARAM;
    }
    
    kos_interrupt_descriptor_t* desc = &g_interrupt_descriptors[irq];
    
    // Clear descriptor
    desc->handler = NULL;
    desc->device_context = NULL;
    desc->enabled = false;
    kos_strcpy(desc->name, "Unregistered");
    
    log_debug("Unregistered handler for IRQ %d (%s)", irq, kos_interrupt_get_name(irq));
    return KOS_SUCCESS;
}

// Enable interrupt
kos_result_t kos_interrupt_enable(kos_irq_t irq) {
    if (!g_interrupt_initialized) {
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (irq >= KOS_IRQ_COUNT) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    kos_interrupt_descriptor_t* desc = &g_interrupt_descriptors[irq];
    
    if (!desc->handler) {
        log_warn("Attempting to enable IRQ %d with no handler", irq);
        return KOS_ERROR_INVALID_STATE;
    }
    
    desc->enabled = true;
    
    // Enable interrupt in hardware (PIC)
    // This would involve writing to the PIC registers
    // For now, we just mark it as enabled
    
    log_debug("Enabled IRQ %d (%s)", irq, kos_interrupt_get_name(irq));
    return KOS_SUCCESS;
}

// Disable interrupt
kos_result_t kos_interrupt_disable(kos_irq_t irq) {
    if (!g_interrupt_initialized) {
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (irq >= KOS_IRQ_COUNT) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    kos_interrupt_descriptor_t* desc = &g_interrupt_descriptors[irq];
    desc->enabled = false;
    
    // Disable interrupt in hardware (PIC)
    // This would involve writing to the PIC registers
    // For now, we just mark it as disabled
    
    log_debug("Disabled IRQ %d (%s)", irq, kos_interrupt_get_name(irq));
    return KOS_SUCCESS;
}

// Get interrupt state
kos_interrupt_state_t kos_interrupt_get_state(void) {
    return g_interrupt_state;
}

// Set interrupt state
kos_result_t kos_interrupt_set_state(kos_interrupt_state_t state) {
    if (!g_interrupt_initialized) {
        return KOS_ERROR_INVALID_STATE;
    }
    
    g_interrupt_state = state;
    
    switch (state) {
        case KOS_INTERRUPT_ENABLED:
            asm volatile("sti");
            break;
        case KOS_INTERRUPT_DISABLED:
            asm volatile("cli");
            break;
        case KOS_INTERRUPT_MASKED:
            // Mask all interrupts
            break;
        case KOS_INTERRUPT_NESTED:
            // Allow nested interrupts
            break;
    }
    
    return KOS_SUCCESS;
}

// Disable all interrupts
kos_result_t kos_interrupt_disable_all(void) {
    return kos_interrupt_set_state(KOS_INTERRUPT_DISABLED);
}

// Enable all interrupts
kos_result_t kos_interrupt_enable_all(void) {
    return kos_interrupt_set_state(KOS_INTERRUPT_ENABLED);
}

// Save and disable interrupts
kos_result_t kos_interrupt_save_disable(void) {
    kos_interrupt_state_t old_state = g_interrupt_state;
    kos_interrupt_set_state(KOS_INTERRUPT_DISABLED);
    return old_state;
}

// Restore interrupt state
kos_result_t kos_interrupt_restore_enable(kos_interrupt_state_t state) {
    return kos_interrupt_set_state(state);
}

// Enter nested interrupt
kos_result_t kos_interrupt_enter_nest(kos_irq_t irq) {
    if (!g_interrupt_initialized) {
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (!g_interrupt_config.enable_nesting) {
        return KOS_ERROR_NOT_IMPLEMENTED;
    }
    
    if (g_interrupt_stack.nesting_level >= g_interrupt_config.max_nesting_level) {
        log_error("Interrupt nesting level too deep: %d", g_interrupt_stack.nesting_level);
        return KOS_ERROR_INVALID_STATE;
    }
    
    g_interrupt_stack.nesting_level++;
    g_interrupt_stats.nested_interrupts++;
    
    log_debug("Entered nested interrupt %d, level: %d", irq, g_interrupt_stack.nesting_level);
    return KOS_SUCCESS;
}

// Exit nested interrupt
kos_result_t kos_interrupt_exit_nest(kos_irq_t irq) {
    if (!g_interrupt_initialized) {
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (g_interrupt_stack.nesting_level == 0) {
        log_error("Attempting to exit nested interrupt with level 0");
        return KOS_ERROR_INVALID_STATE;
    }
    
    g_interrupt_stack.nesting_level--;
    
    log_debug("Exited nested interrupt %d, level: %d", irq, g_interrupt_stack.nesting_level);
    return KOS_SUCCESS;
}

// Get nesting level
uint32_t kos_interrupt_get_nesting_level(void) {
    return g_interrupt_stack.nesting_level;
}

// Check if nested
bool kos_interrupt_is_nested(void) {
    return g_interrupt_stack.nesting_level > 0;
}

// Handle interrupt
void kos_interrupt_handle(uint32_t irq) {
    if (!g_interrupt_initialized) {
        log_error("Interrupt system not initialized");
        return;
    }
    
    if (irq >= KOS_IRQ_COUNT) {
        log_error("Invalid IRQ number: %d", irq);
        return;
    }
    
    kos_interrupt_descriptor_t* desc = &g_interrupt_descriptors[irq];
    
    // Check if handler is registered and enabled
    if (!desc->handler || !desc->enabled) {
        log_warn("No handler or disabled for IRQ %d", irq);
        g_interrupt_stats.failed_interrupts++;
        return;
    }
    
    // Update statistics
    g_interrupt_stats.total_interrupts++;
    g_interrupt_stats.interrupt_counts[irq]++;
    desc->count++;
    
    uint64_t start_time = kos_get_timestamp();
    
    // Create interrupt context
    kos_interrupt_context_t context = {
        .irq = irq,
        .vector = desc->vector,
        .type = KOS_INTERRUPT_HARDWARE,
        .priority = desc->priority,
        .state = KOS_INTERRUPT_ACTIVE,
        .timestamp = start_time,
        .device_context = desc->device_context,
        .nesting_level = g_interrupt_stack.nesting_level,
        .is_nested = g_interrupt_stack.nesting_level > 0
    };
    
    // Handle nesting if enabled
    if (g_interrupt_config.enable_nesting && g_interrupt_stack.nesting_level > 0) {
        kos_interrupt_handle_nested(irq);
        return;
    }
    
    // Call the handler
    desc->handler(irq, desc->device_context);
    
    uint64_t end_time = kos_get_timestamp();
    uint64_t elapsed = end_time - start_time;
    
    // Update timing statistics
    g_interrupt_stats.total_time += elapsed;
    desc->total_time += elapsed;
    desc->last_time = elapsed;
    
    if (elapsed > desc->max_time) {
        desc->max_time = elapsed;
    }
    if (elapsed < desc->min_time) {
        desc->min_time = elapsed;
    }
    
    if (elapsed > g_interrupt_stats.max_interrupt_time) {
        g_interrupt_stats.max_interrupt_time = elapsed;
    }
    if (elapsed < g_interrupt_stats.min_interrupt_time) {
        g_interrupt_stats.min_interrupt_time = elapsed;
    }
    
    g_interrupt_stats.last_interrupt_time = end_time;
    g_interrupt_stats.last_irq = irq;
    
    log_debug("Handled IRQ %d (%s) in %llu cycles", irq, kos_interrupt_get_name(irq), elapsed);
}

// Handle nested interrupt
void kos_interrupt_handle_nested(uint32_t irq) {
    if (!g_interrupt_config.enable_nesting) {
        log_error("Nested interrupts not enabled");
        return;
    }
    
    kos_interrupt_enter_nest(irq);
    
    // Handle the interrupt
    kos_interrupt_handle(irq);
    
    kos_interrupt_exit_nest(irq);
}

// Get interrupt statistics
const kos_interrupt_stats_t* kos_interrupt_get_stats(void) {
    return &g_interrupt_stats;
}

// Reset interrupt statistics
void kos_interrupt_reset_stats(void) {
    kos_memset(&g_interrupt_stats, 0, sizeof(kos_interrupt_stats_t));
    g_interrupt_stats.min_interrupt_time = UINT64_MAX;
    
    // Reset per-IRQ statistics
    for (int i = 0; i < KOS_IRQ_COUNT; i++) {
        kos_interrupt_descriptor_t* desc = &g_interrupt_descriptors[i];
        desc->count = 0;
        desc->total_time = 0;
        desc->last_time = 0;
        desc->max_time = 0;
        desc->min_time = UINT64_MAX;
    }
}

// Dump interrupt statistics
kos_result_t kos_interrupt_dump_stats(void) {
    log_info("=== Interrupt Statistics ===");
    log_info("Total interrupts: %u", g_interrupt_stats.total_interrupts);
    log_info("Nested interrupts: %u", g_interrupt_stats.nested_interrupts);
    log_info("Failed interrupts: %u", g_interrupt_stats.failed_interrupts);
    log_info("Total time: %llu cycles", g_interrupt_stats.total_time);
    log_info("Max time: %llu cycles", g_interrupt_stats.max_interrupt_time);
    log_info("Min time: %llu cycles", g_interrupt_stats.min_interrupt_time);
    
    if (g_interrupt_stats.total_interrupts > 0) {
        log_info("Interrupt counts:");
        for (int i = 0; i < KOS_IRQ_COUNT; i++) {
            if (g_interrupt_stats.interrupt_counts[i] > 0) {
                kos_interrupt_descriptor_t* desc = &g_interrupt_descriptors[i];
                log_info("  %s (%s): %u calls, %llu cycles avg", 
                         kos_interrupt_get_name((kos_irq_t)i), 
                         desc->name,
                         g_interrupt_stats.interrupt_counts[i],
                         desc->count > 0 ? desc->total_time / desc->count : 0);
            }
        }
    }
    
    return KOS_SUCCESS;
}

// Get IRQ name
const char* kos_interrupt_get_name(kos_irq_t irq) {
    if (irq >= KOS_IRQ_COUNT) {
        return "Unknown IRQ";
    }
    return g_irq_names[irq];
}

// Get IRQ description
const char* kos_interrupt_get_description(kos_irq_t irq) {
    if (irq >= KOS_IRQ_COUNT) {
        return "Unknown IRQ";
    }
    return g_irq_descriptions[irq];
}

// Check if IRQ is valid
bool kos_interrupt_is_valid(kos_irq_t irq) {
    return irq < KOS_IRQ_COUNT;
}

// Get vector for IRQ
uint32_t kos_interrupt_get_vector(kos_irq_t irq) {
    if (irq >= KOS_IRQ_COUNT) {
        return 0;
    }
    return g_irq_vectors[irq];
}

// Initialize interrupt stack
kos_result_t kos_interrupt_stack_init(size_t stack_size) {
    if (stack_size == 0) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    g_interrupt_stack.stack_base = kos_memory_alloc(&g_memory_manager, stack_size);
    if (!g_interrupt_stack.stack_base) {
        log_error("Failed to allocate interrupt stack");
        return KOS_ERROR_OUT_OF_MEMORY;
    }
    
    g_interrupt_stack.stack_top = (void*)((uintptr_t)g_interrupt_stack.stack_base + stack_size);
    g_interrupt_stack.stack_size = stack_size;
    g_interrupt_stack.current_sp = g_interrupt_stack.stack_top;
    g_interrupt_stack.nesting_level = 0;
    g_interrupt_stack.in_interrupt = false;
    
    log_info("Interrupt stack initialized: %zu bytes", stack_size);
    return KOS_SUCCESS;
}

// Get interrupt stack
kos_interrupt_stack_t* kos_interrupt_stack_get(void) {
    return &g_interrupt_stack;
}

// Get timestamp (placeholder)
uint64_t kos_get_timestamp(void) {
    uint64_t timestamp;
    asm volatile("rdtsc" : "=A" (timestamp));
    return timestamp;
}

// Dump interrupt descriptors
void kos_interrupt_dump_descriptors(void) {
    log_info("=== Interrupt Descriptors ===");
    for (int i = 0; i < KOS_IRQ_COUNT; i++) {
        kos_interrupt_descriptor_t* desc = &g_interrupt_descriptors[i];
        if (desc->handler) {
            log_info("IRQ %d (%s): %s - %s (vector %d, priority %d, count %u)",
                     i, kos_interrupt_get_name((kos_irq_t)i), desc->name,
                     desc->enabled ? "ENABLED" : "DISABLED",
                     desc->vector, desc->priority, desc->count);
        }
    }
}
