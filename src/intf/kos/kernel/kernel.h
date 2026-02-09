#pragma once

#include "../types.h"
#include "../config.h"
#include "../memory/memory.h"
#include "../cpu/gdt.h"
#include "../interrupts/interrupts.h"
#include "../drivers/driver.h"
#include "../debug/logging.h"
#include <stddef.h>

// =============================================================================
// KOS - Kernel Core Interface
// =============================================================================

// Kernel initialization phases
typedef enum {
    KOS_KERNEL_PHASE_EARLY = 0,      // GDT, basic CPU setup
    KOS_KERNEL_PHASE_INTERRUPTS = 1,  // IDT, PIC, IRQ
    KOS_KERNEL_PHASE_DRIVERS = 2,     // VGA, keyboard, timer
    KOS_KERNEL_PHASE_MEMORY = 3,      // Heap, memory management
    KOS_KERNEL_PHASE_SERVICES = 4,    // Shell, filesystem
    KOS_KERNEL_PHASE_READY = 5       // System ready
} kos_kernel_phase_t;

// Kernel state
typedef struct kos_kernel_state {
    kos_kernel_phase_t current_phase;
    u64 start_timestamp;
    u64 uptime_ticks;
    u32 boot_flags;
    
    // Core subsystems
    kos_gdt_t gdt;
    memory_manager_t memory_manager;
    kos_driver_manager_t driver_manager;
    kos_logger_t logger;
    
    // Statistics
    u32 interrupts_handled;
    u32 system_calls;
    u32 context_switches;
} kos_kernel_state_t;

// Kernel configuration
typedef struct kos_kernel_config {
    b8 enable_serial_logging;
    b8 enable_vga_logging;
    b8 enable_keyboard;
    b8 enable_timer;
    kos_log_level_t min_log_level;
} kos_kernel_config_t;

// Kernel lifecycle functions
kos_result_t kos_kernel_init(const kos_kernel_config_t* config);
kos_result_t kos_kernel_start(void);
kos_result_t kos_kernel_shutdown(void);
kos_result_t kos_kernel_halt(void);

// Phase-specific initialization
kos_result_t kos_kernel_init_early_setup(kos_kernel_state_t* state);
kos_result_t kos_kernel_init_interrupts(kos_kernel_state_t* state);
kos_result_t kos_kernel_init_drivers(kos_kernel_state_t* state);
kos_result_t kos_kernel_init_memory(kos_kernel_state_t* state);
kos_result_t kos_kernel_init_services(kos_kernel_state_t* state);

// Kernel information functions
const char* kos_kernel_get_version(void);
kos_kernel_phase_t kos_kernel_get_phase(void);
u64 kos_kernel_get_uptime(void);
kos_kernel_state_t* kos_kernel_get_state(void);

// Kernel utilities
void kos_kernel_panic(const char* message);
void kos_kernel_assert(b8 condition, const char* message);
void kos_kernel_main_loop(void);

// Global kernel state
extern kos_kernel_state_t g_kernel_state;

// Default kernel configuration
extern const kos_kernel_config_t g_default_kernel_config;
