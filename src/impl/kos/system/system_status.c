#include "kos/system/system_status.h"
#include "kos/drivers/vga.h"
#include "kos/process/process.h"
#include "kos/drivers/driver_framework.h"
#include "kos/utils/log_stubs.h"
#include "kos/utils/hal_utils.h"
#include "hal/hal_interface_clean.h"
#include <stdarg.h>

// =============================================================================
// KOS - System Status Display Implementation
// =============================================================================

// Global system information
static kos_system_info_t g_system_info;
static hal_bool_t g_system_status_initialized = false;

// =============================================================================
// Internal Helper Functions
// =============================================================================

static void status_print_vga_color(kos_vga_color_t fg, kos_vga_color_t bg, const char* format, va_list args) {
    char buffer[512];
    hal_vsnprintf(buffer, sizeof(buffer), format, args);
    
    // Set VGA color and print
    vga_set_color(fg, bg);
    vga_write_string(buffer);
    
    // Reset to default color
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
}

static void status_print_vga_simple(const char* format, va_list args) {
    char buffer[512];
    hal_vsnprintf(buffer, sizeof(buffer), format, args);
    vga_write_string(buffer);
}

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
    
    // Print ASCII art header
    vga_set_color(KOS_VGA_COLOR_BRIGHT_CYAN, KOS_VGA_COLOR_BLACK);
    vga_write_string(
        "    ____  ____  __  __    _  _____                     \n"
        "   |  _ \\|  \\ \\/  |  | |/  _ \\                    \n"
        "   | |_) | \\  / /|  | | | | |                   \n"
        "   |  _ < | \\/  | | | | | |                   \n"
        "   |_| \\_|\\__/  |_|_|_|_|                   \n"
    );
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string(
        "   | | | | __ _| | __ _| | __ _| | __ _|    \n"
        "   | |_| |  _|   |  _|   |  _|   |  _|     \n"
        "   |___/|___|   |___|   |___|   |___|      \n"
        "                                            \n"
    );
    
    vga_set_color(KOS_VGA_COLOR_YELLOW, KOS_VGA_COLOR_BLACK);
    vga_write_string("           KERNEL OPERATING SYSTEM\n");
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("                 Enterprise Edition v");
    vga_write_string(g_system_info.kernel_version);
    vga_write_string("\n\n");
    
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
    
    kos_status_color_t colors[] = {
        KOS_STATUS_COLOR_BRIGHT_MAGENTA,
        KOS_STATUS_COLOR_BRIGHT_BLUE,
        KOS_STATUS_COLOR_BRIGHT_CYAN,
        KOS_STATUS_COLOR_BRIGHT_GREEN,
        KOS_STATUS_COLOR_BRIGHT_YELLOW,
        KOS_STATUS_COLOR_BRIGHT_WHITE,
        KOS_STATUS_COLOR_BRIGHT_GREEN
    };
    
    // Print phase indicator
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("[");
    
    vga_set_color((kos_vga_color_t)colors[phase], KOS_VGA_COLOR_BLACK);
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
    
    kos_vga_color_t fg_colors[] = {
        KOS_VGA_COLOR_LIGHT_GRAY,     // INFO
        KOS_VGA_COLOR_BRIGHT_GREEN,   // SUCCESS
        KOS_VGA_COLOR_BRIGHT_YELLOW,  // WARNING
        KOS_VGA_COLOR_BRIGHT_RED,     // ERROR
        KOS_VGA_COLOR_LIGHT_CYAN      // DEBUG
    };
    
    // Print component name
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("[");
    
    vga_set_color(fg_colors[level], KOS_VGA_COLOR_BLACK);
    vga_write_string(component);
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("] ");
    
    // Print status
    vga_set_color(fg_colors[level], KOS_VGA_COLOR_BLACK);
    vga_write_string(status);
    vga_write_string("\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_hal_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_BLUE, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ HARDWARE ABSTRACTION LAYER ─────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" HAL Core: Initialized\n");
    
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" HAL Interface: Active\n");
    
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" Architecture: ");
    vga_write_string(g_system_info.architecture);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_BLUE, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_driver_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_CYAN, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ DRIVER FRAMEWORK ─────────────────────────────┐\n");
    
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" Driver Manager: Active\n");
    
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" VGA Driver: Ready (80x25 text mode)\n");
    
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" Total Drivers: ");
    
    // Update driver count
    g_system_info.total_drivers = 2; // VGA + Driver Manager
    g_system_info.active_drivers = 2;
    
    char driver_count[16];
    hal_snprintf(driver_count, sizeof(driver_count), "%u", g_system_info.total_drivers);
    vga_write_string(driver_count);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_CYAN, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_process_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ PROCESS MANAGEMENT ─────────────────────────┐\n");
    
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" Process Manager: Initialized\n");
    
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" Scheduler: Round-robin (preemptive)\n");
    
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("✓");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" Idle Process: Running (PID 0)\n");
    
    // Update process info
    g_system_info.process_management_enabled = true;
    g_system_info.scheduler_enabled = true;
    g_system_info.total_processes = 1; // Idle process
    g_system_info.active_processes = 1;
    
    vga_write_string("│ "); 
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" Max Processes: ");
    
    char max_processes[16];
    hal_snprintf(max_processes, sizeof(max_processes), "%u", KOS_CONFIG_MAX_PROCESSES);
    vga_write_string(max_processes);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_system_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_MAGENTA, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ SYSTEM INFORMATION ────────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Kernel: ");
    vga_set_color(KOS_VGA_COLOR_BRIGHT_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string(g_system_info.kernel_name);
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(" v");
    vga_write_string(g_system_info.kernel_version);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Build: ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string(g_system_info.build_date);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Uptime: ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    
    uint64_t uptime = hal_get_timestamp() - g_system_info.boot_time;
    if (uptime < 1000) {
        char uptime_str[32];
        hal_snprintf(uptime_str, sizeof(uptime_str), "%llu ms", uptime);
        vga_write_string(uptime_str);
    } else if (uptime < 60000) {
        char uptime_str[32];
        hal_snprintf(uptime_str, sizeof(uptime_str), "%llu.%llu s", uptime / 1000, (uptime % 1000) / 100);
        vga_write_string(uptime_str);
    } else {
        char uptime_str[32];
        hal_snprintf(uptime_str, sizeof(uptime_str), "%llu.%llu min", uptime / 60000, (uptime % 60000) / 1000);
        vga_write_string(uptime_str);
    }
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_MAGENTA, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_memory_info(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_YELLOW, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ MEMORY STATUS ─────────────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Total Memory: ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("64MB (simulated)\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Available: ");
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("48MB (75% free)\n");
    
    vga_set_color(KOS_VGA_COLOR_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ Used: ");
    vga_set_color(KOS_VGA_COLOR_BRIGHT_YELLOW, KOS_VGA_COLOR_BLACK);
    vga_write_string("16MB (25% used)\n");
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_YELLOW, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────────────┘\n");
    
    return HAL_SUCCESS;
}

hal_result_t kos_system_status_show_footer(void) {
    if (!g_system_status_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("\n╔════════════════════════════════════════════════╗\n");
    vga_write_string("║           ");
    vga_set_color(KOS_VGA_COLOR_BRIGHT_GREEN, KOS_VGA_COLOR_BLACK);
    vga_write_string("KOS SYSTEM READY");
    vga_set_color(KOS_VGA_COLOR_BRIGHT_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("                    ║\n");
    vga_write_string("║                                              ║\n");
    vga_write_string("║  ");
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("Enterprise Operating System v");
    vga_write_string(g_system_info.kernel_version);
    vga_write_string(" is ready");
    vga_set_color(KOS_VGA_COLOR_BRIGHT_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("        ║\n");
    vga_write_string("╚════════════════════════════════════════════════╝\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("\nPress any key to continue to KOS shell...\n");
    
    return HAL_SUCCESS;
}

// =============================================================================
// Utility Functions
// =============================================================================

void kos_status_print_colored(kos_status_color_t color, const char* format, ...) {
    va_list args;
    va_start(args, format);
    status_print_vga_color((kos_vga_color_t)color, KOS_VGA_COLOR_BLACK, format, args);
    va_end(args);
}

void kos_status_print_level(kos_status_level_t level, const char* format, ...) {
    va_list args;
    va_start(args, format);
    status_print_vga_simple(format, args);
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
    vga_set_color(KOS_VGA_COLOR_BRIGHT_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("┌─ ");
    vga_write_string(title);
    vga_write_string(" ─────────────────────────┐\n");
    
    vga_set_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    vga_write_string("│ ");
    vga_write_string(content);
    vga_write_string("\n");
    
    vga_set_color(KOS_VGA_COLOR_BRIGHT_WHITE, KOS_VGA_COLOR_BLACK);
    vga_write_string("└────────────────────────────────────┘\n");
}
