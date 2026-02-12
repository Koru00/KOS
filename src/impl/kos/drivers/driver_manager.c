#include "kos/drivers/driver_framework.h"
#include "kos/utils/log_stubs.h"
#include "kos/utils/hal_utils.h"

// =============================================================================
// KOS - Driver Manager Implementation
// =============================================================================

// HAL error codes that are missing from hal_core.h
#define HAL_ERROR_NOT_INITIALIZED  HAL_ERROR_INVALID_STATE
#define HAL_ERROR_ALREADY_EXISTS   HAL_ERROR_INVALID_PARAM
#define HAL_ERROR_LIMIT_EXCEEDED  HAL_ERROR_OUT_OF_MEMORY
#define HAL_ERROR_ALREADY_OPEN     HAL_ERROR_INVALID_PARAM
#define HAL_ERROR_NOT_OPEN        HAL_ERROR_INVALID_STATE

// Global driver manager instance
static kos_driver_manager_t g_driver_manager = {0};

// =============================================================================
// Driver Manager Core Operations
// =============================================================================

hal_result_t kos_driver_manager_init(void) {
    if (g_driver_manager.initialized) {
        return HAL_SUCCESS;
    }
    
    log_info("Initializing KOS Driver Manager");
    
    // Initialize manager structure
    hal_memset(&g_driver_manager, 0, sizeof(kos_driver_manager_t));
    
    // Initialize mutexes (simplified - no real mutex implementation yet)
    // hal_result_t result = hal_mutex_init(&g_driver_manager.driver_lock);
    // if (result != HAL_SUCCESS) {
    //     log_error("Failed to initialize driver mutex");
    //     return result;
    // }
    
    // result = hal_mutex_init(&g_driver_manager.device_lock);
    // if (result != HAL_SUCCESS) {
    //     log_error("Failed to initialize device mutex");
    //     hal_mutex_destroy(&g_driver_manager.driver_lock);
    //     return result;
    // }
    
    g_driver_manager.initialized = true;
    
    log_info("KOS Driver Manager initialized successfully");
    return HAL_SUCCESS;
}

hal_result_t kos_driver_manager_shutdown(void) {
    if (!g_driver_manager.initialized) {
        return HAL_SUCCESS;
    }
    
    log_info("Shutting down KOS Driver Manager");
    
    // Stop all drivers
    kos_driver_t* driver = g_driver_manager.drivers;
    while (driver) {
        kos_driver_stop(driver);
        kos_driver_shutdown(driver);
        driver = driver->next;
    }
    
    // Destroy mutexes (simplified)
    // hal_mutex_destroy(&g_driver_manager.device_lock);
    // hal_mutex_destroy(&g_driver_manager.driver_lock);
    
    // Clear manager structure
    hal_memset(&g_driver_manager, 0, sizeof(kos_driver_manager_t));
    
    log_info("KOS Driver Manager shutdown completed");
    return HAL_SUCCESS;
}

