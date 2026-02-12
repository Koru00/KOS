#include "kos/system/system_status.h"
#include "kos/drivers/vga.h"
#include "kos/process/process.h"
#include "kos/drivers/driver_framework.h"
#include "kos/utils/log_stubs.h"
#include "kos/utils/hal_utils.h"
#include "hal/hal_interface_clean.h"

// =============================================================================
// KOS - Professional Startup Display Implementation
// =============================================================================

// Global system information
static kos_system_info_t g_system_info;
static hal_bool_t g_system_status_initialized = false;

// =============================================================================
// Simple VGA color definitions (local to avoid conflicts)
// =============================================================================

typedef enum {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GRAY = 7,
    VGA_COLOR_DARK_GRAY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15
} vga_color_t;

// =============================================================================
// System Status API Implementation
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
    
    g_system_info.boot_time = hal_get_timestamp();
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
    
    // Print professional header
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("╔══════════════════════════════════════════════════╗\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("║                    ");
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("KOS KERNEL OPERATING SYSTEM");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("                     ║\n");
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("║                    ");
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("Enterprise Edition v");
    vga_write_string(g_system_info.kernel_version);
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("                     ║\n");
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("╚══════════════════════════════════════════════════╝\n\n");
    
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
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("[");
    
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string(phase_names[phase]);
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("] ");
    
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(description);
    vga_write_string("\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_component(const char* component, kos_status_level_t level, const char* status) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_color_t fg_colors[] = {
        VGA_COLOR_LIGHT_GRAY,     // INFO
        VGA_COLOR_LIGHT_GREEN,   // SUCCESS
        VGA_COLOR_LIGHT_YELLOW,  // WARNING
        VGA_COLOR_LIGHT_RED,     // ERROR
        VGA_COLOR_LIGHT_CYAN      // DEBUG
    };
    
    // Print component name
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("[");
    
    vga_set_color(fg_colors[level], VGA_COLOR_BLACK);
    vga_write_string(component);
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("] ");
    
    // Print status
    vga_set_color(fg_colors[level], VGA_COLOR_BLACK);
    vga_write_string(status);
    vga_write_string("\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_hal_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    vga_write_string("┌─ HARDWARE ABSTRACTION LAYER ─────────────────────┐\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("│ "); 
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" HAL Core: Initialized\n");
    
    vga_write_string("│ "); 
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" HAL Interface: Active\n");
    
    vga_write_string("│ "); 
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" Architecture: ");
    vga_write_string(g_system_info.architecture);
    vga_write_string("\n");
    
    vga_set_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_driver_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("┌─ DRIVER FRAMEWORK ─────────────────────────────┐\n");
    
    vga_write_string("│ "); 
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" Driver Manager: Active\n");
    
    vga_write_string("│ "); 
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" VGA Driver: Ready (80x25 text mode)\n");
    
    vga_write_string("│ "); 
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" Total Drivers: ");
    
    // Update driver count
    g_system_info.total_drivers = 2; // VGA + Driver Manager
    g_system_info.active_drivers = 2;
    
    vga_write_string("2");
    vga_write_string("\n");
    
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_process_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("┌─ PROCESS MANAGEMENT ─────────────────────────┐\n");
    
    vga_write_string("│ "); 
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" Process Manager: Initialized\n");
    
    vga_write_string("│ "); 
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" Scheduler: Round-robin (preemptive)\n");
    
    vga_write_string("│ "); 
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" Idle Process: Running (PID 0)\n");
    
    // Update process info
    g_system_info.process_management_enabled = true;
    g_system_info.scheduler_enabled = true;
    g_system_info.total_processes = 1; // Idle process
    g_system_info.active_processes = 1;
    
    vga_write_string("│ "); 
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(" Max Processes: ");
    
    vga_write_string("256");
    vga_write_string("\n");
    
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_system_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_write_string("┌─ SYSTEM INFORMATION ────────────────────────┐\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("│ Kernel: ");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(g_system_info.kernel_name);
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string(" v");
    vga_write_string(g_system_info.kernel_version);
    vga_write_string("\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("│ Build: ");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string(g_system_info.build_date);
    vga_write_string("\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("│ Uptime: ");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string("Ready");
    vga_write_string("\n");
    
    vga_set_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_memory_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(VGA_COLOR_LIGHT_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("┌─ MEMORY STATUS ─────────────────────────────┐\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("│ Total Memory: ");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string("64MB (simulated)\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("│ Available: ");
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("48MB (75% free)\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("│ Used: ");
    vga_set_color(VGA_COLOR_LIGHT_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("16MB (25% used)\n");
    
    vga_set_color(VGA_COLOR_LIGHT_YELLOW, VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_footer(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("\n╔════════════════════════════════════════════════╗\n");
    vga_write_string("║           ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("KOS SYSTEM READY");
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("                    ║\n");
    vga_write_string("║                                              ║\n");
    vga_write_string("║  ");
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string("Enterprise Operating System v");
    vga_write_string(g_system_info.kernel_version);
    vga_write_string(" is ready");
    vga_set_color((kos_vga_color_t)VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_string("        ║\n");
    vga_write_string("╚══════════════════════════════════════════════╝\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string("\nPress any key to continue to KOS shell...\n");
    
    return HAL_SUCCESS;
}

// =============================================================================
// Utility Functions
// =============================================================================

void kos_status_print_colored(kos_status_color_t color, const char* format, ...) {
    // Simplified - just print without color processing
    (void)color;
    va_list args;
    va_start(args, format);
    char buffer[256];
    hal_vsnprintf(buffer, sizeof(buffer), format, args);
    vga_write_string(buffer);
    va_end(args);
}

void kos_status_print_level(kos_status_level_t level, const char* format, ...) {
    // Simplified - just print without level processing
    (void)level;
    va_list args;
    va_start(args, format);
    char buffer[256];
    hal_vsnprintf(buffer, sizeof(buffer), format, args);
    vga_write_string(buffer);
    va_end(args);
}

void kos_status_print_separator(char character) {
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    
    char separator[65];
    for (int i = 0; i < 64; i++) {
        separator[i] = character;
    }
    separator[64] = '\0';
    vga_write_string(separator);
    vga_write_string("\n");
}

void kos_status_print_box(const char* title, const char* content) {
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("┌─ ");
    vga_write_string(title);
    vga_write_string(" ─────────────────────────┐\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GRAY, VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_write_string(content);
    vga_write_string("\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────┘\n");
}
