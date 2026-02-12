#include "kos/drivers/driver_framework.h"
#include "kos/drivers/vga.h"
#include "kos/utils/log_stubs.h"
#include "kos/utils/hal_utils.h"

// =============================================================================
// KOS - Simple VGA Driver Implementation (HAL-based)
// =============================================================================

// VGA hardware constants
#define VGA_MEMORY_BASE        0xB8000
#define VGA_WIDTH             80
#define VGA_HEIGHT            25
#define VGA_SIZE              (VGA_WIDTH * VGA_HEIGHT)

// VGA entry structure
typedef struct {
    uint8_t character;
    uint8_t color;
} __attribute__((packed)) vga_entry_t;

// VGA driver private data
typedef struct {
    vga_entry_t* buffer;
    uint32_t cursor_x;
    uint32_t cursor_y;
    uint8_t current_color;
    hal_bool_t initialized;
} vga_driver_data_t;

// VGA driver instance
static kos_driver_t g_vga_driver;
static kos_device_t g_vga_device;
static vga_driver_data_t g_vga_data;

// =============================================================================
// VGA Helper Functions
// =============================================================================

static inline uint8_t vga_make_color(kos_vga_color_t fg, kos_vga_color_t bg) {
    return fg | (bg << 4);
}

static inline vga_entry_t vga_make_entry(char c, uint8_t color) {
    vga_entry_t entry;
    entry.character = c;
    entry.color = color;
    return entry;
}

static void vga_set_cursor(uint32_t x, uint32_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return;
    }
    
    g_vga_data.cursor_x = x;
    g_vga_data.cursor_y = y;
    
    // Calculate cursor position
    // uint16_t pos = y * VGA_WIDTH + x;
    
    // Send cursor position to VGA hardware (simplified)
    // hal_outb(0x3D4, 14);
    // hal_outb(0x3D5, (pos >> 8) & 0xFF);
    // hal_outb(0x3D4, 15);
    // hal_outb(0x3D5, pos & 0xFF);
}

static void vga_put_char_at(char c, uint8_t color, uint32_t x, uint32_t y) {
    if (x >= VGA_WIDTH || y >= VGA_HEIGHT) {
        return;
    }
    
    const size_t index = y * VGA_WIDTH + x;
    g_vga_data.buffer[index] = vga_make_entry(c, color);
}

static void vga_put_char(char c) {
    switch (c) {
        case '\n':
            g_vga_data.cursor_x = 0;
            g_vga_data.cursor_y++;
            break;
        case '\r':
            g_vga_data.cursor_x = 0;
            break;
        case '\t':
            for (uint32_t i = 0; i < 4; i++) {
                vga_put_char_at(' ', g_vga_data.current_color, g_vga_data.cursor_x++, g_vga_data.cursor_y);
                if (g_vga_data.cursor_x >= VGA_WIDTH) {
                    g_vga_data.cursor_x = 0;
                    g_vga_data.cursor_y++;
                }
            }
            break;
        default:
            vga_put_char_at(c, g_vga_data.current_color, g_vga_data.cursor_x++, g_vga_data.cursor_y);
            break;
    }
    
    // Handle line wrapping
    if (g_vga_data.cursor_x >= VGA_WIDTH) {
        g_vga_data.cursor_x = 0;
        g_vga_data.cursor_y++;
    }
    
    // Handle scrolling
    if (g_vga_data.cursor_y >= VGA_HEIGHT) {
        // Move all lines up by one
        for (size_t i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            g_vga_data.buffer[i] = g_vga_data.buffer[i + VGA_WIDTH];
        }
        
        // Clear the last line
        for (size_t i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            g_vga_data.buffer[i] = vga_make_entry(' ', g_vga_data.current_color);
        }
        
        g_vga_data.cursor_y = VGA_HEIGHT - 1;
    }
    
    vga_set_cursor(g_vga_data.cursor_x, g_vga_data.cursor_y);
}

static void vga_clear_screen(void) {
    for (size_t i = 0; i < VGA_SIZE; i++) {
        g_vga_data.buffer[i] = vga_make_entry(' ', g_vga_data.current_color);
    }
    
    g_vga_data.cursor_x = 0;
    g_vga_data.cursor_y = 0;
    vga_set_cursor(0, 0);
}