hal_result_t kos_driver_manager_register_driver(kos_driver_t* driver) {
    if (!driver || !driver->ops) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (!g_driver_manager.initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    // hal_mutex_lock(&g_driver_manager.driver_lock);
    
    // Check if driver already exists
    kos_driver_t* existing = kos_driver_find_by_name(driver->name);
    if (existing) {
        // // hal_mutex_unlock(&g_driver_manager.driver_lock);
        log_error("Driver %s already registered", driver->name);
        return HAL_ERROR_ALREADY_EXISTS;
    }
    
    // Initialize driver if needed
    if (driver->state == KOS_DRIVER_STATE_UNINITIALIZED) {
        if (driver->ops->init) {
            hal_result_t result = driver->ops->init(driver);
            if (result != HAL_SUCCESS) {
                // hal_mutex_unlock(&g_driver_manager.driver_lock);
                log_error("Failed to initialize driver %s", driver->name);
                return result;
            }
        }
        driver->state = KOS_DRIVER_STATE_INITIALIZED;
    }
    
    // Add to driver list
    driver->next = g_driver_manager.drivers;
    g_driver_manager.drivers = driver;
    g_driver_manager.driver_count++;
    g_driver_manager.total_drivers++;
    
    // hal_mutex_unlock(&g_driver_manager.driver_lock);
    
    log_info("Driver %s registered successfully", driver->name);
    return HAL_SUCCESS;
}

hal_result_t kos_driver_manager_unregister_driver(kos_driver_t* driver) {
    if (!driver) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (!g_driver_manager.initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    // hal_mutex_lock(&g_driver_manager.driver_lock);
    
    // Find and remove from list
    kos_driver_t** current = &g_driver_manager.drivers;
    while (*current) {
        if (*current == driver) {
            *current = driver->next;
            g_driver_manager.driver_count--;
            break;
        }
        current = &(*current)->next;
    }
    
    // Stop and shutdown driver
    kos_driver_stop(driver);
    kos_driver_shutdown(driver);
    
    // hal_mutex_unlock(&g_driver_manager.driver_lock);
    
    log_info("Driver %s unregistered successfully", driver->name);
    return HAL_SUCCESS;
}

hal_result_t kos_driver_manager_register_device(kos_device_t* device) {
    if (!device || !device->driver) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (!g_driver_manager.initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    // hal_mutex_lock(&g_driver_manager.device_lock);
    
    // Add to device list
    device->next = g_driver_manager.devices;
    g_driver_manager.devices = device;
    g_driver_manager.device_count++;
    g_driver_manager.total_devices++;
    
    // hal_mutex_unlock(&g_driver_manager.device_lock);
    
    log_info("Device %s registered successfully", device->info.name);
    return HAL_SUCCESS;
}

hal_result_t kos_driver_manager_unregister_device(kos_device_t* device) {
    if (!device) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (!g_driver_manager.initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    // hal_mutex_lock(&g_driver_manager.device_lock);
    
    // Find and remove from list
    kos_device_t** current = &g_driver_manager.devices;
    while (*current) {
        if (*current == device) {
            *current = device->next;
            g_driver_manager.device_count--;
            break;
        }
        current = &(*current)->next;
    }
    
    // hal_mutex_unlock(&g_driver_manager.device_lock);
    
    log_info("Device %s unregistered successfully", device->info.name);
    return HAL_SUCCESS;
}

// =============================================================================
// Driver Operations
// =============================================================================

hal_result_t kos_driver_init(kos_driver_t* driver, const char* name, kos_driver_type_t type, kos_driver_ops_t* ops) {
    if (!driver || !name || !ops) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Initialize driver structure
    hal_memset(driver, 0, sizeof(kos_driver_t));
    hal_strcpy(driver->name, name);
    driver->type = type;
    driver->ops = ops;
    driver->state = KOS_DRIVER_STATE_UNINITIALIZED;
    
    // Get HAL interface
    driver->hal_interface = hal_get_interface();
    if (!driver->hal_interface) {
        log_error("Failed to get HAL interface for driver %s", name);
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    log_info("Driver %s initialized successfully", name);
    return HAL_SUCCESS;
}

hal_result_t kos_driver_shutdown(kos_driver_t* driver) {
    if (!driver) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (driver->state == KOS_DRIVER_STATE_UNINITIALIZED) {
        return HAL_SUCCESS;
    }
    
    log_info("Shutting down driver %s", driver->name);
    
    // Stop driver if running
    if (driver->state == KOS_DRIVER_STATE_RUNNING) {
        kos_driver_stop(driver);
    }
    
    // Call driver shutdown
    if (driver->ops->shutdown) {
        hal_result_t result = driver->ops->shutdown(driver);
        if (result != HAL_SUCCESS) {
            log_error("Driver %s shutdown failed", driver->name);
            driver->error_count++;
            return result;
        }
    }
    
    driver->state = KOS_DRIVER_STATE_UNINITIALIZED;
    log_info("Driver %s shutdown completed", driver->name);
    return HAL_SUCCESS;
}

hal_result_t kos_driver_start(kos_driver_t* driver) {
    if (!driver) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (driver->state != KOS_DRIVER_STATE_INITIALIZED) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    log_info("Starting driver %s", driver->name);
    
    // Call driver start
    if (driver->ops->start) {
        hal_result_t result = driver->ops->start(driver);
        if (result != HAL_SUCCESS) {
            log_error("Driver %s start failed", driver->name);
            driver->error_count++;
            driver->state = KOS_DRIVER_STATE_ERROR;
            return result;
        }
    }
    
    driver->state = KOS_DRIVER_STATE_RUNNING;
    driver->start_count++;
    g_driver_manager.active_drivers++;
    
    log_info("Driver %s started successfully", driver->name);
    return HAL_SUCCESS;
}

hal_result_t kos_driver_stop(kos_driver_t* driver) {
    if (!driver) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (driver->state != KOS_DRIVER_STATE_RUNNING) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    log_info("Stopping driver %s", driver->name);
    
    // Call driver stop
    if (driver->ops->stop) {
        hal_result_t result = driver->ops->stop(driver);
        if (result != HAL_SUCCESS) {
            log_error("Driver %s stop failed", driver->name);
            driver->error_count++;
            driver->state = KOS_DRIVER_STATE_ERROR;
            return result;
        }
    }
    
    driver->state = KOS_DRIVER_STATE_STOPPED;
    driver->stop_count++;
    g_driver_manager.active_drivers--;
    
    log_info("Driver %s stopped successfully", driver->name);
    return HAL_SUCCESS;
}

hal_result_t kos_driver_add_device(kos_driver_t* driver, kos_device_t* device) {
    if (!driver || !device) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Check device limit
    if (driver->max_devices > 0 && driver->device_count >= driver->max_devices) {
        return HAL_ERROR_LIMIT_EXCEEDED;
    }
    
    // Set device driver
    device->driver = driver;
    
    // Add to driver's device list
    device->next = driver->devices;
    driver->devices = device;
    driver->device_count++;
    
    // Probe device
    if (driver->ops->probe) {
        hal_result_t result = driver->ops->probe(driver, device);
        if (result != HAL_SUCCESS) {
            log_error("Driver %s failed to probe device %s", driver->name, device->info.name);
            device->driver = NULL;
            driver->devices = device->next;
            driver->device_count--;
            return result;
        }
    }
    
    log_info("Device %s added to driver %s", device->info.name, driver->name);
    return HAL_SUCCESS;
}

hal_result_t kos_driver_remove_device(kos_driver_t* driver, kos_device_t* device) {
    if (!driver || !device) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Find and remove from driver's device list
    kos_device_t** current = &driver->devices;
    while (*current) {
        if (*current == device) {
            *current = device->next;
            driver->device_count--;
            break;
        }
        current = &(*current)->next;
    }
    
    // Remove device
    if (driver->ops->remove) {
        driver->ops->remove(driver, device);
    }
    
    device->driver = NULL;
    
    log_info("Device %s removed from driver %s", device->info.name, driver->name);
    return HAL_SUCCESS;
}

// =============================================================================
// Device Operations
// =============================================================================

hal_result_t kos_device_init(kos_device_t* device, const kos_device_info_t* info) {
    if (!device || !info) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Initialize device structure
    hal_memset(device, 0, sizeof(kos_device_t));
    hal_memcpy(&device->info, info, sizeof(kos_device_info_t));
    device->state = KOS_DRIVER_STATE_INITIALIZED;
    device->reference_count = 0;
    device->is_open = false;
    
    // Initialize device lock (simplified)
    // hal_result_t result = hal_mutex_init(&device->lock);
    // if (result != HAL_SUCCESS) {
    //     log_error("Failed to initialize device lock for %s", info->name);
    //     return result;
    // }
    
    log_info("Device %s initialized successfully", info->name);
    return HAL_SUCCESS;
}

hal_result_t kos_device_shutdown(kos_device_t* device) {
    if (!device) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Close device if open
    if (device->is_open) {
        kos_device_close(device);
    }
    
    // Destroy device lock (simplified)
    // hal_mutex_destroy(&device->lock);
    
    device->state = KOS_DRIVER_STATE_UNINITIALIZED;
    
    log_info("Device %s shutdown completed", device->info.name);
    return HAL_SUCCESS;
}

hal_result_t kos_device_open(kos_device_t* device) {
    if (!device) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // hal_mutex_lock(&device->lock);
    
    if (device->is_open) {
        // hal_mutex_unlock(&device->lock);
        return HAL_ERROR_ALREADY_OPEN;
    }
    
    device->is_open = true;
    device->reference_count++;
    
    // hal_mutex_unlock(&device->lock);
    
    log_debug("Device %s opened", device->info.name);
    return HAL_SUCCESS;
}

hal_result_t kos_device_close(kos_device_t* device) {
    if (!device) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // hal_mutex_lock(&device->lock);
    
    if (!device->is_open) {
        // hal_mutex_unlock(&device->lock);
        return HAL_ERROR_NOT_OPEN;
    }
    
    device->is_open = false;
    if (device->reference_count > 0) {
        device->reference_count--;
    }
    
    // hal_mutex_unlock(&device->lock);
    
    log_debug("Device %s closed", device->info.name);
    return HAL_SUCCESS;
}

hal_result_t kos_device_read(kos_device_t* device, void* buffer, size_t size, size_t* bytes_read) {
    if (!device || !buffer || !device->driver) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (!device->is_open) {
        return HAL_ERROR_NOT_OPEN;
    }
    
    if (!device->driver->ops->read) {
        return HAL_ERROR_NOT_SUPPORTED;
    }
    
    hal_result_t result = device->driver->ops->read(device, buffer, size, bytes_read);
    if (result != HAL_SUCCESS) {
        device->driver->error_count++;
    }
    
    return result;
}

hal_result_t kos_device_write(kos_device_t* device, const void* buffer, size_t size, size_t* bytes_written) {
    if (!device || !buffer || !device->driver) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (!device->is_open) {
        return HAL_ERROR_NOT_OPEN;
    }
    
    if (!device->driver->ops->write) {
        return HAL_ERROR_NOT_SUPPORTED;
    }
    
    hal_result_t result = device->driver->ops->write(device, buffer, size, bytes_written);
    if (result != HAL_SUCCESS) {
        device->driver->error_count++;
    }
    
    return result;
}

hal_result_t kos_device_ioctl(kos_device_t* device, uint32_t request, void* arg) {
    if (!device || !device->driver) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (!device->is_open) {
        return HAL_ERROR_NOT_OPEN;
    }
    
    if (!device->driver->ops->ioctl) {
        return HAL_ERROR_NOT_SUPPORTED;
    }
    
    hal_result_t result = device->driver->ops->ioctl(device, request, arg);
    if (result != HAL_SUCCESS) {
        device->driver->error_count++;
    }
    
    return result;
}

// =============================================================================
// Utility Functions
// =============================================================================

kos_driver_t* kos_driver_find_by_name(const char* name) {
    if (!name || !g_driver_manager.initialized) {
        return NULL;
    }
    
    kos_driver_t* driver = g_driver_manager.drivers;
    while (driver) {
        if (hal_strcmp(driver->name, name) == 0) {
            return driver;
        }
        driver = driver->next;
    }
    
    return NULL;
}

kos_device_t* kos_device_find_by_name(const char* name) {
    if (!name || !g_driver_manager.initialized) {
        return NULL;
    }
    
    kos_device_t* device = g_driver_manager.devices;
    while (device) {
        if (hal_strcmp(device->info.name, name) == 0) {
            return device;
        }
        device = device->next;
    }
    
    return NULL;
}

kos_device_t* kos_device_find_by_id(uint32_t device_id) {
    if (!g_driver_manager.initialized) {
        return NULL;
    }
    
    kos_device_t* device = g_driver_manager.devices;
    while (device) {
        if (device->info.device_id == device_id) {
            return device;
        }
        device = device->next;
    }
    
    return NULL;
}

hal_result_t kos_driver_manager_dump_stats(void) {
    if (!g_driver_manager.initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    log_info("=== KOS Driver Manager Statistics ===");
    log_info("Total Drivers: %llu", g_driver_manager.total_drivers);
    log_info("Active Drivers: %llu", g_driver_manager.active_drivers);
    log_info("Total Devices: %llu", g_driver_manager.total_devices);
    log_info("Active Devices: %llu", g_driver_manager.active_devices);
    log_info("Failed Operations: %llu", g_driver_manager.failed_operations);
    
    return HAL_SUCCESS;
}

hal_result_t kos_driver_manager_dump_drivers(void) {
    if (!g_driver_manager.initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    log_info("=== Registered Drivers ===");
    
    kos_driver_t* driver = g_driver_manager.drivers;
    while (driver) {
        log_info("Driver: %s", driver->name);
        log_info("  Type: %d", driver->type);
        log_info("  State: %d", driver->state);
        log_info("  Devices: %u", driver->device_count);
        log_info("  Init Count: %llu", driver->init_count);
        log_info("  Start Count: %llu", driver->start_count);
        log_info("  Error Count: %llu", driver->error_count);
        driver = driver->next;
    }
    
    return HAL_SUCCESS;
}

hal_result_t kos_driver_manager_dump_devices(void) {
    if (!g_driver_manager.initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    log_info("=== Registered Devices ===");
    
    kos_device_t* device = g_driver_manager.devices;
    while (device) {
        log_info("Device: %s", device->info.name);
        log_info("  Driver: %s", device->driver ? device->driver->name : "None");
        log_info("  State: %d", device->state);
        log_info("  Device ID: 0x%08x", device->info.device_id);
        log_info("  Vendor ID: 0x%08x", device->info.vendor_id);
        log_info("  IRQ: %u", device->info.irq_line);
        device = device->next;
    }
    
    return HAL_SUCCESS;
}
