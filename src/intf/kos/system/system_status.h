#pragma once

#include "hal/hal_core.h"
#include "../types.h"
#include "../config.h"
#include <stdarg.h>

// =============================================================================
// KOS - System Status Display Interface
// =============================================================================

// System status colors
typedef enum {
    KOS_STATUS_COLOR_RESET = 0,
    KOS_STATUS_COLOR_BLACK = 30,
    KOS_STATUS_COLOR_RED = 31,
    KOS_STATUS_COLOR_GREEN = 32,
    KOS_STATUS_COLOR_YELLOW = 33,
    KOS_STATUS_COLOR_BLUE = 34,
    KOS_STATUS_COLOR_MAGENTA = 35,
    KOS_STATUS_COLOR_CYAN = 36,
    KOS_STATUS_COLOR_WHITE = 37,
    KOS_STATUS_COLOR_BRIGHT_BLACK = 90,
    KOS_STATUS_COLOR_BRIGHT_RED = 91,
    KOS_STATUS_COLOR_BRIGHT_GREEN = 92,
    KOS_STATUS_COLOR_BRIGHT_YELLOW = 93,
    KOS_STATUS_COLOR_BRIGHT_BLUE = 94,
    KOS_STATUS_COLOR_BRIGHT_MAGENTA = 95,
    KOS_STATUS_COLOR_BRIGHT_CYAN = 96,
    KOS_STATUS_COLOR_BRIGHT_WHITE = 97
} kos_status_color_t;

// System status levels
typedef enum {
    KOS_STATUS_LEVEL_INFO = 0,
    KOS_STATUS_LEVEL_SUCCESS = 1,
    KOS_STATUS_LEVEL_WARNING = 2,
    KOS_STATUS_LEVEL_ERROR = 3,
    KOS_STATUS_LEVEL_DEBUG = 4
} kos_status_level_t;

// System boot phases
typedef enum {
    KOS_BOOT_PHASE_EARLY = 0,
    KOS_BOOT_PHASE_HAL = 1,
    KOS_BOOT_PHASE_DRIVERS = 2,
    KOS_BOOT_PHASE_PROCESS = 3,
    KOS_BOOT_PHASE_SERVICES = 4,
    KOS_BOOT_PHASE_SHELL = 5,
    KOS_BOOT_PHASE_COMPLETE = 6
} kos_boot_phase_t;

// System information structure
typedef struct {
    char kernel_name[32];
    char kernel_version[16];
    char build_date[32];
    char architecture[16];
    uint64_t boot_time;
    uint64_t uptime;
    uint32_t total_processes;
    uint32_t active_processes;
    uint32_t total_drivers;
    uint32_t active_drivers;
    hal_bool_t process_management_enabled;
    hal_bool_t scheduler_enabled;
    hal_bool_t debug_mode;
} kos_system_info_t;

// =============================================================================
// System Status API
// =============================================================================

// System status display functions
hal_result_t kos_system_status_init(void);
hal_result_t kos_system_status_show_header(void);
hal_result_t kos_system_status_show_boot_phase(kos_boot_phase_t phase, const char* description);
hal_result_t kos_system_status_show_component(const char* component, kos_status_level_t level, const char* status);
hal_result_t kos_system_status_show_hal_info(void);
hal_result_t kos_system_status_show_driver_info(void);
hal_result_t kos_system_status_show_process_info(void);
hal_result_t kos_system_status_show_system_info(void);
hal_result_t kos_system_status_show_memory_info(void);
hal_result_t kos_system_status_show_footer(void);

// Utility functions
void kos_status_print_colored(kos_status_color_t color, const char* format, ...);
void kos_status_print_level(kos_status_level_t level, const char* format, ...);
void kos_status_print_separator(char character);
void kos_status_print_box(const char* title, const char* content);

// =============================================================================
// System Status Macros
// =============================================================================

#define KOS_STATUS_PRINT_INFO(fmt, ...) kos_status_print_level(KOS_STATUS_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define KOS_STATUS_PRINT_SUCCESS(fmt, ...) kos_status_print_level(KOS_STATUS_LEVEL_SUCCESS, fmt, ##__VA_ARGS__)
#define KOS_STATUS_PRINT_WARNING(fmt, ...) kos_status_print_level(KOS_STATUS_LEVEL_WARNING, fmt, ##__VA_ARGS__)
#define KOS_STATUS_PRINT_ERROR(fmt, ...) kos_status_print_level(KOS_STATUS_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define KOS_STATUS_PRINT_DEBUG(fmt, ...) kos_status_print_level(KOS_STATUS_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

#define KOS_STATUS_BOX(title, content) kos_status_print_box(title, content)