// =============================================================================
// VGA Driver Operations
// =============================================================================

static hal_result_t vga_driver_init(kos_driver_t* driver) {
    (void)driver;
    log_info("Initializing VGA driver");
    
    // Initialize private data
    hal_memset(&g_vga_data, 0, sizeof(vga_driver_data_t));
    g_vga_data.buffer = (vga_entry_t*)VGA_MEMORY_BASE;
    g_vga_data.current_color = vga_make_color(KOS_VGA_COLOR_LIGHT_GRAY, KOS_VGA_COLOR_BLACK);
    g_vga_data.initialized = true;
    
    // Clear screen
    vga_clear_screen();
    
    log_info("VGA driver initialized successfully");
    return HAL_SUCCESS;
}

static hal_result_t vga_driver_shutdown(kos_driver_t* driver) {
    (void)driver;
    log_info("Shutting down VGA driver");
    
    g_vga_data.initialized = false;
    
    log_info("VGA driver shutdown completed");
    return HAL_SUCCESS;
}

static hal_result_t vga_driver_start(kos_driver_t* driver) {
    log_info("Starting VGA driver");
    
    driver->state = KOS_DRIVER_STATE_RUNNING;
    
    log_info("VGA driver started successfully");
    return HAL_SUCCESS;
}

static hal_result_t vga_driver_stop(kos_driver_t* driver) {
    log_info("Stopping VGA driver");
    
    driver->state = KOS_DRIVER_STATE_STOPPED;
    
    log_info("VGA driver stopped successfully");
    return HAL_SUCCESS;
}

static hal_result_t vga_driver_probe(kos_driver_t* driver, kos_device_t* device) {
    (void)driver;
    log_info("Probing VGA device %s", device->info.name);
    
    // VGA is always present in standard PC hardware
    return HAL_SUCCESS;
}

static hal_result_t vga_driver_remove(kos_driver_t* driver, kos_device_t* device) {
    (void)driver;
    log_info("Removing VGA device %s", device->info.name);
    
    // Clear screen before removing
    vga_clear_screen();
    
    return HAL_SUCCESS;
}

static hal_result_t vga_device_write(kos_device_t* device, const void* buffer, size_t size, size_t* bytes_written) {
    if (!device || !buffer || !g_vga_data.initialized) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    const char* str = (const char*)buffer;
    size_t written = 0;
    
    for (size_t i = 0; i < size; i++) {
        vga_put_char(str[i]);
        written++;
    }
    
    if (bytes_written) {
        *bytes_written = written;
    }
    
    return HAL_SUCCESS;
}

static hal_result_t vga_device_ioctl(kos_device_t* device, uint32_t request, void* arg) {
    if (!device || !g_vga_data.initialized) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    switch (request) {
        case KOS_VGA_IOCTL_SET_COLOR: {
            uint32_t* color = (uint32_t*)arg;
            if (color) {
                g_vga_data.current_color = (uint8_t)*color;
            }
            return HAL_SUCCESS;
        }
        
        case KOS_VGA_IOCTL_GET_COLOR: {
            uint32_t* color = (uint32_t*)arg;
            if (color) {
                *color = g_vga_data.current_color;
            }
            return HAL_SUCCESS;
        }
        
        case KOS_VGA_IOCTL_CLEAR_SCREEN: {
            vga_clear_screen();
            return HAL_SUCCESS;
        }
        
        case KOS_VGA_IOCTL_SET_CURSOR: {
            uint32_t* pos = (uint32_t*)arg;
            if (pos) {
                vga_set_cursor(pos[0], pos[1]);
            }
            return HAL_SUCCESS;
        }
        
        case KOS_VGA_IOCTL_GET_CURSOR: {
            uint32_t* pos = (uint32_t*)arg;
            if (pos) {
                pos[0] = g_vga_data.cursor_x;
                pos[1] = g_vga_data.cursor_y;
            }
            return HAL_SUCCESS;
        }
        
        default:
            return HAL_ERROR_NOT_SUPPORTED;
    }
}

