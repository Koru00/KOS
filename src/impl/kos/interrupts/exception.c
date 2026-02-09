#include "kos/interrupts/exception.h"
#include "kos/utils/string.h"
#include "kos/memory/memory.h"
#include "debug/debug.h"

// =============================================================================
// KOS - Exception Handling Implementation
// =============================================================================

// Forward declarations
extern memory_manager_t g_memory_manager;
extern uint64_t kos_get_timestamp(void);

// Global exception state
static kos_exception_config_t g_exception_config = {0};
static kos_exception_stats_t g_exception_stats = {0};
static kos_exception_handler_t g_exception_handlers[KOS_EXCEPTION_COUNT] = {NULL};
static bool g_exception_initialized = false;
static void* g_exception_stack = NULL;
static size_t g_exception_stack_size = 0;

// Exception names and descriptions
static const char* g_exception_names[KOS_EXCEPTION_COUNT] = {
    "Divide by Zero",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Reserved",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 FPU Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

static const char* g_exception_descriptions[KOS_EXCEPTION_COUNT] = {
    "Division by zero error",
    "Debug exception",
    "Non-maskable interrupt",
    "Breakpoint exception",
    "Overflow exception",
    "Bound range exceeded",
    "Invalid opcode encountered",
    "Floating-point unit not available",
    "Double fault occurred",
    "Reserved exception",
    "Invalid TSS encountered",
    "Segment not present",
    "Stack segment fault",
    "General protection fault",
    "Page fault occurred",
    "Reserved exception",
    "x87 FPU error",
    "Alignment check exception",
    "Machine check exception",
    "SIMD floating-point exception",
    "Virtualization exception",
    "Reserved exception",
    "Reserved exception",
    "Reserved exception",
    "Reserved exception",
    "Reserved exception",
    "Reserved exception",
    "Reserved exception",
    "Reserved exception",
    "Reserved exception",
    "Security exception",
    "Reserved exception"
};

// Exception initialization
kos_result_t kos_exception_init(const kos_exception_config_t* config) {
    if (!config) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    // Copy configuration
    g_exception_config = *config;
    
    // Initialize statistics
    kos_memset(&g_exception_stats, 0, sizeof(kos_exception_stats_t));
    
    // Initialize exception stack
    if (config->max_recovery_attempts > 0) {
        g_exception_stack_size = config->max_recovery_attempts * 4096; // 4KB per attempt
        g_exception_stack = kos_memory_alloc(&g_memory_manager, g_exception_stack_size);
        if (!g_exception_stack) {
            log_error("Failed to allocate exception stack");
            return KOS_ERROR_OUT_OF_MEMORY;
        }
    }
    
    // Clear all handlers
    for (int i = 0; i < KOS_EXCEPTION_COUNT; i++) {
        g_exception_handlers[i] = NULL;
    }
    
    g_exception_initialized = true;
    
    log_info("Exception handling initialized");
    return KOS_SUCCESS;
}

// Register exception handler
kos_result_t kos_exception_register_handler(kos_exception_t exception, kos_exception_handler_t handler) {
    if (!g_exception_initialized) {
        log_error("Exception system not initialized");
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (exception >= KOS_EXCEPTION_COUNT) {
        log_error("Invalid exception number: %d", exception);
        return KOS_ERROR_INVALID_PARAM;
    }
    
    g_exception_handlers[exception] = handler;
    
    log_debug("Registered handler for exception %d (%s)", exception, kos_exception_get_name(exception));
    return KOS_SUCCESS;
}

// Unregister exception handler
kos_result_t kos_exception_unregister_handler(kos_exception_t exception) {
    if (!g_exception_initialized) {
        log_error("Exception system not initialized");
        return KOS_ERROR_INVALID_STATE;
    }
    
    if (exception >= KOS_EXCEPTION_COUNT) {
        log_error("Invalid exception number: %d", exception);
        return KOS_ERROR_INVALID_PARAM;
    }
    
    g_exception_handlers[exception] = NULL;
    
    log_debug("Unregistered handler for exception %d (%s)", exception, kos_exception_get_name(exception));
    return KOS_SUCCESS;
}

// Main exception handler
void kos_exception_handle(const kos_exception_context_t* context) {
    if (!context) {
        log_error("Null exception context");
        return;
    }
    
    kos_exception_t exception = (kos_exception_t)context->vector_number;
    
    if (exception >= KOS_EXCEPTION_COUNT) {
        log_error("Invalid exception vector: %d", exception);
        return;
    }
    
    // Update statistics
    g_exception_stats.exception_counts[exception]++;
    g_exception_stats.total_exceptions++;
    g_exception_stats.last_exception_time = kos_get_timestamp();
    g_exception_stats.last_exception_type = exception;
    
    // Log the exception
    if (g_exception_config.enable_logging) {
        kos_exception_log(exception, context, "Exception occurred");
    }
    
    // Call registered handler if available
    if (g_exception_handlers[exception]) {
        g_exception_handlers[exception](context);
        return;
    }
    
    // Try to recover if enabled
    if (g_exception_config.enable_recovery) {
        kos_result_t result = kos_exception_try_recover(exception, context);
        if (result == KOS_SUCCESS) {
            g_exception_stats.successful_recoveries++;
            log_info("Successfully recovered from exception %d", exception);
            return;
        } else {
            g_exception_stats.failed_recoveries++;
            log_error("Failed to recover from exception %d", exception);
        }
    }
    
    // If unrecoverable, panic
    if (kos_exception_is_fatal(exception)) {
        kos_exception_panic(exception, context, "Fatal exception occurred");
    }
}

// Try to recover from exception
kos_result_t kos_exception_try_recover(kos_exception_t exception, const kos_exception_context_t* context) {
    if (!context) {
        return KOS_ERROR_INVALID_PARAM;
    }
    
    g_exception_stats.recovery_attempts++;
    
    switch (exception) {
        case KOS_EXCEPTION_DIVIDE_BY_ZERO:
            return kos_exception_recover_divide_by_zero(context);
            
        case KOS_EXCEPTION_PAGE_FAULT:
            return kos_exception_recover_page_fault(context);
            
        case KOS_EXCEPTION_GENERAL_PROTECTION_FAULT:
            return kos_exception_recover_general_protection_fault(context);
            
        case KOS_EXCEPTION_STACK_SEGMENT_FAULT:
            return kos_exception_recover_stack_fault(context);
            
        default:
            log_error("No recovery handler for exception %d", exception);
            return KOS_ERROR_NOT_IMPLEMENTED;
    }
}

// Panic handler
void kos_exception_panic(kos_exception_t exception, const kos_exception_context_t* context, const char* message) {
    // Disable interrupts
    asm volatile("cli");
    
    // Log panic information
    log_critical("=== KERNEL PANIC ===");
    log_critical("Exception: %s", kos_exception_get_name(exception));
    log_critical("Message: %s", message ? message : "No message");
    log_critical("RIP: 0x%016x", context->rip);
    log_critical("RSP: 0x%016x", context->rsp);
    log_critical("RAX: 0x%016x", context->rax);
    log_critical("RBX: 0x%016x", context->rbx);
    log_critical("RCX: 0x%016x", context->rcx);
    log_critical("RDX: 0x%016x", context->rdx);
    log_critical("Error Code: 0x%016x", context->error_code);
    
    // Dump stack trace if enabled
    if (g_exception_config.enable_stack_trace) {
        kos_exception_dump_stack_trace(context);
    }
    
    // Halt the system
    while (true) {
        asm volatile("hlt");
    }
}

// Get exception name
const char* kos_exception_get_name(kos_exception_t exception) {
    if (exception >= KOS_EXCEPTION_COUNT) {
        return "Unknown Exception";
    }
    return g_exception_names[exception];
}

// Get exception description
const char* kos_exception_get_description(kos_exception_t exception) {
    if (exception >= KOS_EXCEPTION_COUNT) {
        return "Unknown exception";
    }
    return g_exception_descriptions[exception];
}

// Check if exception has error code
bool kos_exception_has_error_code(kos_exception_t exception) {
    switch (exception) {
        case KOS_EXCEPTION_DIVIDE_BY_ZERO:
        case KOS_EXCEPTION_DEBUG:
        case KOS_EXCEPTION_NMI:
        case KOS_EXCEPTION_BREAKPOINT:
        case KOS_EXCEPTION_OVERFLOW:
        case KOS_EXCEPTION_BOUND_RANGE:
        case KOS_EXCEPTION_INVALID_OPCODE:
        case KOS_EXCEPTION_DEVICE_NOT_AVAILABLE:
        case KOS_EXCEPTION_X87_FPU_ERROR:
        case KOS_EXCEPTION_ALIGNMENT_CHECK:
        case KOS_EXCEPTION_MACHINE_CHECK:
        case KOS_EXCEPTION_SIMD_FP_EXCEPTION:
        case KOS_EXCEPTION_VIRTUALIZATION:
        case KOS_EXCEPTION_SECURITY:
            return false;
            
        case KOS_EXCEPTION_DOUBLE_FAULT:
        case KOS_EXCEPTION_INVALID_TSS:
        case KOS_EXCEPTION_SEGMENT_NOT_PRESENT:
        case KOS_EXCEPTION_STACK_SEGMENT_FAULT:
        case KOS_EXCEPTION_GENERAL_PROTECTION_FAULT:
        case KOS_EXCEPTION_PAGE_FAULT:
            return true;
            
        default:
            return false;
    }
}

// Get exception statistics
const kos_exception_stats_t* kos_exception_get_stats(void) {
    return &g_exception_stats;
}

// Reset exception statistics
void kos_exception_reset_stats(void) {
    kos_memset(&g_exception_stats, 0, sizeof(kos_exception_stats_t));
}

// Dump exception statistics
kos_result_t kos_exception_dump_stats(void) {
    log_info("=== Exception Statistics ===");
    log_info("Total exceptions: %u", g_exception_stats.total_exceptions);
    log_info("Recovery attempts: %u", g_exception_stats.recovery_attempts);
    log_info("Successful recoveries: %u", g_exception_stats.successful_recoveries);
    log_info("Failed recoveries: %u", g_exception_stats.failed_recoveries);
    
    if (g_exception_stats.total_exceptions > 0) {
        log_info("Exception counts:");
        for (int i = 0; i < KOS_EXCEPTION_COUNT; i++) {
            if (g_exception_stats.exception_counts[i] > 0) {
                log_info("  %s: %u", kos_exception_get_name((kos_exception_t)i), g_exception_stats.exception_counts[i]);
            }
        }
    }
    
    return KOS_SUCCESS;
}

// Log exception
void kos_exception_log(kos_exception_t exception, const kos_exception_context_t* context, const char* message) {
    log_error("Exception %s: %s", kos_exception_get_name(exception), message ? message : "No message");
    log_error("  RIP: 0x%016x, RSP: 0x%016x, RAX: 0x%016x", context->rip, context->rsp, context->rax);
    
    if (kos_exception_has_error_code(exception)) {
        log_error("  Error Code: 0x%016x", context->error_code);
    }
}

// Check if exception is fatal
bool kos_exception_is_fatal(kos_exception_t exception) {
    switch (exception) {
        case KOS_EXCEPTION_DOUBLE_FAULT:
        case KOS_EXCEPTION_MACHINE_CHECK:
        case KOS_EXCEPTION_NMI:
            return true;
            
        default:
            return false;
    }
}

// Check if exception is recoverable
bool kos_exception_is_recoverable(kos_exception_t exception) {
    switch (exception) {
        case KOS_EXCEPTION_DIVIDE_BY_ZERO:
        case KOS_EXCEPTION_PAGE_FAULT:
        case KOS_EXCEPTION_GENERAL_PROTECTION_FAULT:
        case KOS_EXCEPTION_STACK_SEGMENT_FAULT:
            return true;
            
        default:
            return false;
    }
}

// Recovery handlers
kos_result_t kos_exception_recover_divide_by_zero(const kos_exception_context_t* context) {
    log_warn("Attempting to recover from divide by zero at RIP: 0x%016x", context->rip);
    
    // For now, we can't really recover from divide by zero
    // In a real implementation, we might patch the instruction or skip it
    return KOS_ERROR_NOT_IMPLEMENTED;
}

kos_result_t kos_exception_recover_page_fault(const kos_exception_context_t* context) {
    log_warn("Attempting to recover from page fault at RIP: 0x%016x", context->rip);
    
    // Check if it's a valid address that we can map
    uint64_t fault_address;
    asm volatile("mov %%cr2, %0" : "=r" (fault_address));
    
    log_info("Page fault address: 0x%016x", fault_address);
    
    // For now, we can't handle page faults
    // In a real implementation, we would check if the address is valid and map it
    return KOS_ERROR_NOT_IMPLEMENTED;
}

kos_result_t kos_exception_recover_general_protection_fault(const kos_exception_context_t* context) {
    log_warn("Attempting to recover from GPF at RIP: 0x%016x", context->rip);
    log_info("GPF error code: 0x%016x", context->error_code);
    
    // For now, we can't handle GPF
    // In a real implementation, we would check the error code and try to fix the issue
    return KOS_ERROR_NOT_IMPLEMENTED;
}

kos_result_t kos_exception_recover_stack_fault(const kos_exception_context_t* context) {
    log_warn("Attempting to recover from stack fault at RIP: 0x%016x", context->rip);
    
    // For now, we can't handle stack faults
    // In a real implementation, we would try to extend the stack or fix the issue
    return KOS_ERROR_NOT_IMPLEMENTED;
}

// Dump exception context
void kos_exception_dump_context(const kos_exception_context_t* context) {
    if (!context) return;
    
    log_info("=== Exception Context ===");
    log_info("Vector: %u", context->vector_number);
    log_info("Error Code: 0x%016x", context->error_code);
    log_info("RIP: 0x%016x", context->rip);
    log_info("RSP: 0x%016x", context->rsp);
    log_info("RFLAGS: 0x%016x", context->rflags);
    log_info("CS: 0x%016x", context->cs);
    log_info("SS: 0x%016x", context->ss);
    
    log_info("General Purpose Registers:");
    log_info("  RAX: 0x%016x", context->rax);
    log_info("  RBX: 0x%016x", context->rbx);
    log_info("  RCX: 0x%016x", context->rcx);
    log_info("  RDX: 0x%016x", context->rdx);
    log_info("  RSI: 0x%016x", context->rsi);
    log_info("  RDI: 0x%016x", context->rdi);
    log_info("  RBP: 0x%016x", context->rbp);
    
    log_info("Extended Registers:");
    log_info("  R8: 0x%016x", context->r8);
    log_info("  R9: 0x%016x", context->r9);
    log_info("  R10: 0x%016x", context->r10);
    log_info("  R11: 0x%016x", context->r11);
    log_info("  R12: 0x%016x", context->r12);
    log_info("  R13: 0x%016x", context->r13);
    log_info("  R14: 0x%016x", context->r14);
    log_info("  R15: 0x%016x", context->r15);
}

// Dump stack trace (placeholder)
void kos_exception_dump_stack_trace(const kos_exception_context_t* context) {
    if (!context) return;
    
    log_info("=== Stack Trace ===");
    log_info("Stack pointer: 0x%016x", context->rsp);
    
    // In a real implementation, we would walk the stack and print return addresses
    // For now, just print the current stack pointer
    log_info("Stack trace not implemented yet");
}

// Dump registers
void kos_exception_dump_registers(const kos_exception_context_t* context) {
    kos_exception_dump_context(context);
}
