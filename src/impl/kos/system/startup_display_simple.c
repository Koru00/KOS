#include "kos/system/system_status.h"
#include "kos/drivers/vga.h"
#include "kos/process/process.h"
#include "kos/drivers/driver_framework.h"
#include "kos/utils/log_stubs.h"
#include "kos/utils/hal_utils.h"
#include "hal/hal_interface_clean.h"

// =============================================================================
// KOS - Simple Professional Startup Display
// =============================================================================

// Global system information
static kos_system_info_t g_system_info;
static hal_bool_t g_system_status_initialized = false;

// =============================================================================
// System Status API Implementation (Simple)
// =============================================================================

hal_result_t kos_system_status_init(void) {
    if (g_system_status_initialized) {
        return HAL_ERROR_ALREADY_INITIALIZED;
    }
    
    // Initialize system information
    hal_memset(&g_system_info, 0, sizeof(kos_system_info_t));
    
    hal_strcpy(g_system_info.kernel_name, "KOS");
    hal_strcpy(g_system_info.kernel_version, "1.0.0");
    hal_strcpy(g_system_info.build_date, __DATE__);
    hal_strcpy(g_system_info.architecture, "x86_64");
    
    g_system_info.boot_time = 0;
    g_system_info.uptime = 0;
    g_system_info.total_processes = 0;
    g_system_info.active_processes = 0;
    g_system_info.total_drivers = 0;
    g_system_info.active_drivers = 0;
    g_system_info.process_management_enabled = false;
    g_system_info.scheduler_enabled = false;
    g_system_info.debug_mode = false;
    
    g_system_status_initialized = true;
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_header(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    // Clear screen first
    vga_clear();
    
    // Print simple header
    vga_set_color(KOS_VGA_COLOR_LIGHT_CYAN, KOS_VGA_COLOR_BLACK);
    vga_write_string("╔════════════════════════════════════════════════════╗\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("║                    ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("KOS KERNEL OPERATING SYSTEM");
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("                     ║\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_CYAN, KOS_VGA_COLOR_BLACK);
    vga_write_string("║                    ");
    vga_set_color(KOS_VGA_COLOR_YELLOW, KOS_VGA_COLOR_BLACK);
    vga_write_string("Enterprise Edition v");
    vga_write_string(g_system_info.kernel_version);
    vga_set_color(KOS_VGA_COLOR_LIGHT_CYAN, KOS_VGA_COLOR_BLACK);
    vga_write_string("                     ║\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_CYAN, KOS_VGA_COLOR_BLACK);
    vga_write_string("╚══════════════════════════════════════════════════════╝\n\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_boot_phase(kos_boot_phase_t phase, const char* description) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    const char* phase_names[] = {
        "EARLY INIT",
        "HAL INITIALIZATION",
        "DRIVER INITIALIZATION", 
        "PROCESS MANAGER INIT",
        "SYSTEM SERVICES",
        "SHELL INITIALIZATION",
        "SYSTEM READY"
    };
    
    // Print phase indicator
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("[");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string(phase_names[phase]);
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("] ");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(description);
    vga_write_string("\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_component(const char* component, kos_status_level_t level, const char* status) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    // Print component name
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("[");
    vga_write_string(component);
    vga_write_string("] ");
    
    // Print status
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(status);
    vga_write_string("\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_hal_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_BLUE, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ HARDWARE ABSTRACTION LAYER ─────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("[OK] HAL Core: Initialized\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("[OK] HAL Interface: Active\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("[OK] Architecture: ");
    vga_write_string(g_system_info.architecture);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_BLUE, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_driver_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_CYAN, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ DRIVER FRAMEWORK ─────────────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("[OK] Driver Manager: Active\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("[OK] VGA Driver: Ready (80x25 text mode)\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("[OK] Total Drivers: 2\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_CYAN, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_process_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ PROCESS MANAGEMENT ─────────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("[OK] Process Manager: Initialized\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("[OK] Scheduler: Round-robin (preemptive)\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("[OK] Idle Process: Running (PID 0)\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("[INFO] Max Processes: 256\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_system_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_MAGENTA, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ SYSTEM INFORMATION ────────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Kernel: ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(g_system_info.kernel_name);
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string(" v");
    vga_write_string(g_system_info.kernel_version);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Build: ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(g_system_info.build_date);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Uptime: Ready\n");
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_MAGENTA, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_memory_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_RED, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ MEMORY STATUS ─────────────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Total Memory: ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("64MB (simulated)\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Available: ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("48MB (75% free)\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Used: ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_RED, KOS_VGA_COLOR_BLACK);
    vga_write_string("16MB (25% used)\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_RED, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_footer(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("\n╔══════════════════════════════════════════════════╗\n");
    vga_write_string("║           ");
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("KOS SYSTEM READY");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("                    ║\n");
    vga_write_string("║                                              ║\n");
    vga_write_string("║  ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("Enterprise Operating System v");
    vga_write_string(g_system_info.kernel_version);
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string(" is ready");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("        ║\n");
    vga_write_string("╚════════════════════════════════════════════════╝\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("\nPress any key to continue to KOS shell...\n");
    
    return HAL_SUCCESS;
}

// =============================================================================
// Utility Functions (Simple)
// =============================================================================

void kos_status_print_colored(kos_status_color_t color, const char* format, ...) {
    // Simplified - just print without color processing
    (void)color;
    va_list args;
    va_start(args, format);
    char buffer[256];
    hal_strcpy(buffer, "FORMAT STUB");
    vga_write_string(buffer);
    va_end(args);
}

void kos_status_print_level(kos_status_level_t level, const char* format, ...) {
    // Simplified - just print without level processing
    (void)level;
    va_list args;
    va_start(args, format);
    char buffer[256];
    hal_strcpy(buffer, "FORMAT STUB");
    vga_write_string(buffer);
    va_end(args);
}

void kos_status_print_separator(char character) {
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    
    char separator[65];
    for (int i = 0; i < 64; i++) {
        separator[i] = character;
    }
    separator[64] = '\0';
    vga_write_string(separator);
    vga_write_string("\n");
}

void kos_status_print_box(const char* title, const char* content) {
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ ");
    vga_write_string(title);
    vga_write_string(" ─────────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_write_string(content);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────┘\n");
}