static hal_result_t vga_device_get_status(kos_device_t* device, void* status, size_t size) {
    if (!device || !status || size < sizeof(kos_vga_status_t)) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    kos_vga_status_t* vga_status = (kos_vga_status_t*)status;
    vga_status->cursor_x = g_vga_data.cursor_x;
    vga_status->cursor_y = g_vga_data.cursor_y;
    vga_status->current_color = g_vga_data.current_color;
    vga_status->width = VGA_WIDTH;
    vga_status->height = VGA_HEIGHT;
    vga_status->initialized = g_vga_data.initialized;
    
    return HAL_SUCCESS;
}

// =============================================================================
// VGA Driver Operations Structure
// =============================================================================

static kos_driver_ops_t vga_driver_ops = {
    .init = vga_driver_init,
    .shutdown = vga_driver_shutdown,
    .start = vga_driver_start,
    .stop = vga_driver_stop,
    .probe = vga_driver_probe,
    .remove = vga_driver_remove,
    .read = NULL,
    .write = vga_device_write,
    .ioctl = vga_device_ioctl,
    .interrupt_handler = NULL,
    .power_on = NULL,
    .power_off = NULL,
    .get_config = NULL,
    .set_config = NULL,
    .get_status = vga_device_get_status,
    .reset = NULL,
    .self_test = NULL
};

// =============================================================================
// VGA Driver Registration
// =============================================================================

hal_result_t vga_init(void) {
    log_info("Registering VGA driver");
    
    // Initialize driver
    hal_result_t result = kos_driver_init(&g_vga_driver, "vga", KOS_DRIVER_TYPE_OUTPUT, &vga_driver_ops);
    if (result != HAL_SUCCESS) {
        log_error("Failed to initialize VGA driver");
        return result;
    }
    
    // Set driver properties
    g_vga_driver.flags = KOS_DRIVER_FLAG_CRITICAL;
    hal_strcpy(g_vga_driver.description, "VGA Text Mode Display Driver");
    hal_strcpy(g_vga_driver.version, "1.0.0");
    
    // Initialize device info
    kos_device_info_t device_info = {
        .name = "vga0",
        .description = "VGA Text Mode Display",
        .version = "1.0.0",
        .manufacturer = "KOS",
        .device_id = 0x0001,
        .vendor_id = 0x0001,
        .class_id = 0x03,
        .subclass_id = 0x00,
        .base_address = VGA_MEMORY_BASE,
        .memory_size = VGA_SIZE * sizeof(vga_entry_t),
        .irq_line = 0,
        .dma_channel = 0,
        .flags = KOS_DRIVER_FLAG_CRITICAL
    };
    
    // Initialize device
    result = kos_device_init(&g_vga_device, &device_info);
    if (result != HAL_SUCCESS) {
        log_error("Failed to initialize VGA device");
        return result;
    }
    
    // Register driver
    result = kos_driver_manager_register_driver(&g_vga_driver);
    if (result != HAL_SUCCESS) {
        log_error("Failed to register VGA driver");
        return result;
    }
    
    // Add device to driver
    result = kos_driver_add_device(&g_vga_driver, &g_vga_device);
    if (result != HAL_SUCCESS) {
        log_error("Failed to add VGA device to driver");
        return result;
    }
    
    // Start driver
    result = kos_driver_start(&g_vga_driver);
    if (result != HAL_SUCCESS) {
        log_error("Failed to start VGA driver");
        return result;
    }
    
    log_info("VGA driver registered and started successfully");
    return HAL_SUCCESS;
}

// =============================================================================
// VGA Public API
// =============================================================================

hal_result_t vga_write_string(const char* str) {
    if (!str || !g_vga_data.initialized) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    size_t len = hal_strlen(str);
    return vga_device_write(&g_vga_device, str, len, NULL);
}

hal_result_t vga_set_color(kos_vga_color_t fg, kos_vga_color_t bg) {
    if (!g_vga_data.initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    g_vga_data.current_color = vga_make_color(fg, bg);
    return HAL_SUCCESS;
}

hal_result_t vga_clear(void) {
    if (!g_vga_data.initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    vga_clear_screen();
    return HAL_SUCCESS;
}
