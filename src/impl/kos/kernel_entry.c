#include <stdint.h>
#include "hal/hal_core.h"
#include "hal/hal_interface_clean.h"
#include "kos/config.h"
#include "kos/utils/log_stubs.h"
#include "kos/drivers/driver_framework.h"
#include "kos/drivers/vga.h"
#include "kos/process/process.h"
#include "kos/system/system_status.h"

// =============================================================================
// KOS - Ultra Robust Kernel Entry Point
// =============================================================================

// Kernel state management
typedef enum {
    KOS_KERNEL_STATE_UNINITIALIZED = 0,
    KOS_KERNEL_STATE_INITIALIZING,
    KOS_KERNEL_STATE_RUNNING,
    KOS_KERNEL_STATE_PANIC,
    KOS_KERNEL_STATE_SHUTTING_DOWN
} kos_kernel_state_t;

static volatile kos_kernel_state_t g_kernel_state = KOS_KERNEL_STATE_UNINITIALIZED;
static volatile uint64_t g_kernel_uptime = 0;

// =============================================================================
// Kernel Core Functions
// =============================================================================

static void kernel_panic(const char* message) {
    g_kernel_state = KOS_KERNEL_STATE_PANIC;
    
    // Disable interrupts
    hal_interrupt_disable();
    
    // Log panic message
    log_critical("KERNEL PANIC: %s", message);
    
    // Infinite loop with CPU halt
    while (1) {
        hal_cpu_halt();
    }
}

static hal_result_t kernel_init_subsystems(void) {
    log_info("Initializing KOS kernel subsystems");
    
    // Initialize system status display first
    kos_system_status_init();
    kos_system_status_show_header();
    
    // Phase 1: HAL Initialization
    kos_system_status_show_boot_phase(KOS_BOOT_PHASE_HAL, "Initializing Hardware Abstraction Layer");
    hal_result_t result = hal_init();
    if (result != HAL_SUCCESS) {
        kos_system_status_show_component("HAL", KOS_STATUS_LEVEL_ERROR, "Initialization failed");
        kernel_panic("Failed to initialize HAL");
        return result;
    }
    kos_system_status_show_component("HAL", KOS_STATUS_LEVEL_SUCCESS, "Core initialized");
    kos_system_status_show_hal_info();
    
    // Phase 2: Process Manager Initialization
    kos_system_status_show_boot_phase(KOS_BOOT_PHASE_PROCESS, "Initializing Process Management");
    result = kos_process_init(NULL);
    if (result != HAL_SUCCESS) {
        kos_system_status_show_component("Process Manager", KOS_STATUS_LEVEL_ERROR, "Initialization failed");
        kernel_panic("Failed to initialize process manager");
        return result;
    }
    kos_system_status_show_component("Process Manager", KOS_STATUS_LEVEL_SUCCESS, "Ready");
    kos_system_status_show_process_info();
    
    // Phase 3: Driver Framework Initialization
    kos_system_status_show_boot_phase(KOS_BOOT_PHASE_DRIVERS, "Initializing Driver Framework");
    result = kos_driver_manager_init();
    if (result != HAL_SUCCESS) {
        kos_system_status_show_component("Driver Framework", KOS_STATUS_LEVEL_ERROR, "Initialization failed");
        kernel_panic("Failed to initialize driver manager");
        return result;
    }
    kos_system_status_show_component("Driver Framework", KOS_STATUS_LEVEL_SUCCESS, "Manager initialized");
    
    // Phase 4: VGA Driver Initialization
    kos_system_status_show_boot_phase(KOS_BOOT_PHASE_DRIVERS, "Initializing VGA Display Driver");
    result = vga_init();
    if (result != HAL_SUCCESS) {
        kos_system_status_show_component("VGA Driver", KOS_STATUS_LEVEL_ERROR, "Initialization failed");
        kernel_panic("Failed to initialize VGA driver");
        return result;
    }
    kos_system_status_show_component("VGA Driver", KOS_STATUS_LEVEL_SUCCESS, "Display ready (80x25 text mode)");
    kos_system_status_show_driver_info();
    
    // Show system information
    kos_system_status_show_system_info();
    kos_system_status_show_memory_info();
    kos_system_status_show_footer();
    
    log_info("All kernel subsystems initialized successfully");
    return HAL_SUCCESS;
}

