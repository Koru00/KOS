#include "hal/hal_interface.h"
#include "hal/hal_types.h"
#include "hal/hal_interrupt.h"
#include "kos/utils/string.h"
#include "debug/debug.h"

// =============================================================================
// KOS - HAL Interrupt Operations x86_64 Implementation
// =============================================================================

// Global x86_64 interrupt state
static hal_interrupt_info_t g_interrupt_info = {0};
static bool g_interrupt_initialized = false;

// x86_64 interrupt operations implementation
static hal_result_t hal_x86_64_interrupt_init(void);
static hal_result_t hal_x86_64_interrupt_shutdown(void);
static hal_result_t hal_x86_64_interrupt_reset(void);
static hal_result_t hal_x86_64_interrupt_cleanup(void);
static hal_result_t hal_x86_64_interrupt_get_info(hal_interrupt_info_t* info);
static hal_u32_t hal_x86_64_interrupt_get_total_interrupts(void);
static hal_u32_t hal_x86_64_interrupt_get_available_interrupts(void);
static hal_u32_t hal_x86_64_interrupt_get_max_priority(void);
static hal_result_t hal_x86_64_interrupt_enable_interrupts(void);
static hal_result_t hal_x86_64_interrupt_disable_interrupts(void);
static hal_u32_t hal_x86_64_interrupt_get_interrupt_state(void);

// x86_64 interrupt operations structure
static hal_interrupt_ops_t hal_x86_64_interrupt_ops = {
    .init = hal_x86_64_interrupt_init,
    .shutdown = hal_x86_64_interrupt_shutdown,
    .reset = hal_x86_64_interrupt_reset,
    .cleanup = hal_x86_64_interrupt_cleanup,
    .get_info = hal_x86_64_interrupt_get_info,
    .get_total_interrupts = hal_x86_64_interrupt_get_total_interrupts,
    .get_available_interrupts = hal_x86_64_interrupt_get_available_interrupts,
    .get_max_priority = hal_x86_64_interrupt_get_max_priority,
    .enable_interrupts = hal_x86_64_interrupt_enable_interrupts,
    .disable_interrupts = hal_x86_64_interrupt_disable_interrupts,
    .get_interrupt_state = hal_x86_64_interrupt_get_interrupt_state
};

// Interrupt initialization
static hal_result_t hal_x86_64_interrupt_init(void) {
    if (g_interrupt_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    log_info("Initializing x86_64 interrupt operations");
    
    // Initialize interrupt information
    kos_memset(&g_interrupt_info, 0, sizeof(hal_interrupt_info_t));
    g_interrupt_info.total_interrupts = 256;
    g_interrupt_info.available_interrupts = 224; // 0-31 exceptions, 32-255 IRQs
    g_interrupt_info.max_interrupt_priority = 15;
    g_interrupt_info.has_apic = true;
    g_interrupt_info.has_pic = true;
    g_interrupt_info.has_msi = true;
    g_interrupt_info.has_msix = true;
    
    g_interrupt_initialized = true;
    
    log_info("x86_64 interrupt operations initialized");
    log_info("Total Interrupts: %u", g_interrupt_info.total_interrupts);
    log_info("Available Interrupts: %u", g_interrupt_info.available_interrupts);
    log_info("APIC: %s", g_interrupt_info.has_apic ? "Yes" : "No");
    log_info("PIC: %s", g_interrupt_info.has_pic ? "Yes" : "No");
    
    return HAL_SUCCESS;
}

// Interrupt shutdown
static hal_result_t hal_x86_64_interrupt_shutdown(void) {
    if (!g_interrupt_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    g_interrupt_initialized = false;
    log_info("x86_64 interrupt operations shutdown");
    return HAL_SUCCESS;
}

// Interrupt reset
static hal_result_t hal_x86_64_interrupt_reset(void) {
    if (!g_interrupt_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    hal_x86_64_interrupt_shutdown();
    return hal_x86_64_interrupt_init();
}

// Interrupt cleanup
static hal_result_t hal_x86_64_interrupt_cleanup(void) {
    return hal_x86_64_interrupt_shutdown();
}

// Get interrupt information
static hal_result_t hal_x86_64_interrupt_get_info(hal_interrupt_info_t* info) {
    if (!g_interrupt_initialized || !info) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *info = g_interrupt_info;
    return HAL_SUCCESS;
}

// Get total interrupts
static hal_u32_t hal_x86_64_interrupt_get_total_interrupts(void) {
    if (!g_interrupt_initialized) {
        return 0;
    }
    
    return g_interrupt_info.total_interrupts;
}

// Get available interrupts
static hal_u32_t hal_x86_64_interrupt_get_available_interrupts(void) {
    if (!g_interrupt_initialized) {
        return 0;
    }
    
    return g_interrupt_info.available_interrupts;
}

// Get max priority
static hal_u32_t hal_x86_64_interrupt_get_max_priority(void) {
    if (!g_interrupt_initialized) {
        return 0;
    }
    
    return g_interrupt_info.max_interrupt_priority;
}

// Enable interrupts
static hal_result_t hal_x86_64_interrupt_enable_interrupts(void) {
    if (!g_interrupt_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    asm volatile("sti");
    return HAL_SUCCESS;
}

// Disable interrupts
static hal_result_t hal_x86_64_interrupt_disable_interrupts(void) {
    if (!g_interrupt_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    asm volatile("cli");
    return HAL_SUCCESS;
}

// Get interrupt state
static hal_u32_t hal_x86_64_interrupt_get_interrupt_state(void) {
    if (!g_interrupt_initialized) {
        return 0;
    }
    
    hal_u64_t flags;
    asm volatile("pushf; pop %0" : "=r" (flags));
    return (flags & (1 << 9)) ? 1 : 0; // IF flag is bit 9
}

// Get x86_64 interrupt interface
hal_interface_t* hal_x86_64_get_interrupt_interface(void) {
    static hal_interface_t interface = {0};
    
    interface.arch_name = "x86_64";
    interface.arch_bits = 64;
    interface.architecture = HAL_ARCH_X86_64;
    interface.interrupt_ops = &hal_x86_64_interrupt_ops;
    
    return &interface;
}
