#include "kos/process/process.h"
#include "kos/utils/log_stubs.h"

// =============================================================================
// KOS - Context Switching Implementation (C version for simplicity)
// =============================================================================

// External assembly functions (stubs for now)
void kos_context_switch(kos_cpu_context_t* old_context, kos_cpu_context_t* new_context) {
    // Simple stub implementation
    log_debug("Context switch from %p to %p", old_context, new_context);
    
    // For now, just copy the context
    if (old_context && new_context) {
        // This would normally call assembly code
        // For now, just log the switch
        (void)old_context;
        (void)new_context;
    }
}

void kos_context_switch_first(kos_cpu_context_t* new_context) {
    // Simple stub implementation
    log_debug("First context switch to %p", new_context);
    
    // For now, just log the switch
    (void)new_context;
}

void kos_timer_scheduler(void) {
    // Simple stub implementation
    log_debug("Timer scheduler called");
    
    // Call the scheduler
    kos_process_schedule();
}

uintptr_t kos_get_stack_pointer(void) {
    // Simple stub implementation
    uintptr_t sp;
    __asm__ volatile("mov %%rsp, %0" : "=r"(sp));
    return sp;
}