static void kernel_main_loop(void) {
    log_info("Entering kernel main loop");
    
    // Enable process scheduling
    // TODO: Enable scheduler when fully implemented
    
    while (g_kernel_state == KOS_KERNEL_STATE_RUNNING) {
        // Update kernel uptime
        g_kernel_uptime++;
        
        // Process scheduling
        // TODO: Add scheduler tick handling
        // kos_process_schedule();
        
        // Process pending tasks
        // TODO: Add task scheduler calls here
        
        // Handle system events
        // TODO: Add event handling here
        
        // Power management - halt CPU until next interrupt
        hal_cpu_halt();
    }
}

// =============================================================================
// Public Kernel Entry Point
// =============================================================================

void kernel_main(void) {
    log_info("KOS Kernel v%s starting", KOS_VERSION_STRING);
    
    // Set kernel state
    g_kernel_state = KOS_KERNEL_STATE_INITIALIZING;
    
    // Initialize all subsystems
    hal_result_t result = kernel_init_subsystems();
    if (result != HAL_SUCCESS) {
        kernel_panic("Kernel initialization failed");
        return;
    }
    
    // Set kernel to running state
    g_kernel_state = KOS_KERNEL_STATE_RUNNING;
    
    // Display startup message
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("=== KOS Kernel Started Successfully ===\n");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("Hardware Abstraction Layer: ACTIVE\n");
    vga_write_string("Driver Framework: ACTIVE\n");
    vga_write_string("Process Management: ACTIVE\n");
    vga_write_string("Ready for system operations\n\n");
    
    log_info("KOS kernel started successfully");
    
    // Enter main kernel loop
    kernel_main_loop();
    
    // Should never reach here
    kernel_panic("Kernel main loop exited unexpectedly");
}

// =============================================================================
// Interrupt Handlers
// =============================================================================

void irq_handler(uint64_t irq_number) {
    // Validate IRQ number
    if (irq_number >= 256) {
        log_error("Invalid IRQ number: %llu", irq_number);
        return;
    }
    
    // Log IRQ for debugging
    log_debug("IRQ %llu received", irq_number);
    
    // TODO: Implement proper IRQ dispatching
    // This will be enhanced when we implement the interrupt framework
    
    // Acknowledge IRQ (platform-specific)
    // TODO: Add platform-specific IRQ acknowledgment
}

void syscall_handler(uint64_t syscall_number, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    // Validate syscall number
    if (syscall_number >= 1024) {
        log_error("Invalid syscall number: %llu", syscall_number);
        return;
    }
    
    // Log syscall for debugging
    log_debug("Syscall %llu called with args: %llu, %llu, %llu", 
               syscall_number, arg1, arg2, arg3);
    
    // TODO: Implement proper syscall dispatching
    // This will be enhanced when we implement the system call interface
    
    // For now, just return without doing anything
}

// =============================================================================
// Kernel Utility Functions
// =============================================================================

uint64_t kernel_get_uptime(void) {
    return g_kernel_uptime;
}

kos_kernel_state_t kernel_get_state(void) {
    return g_kernel_state;
}

void kernel_shutdown(void) {
    log_info("Shutting down KOS kernel");
    
    g_kernel_state = KOS_KERNEL_STATE_SHUTTING_DOWN;
    
    // Shutdown driver framework
    kos_driver_manager_shutdown();
    
    // Shutdown HAL
    hal_shutdown();
    
    log_info("KOS kernel shutdown completed");
    
    // Halt CPU
    while (1) {
        hal_cpu_halt();
    }
}
