#pragma once

#include "hal_types.h"
#include "hal_interface.h"

// =============================================================================
// KOS - HAL Platform Operations Interface
// =============================================================================

// Platform information structure
typedef struct {
    const char* platform_name;
    const char* manufacturer;
    const char* product_name;
    const char* version;
    const char* serial_number;
    const char* uuid;
    const char* bios_vendor;
    const char* bios_version;
    const char* bios_release_date;
    const char* system_manufacturer;
    const char* system_product_name;
    const char* system_version;
    const char* system_serial_number;
    const char* baseboard_manufacturer;
    const char* baseboard_product_name;
    const char* baseboard_version;
    const char* baseboard_serial_number;
    const char* chassis_manufacturer;
    const char* chassis_type;
    const char* chassis_version;
    const char* chassis_serial_number;
    const char* chassis_asset_tag;
    hal_architecture_t architecture;
    hal_u32_t architecture_bits;
    hal_bool_t is_virtualized;
    hal_bool_t is_debug;
    hal_bool_t is_secure_boot;
    hal_bool_t has_acpi;
    hal_bool_t has_smbios;
    hal_bool_t has_dmi;
    hal_bool_t has_uefi;
    hal_u32_t cpu_count;
    hal_u32_t memory_size;
    hal_u32_t disk_count;
    hal_u32_t network_count;
    hal_u32_t display_count;
    hal_u32_t usb_count;
    hal_u32_t audio_count;
    hal_u32_t input_count;
} hal_platform_info_t;

// Platform event types
typedef enum {
    HAL_PLATFORM_EVENT_POWER_ON = 0,
    HAL_PLATFORM_EVENT_POWER_OFF = 1,
    HAL_PLATFORM_EVENT_REBOOT = 2,
    HAL_PLATFORM_EVENT_SLEEP = 3,
    HAL_PLATFORM_EVENT_WAKE = 4,
    HAL_PLATFORM_EVENT_HIBERNATE = 5,
    HAL_PLATFORM_EVENT_RESUME = 6,
    HAL_PLATFORM_EVENT_THERMAL = 7,
    HAL_PLATFORM_EVENT_BATTERY_LOW = 8,
    HAL_PLATFORM_EVENT_BATTERY_CRITICAL = 9,
    HAL_PLATFORM_EVENT_AC_CONNECTED = 10,
    HAL_PLATFORM_EVENT_AC_DISCONNECTED = 11,
    HAL_PLATFORM_EVENT_LID_OPEN = 12,
    HAL_PLATFORM_EVENT_LID_CLOSE = 13,
    HAL_PLATFORM_EVENT_DISPLAY_CHANGE = 14,
    HAL_PLATFORM_EVENT_DOCK = 15,
    HAL_PLATFORM_EVENT_UNDOCK = 16,
    HAL_PLATFORM_EVENT_DEVICE_CONNECT = 17,
    HAL_PLATFORM_EVENT_DEVICE_DISCONNECT = 18,
    HAL_PLATFORM_EVENT_HOTPLUG = 19,
    HAL_PLATFORM_EVENT_HOTUNPLUG = 20
} hal_platform_event_type_t;

// Platform event handler function type
typedef void (*hal_platform_event_handler_t)(hal_platform_event_type_t event_type, hal_u64_t event_data, void* context);

// Platform operations interface structure
struct hal_platform_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Platform information
    hal_result_t (*get_info)(hal_platform_info_t* info);
    const char* (*get_platform_name)(void);
    const char* (*get_manufacturer)(void);
    const char* (*get_product_name)(void);
    const char* (*get_version)(void);
    const char* (*get_serial_number)(void);
    const char* (*get_uuid)(void);
    
    // Platform detection
    hal_result_t (*detect_platform)(void);
    hal_result_t (*detect_hardware)(void);
    hal_result_t (*detect_firmware)(void);
    hal_result_t (*detect_devices)(void);
    
    // Power management
    hal_result_t (*power_on)(void);
    hal_result_t (*power_off)(void);
    hal_result_t (*reboot)(void);
    hal_result_t (*shutdown_system)(void);
    hal_result_t (*enter_sleep)(hal_u32_t state);
    hal_result_t (*exit_sleep)(hal_u32_t state);
    hal_result_t (*hibernate)(void);
    hal_result_t (*resume)(void);
    hal_u32_t (*get_power_state)(void);
    hal_result_t (*set_power_state)(hal_u32_t state);
    
    // Thermal management
    hal_result_t (*get_temperature)(hal_i32_t* temperature);
    hal_result_t (*get_thermal_state)(hal_u32_t* state);
    hal_result_t (*set_thermal_state)(hal_u32_t state);
    hal_result_t (*get_fan_speed)(hal_u32_t* speed);
    hal_result_t (*set_fan_speed)(hal_u32_t speed);
    hal_result_t (*get_cooling_state)(hal_u32_t* state);
    hal_result_t (*set_cooling_state)(hal_u32_t state);
    
    // Battery management
    hal_result_t (*get_battery_info)(hal_battery_info_t* info);
    hal_result_t (*get_battery_level)(hal_u32_t* level);
    hal_result_t (*is_battery_charging)(hal_bool_t* charging);
    hal_result_t (*get_battery_time_remaining)(hal_u32_t* minutes);
    hal_result_t (*get_battery_health)(hal_u32_t* health);
    
    // ACPI support
    hal_result_t (*acpi_init)(void);
    hal_result_t (*acpi_shutdown)(void);
    hal_result_t (*acpi_get_tables)(hal_acpi_table_t* tables, hal_u32_t* count);
    hal_result_t (*acpi_find_table)(const char* signature, hal_acpi_table_t* table);
    hal_result_t (*acpi_enable)(void);
    hal_result_t (*acpi_disable)(void);
    
    // SMBIOS support
    hal_result_t (*smbios_init)(void);
    hal_result_t (*smbios_shutdown)(void);
    hal_result_t (*smbios_get_info)(hal_smbios_info_t* info);
    hal_result_t (*smbios_get_structure)(hal_u16_t type, hal_smbios_structure_t* structure);
    hal_result_t (*smbios_find_structure)(hal_u16_t type, hal_smbios_structure_t* structure);
    
    // DMI support
    hal_result_t (*dmi_init)(void);
    hal_result_t (*dmi_shutdown)(void);
    hal_result_t (*dmi_get_info)(hal_dmi_info_t* info);
    hal_result_t (*dmi_get_structure)(hal_u16_t type, hal_dmi_structure_t* structure);
    hal_result_t (*dmi_find_structure)(hal_u16_t type, hal_dmi_structure_t* structure);
    
    // UEFI support
    hal_result_t (*uefi_init)(void);
    hal_result_t (*uefi_shutdown)(void);
    hal_result_t (*uefi_get_info)(hal_uefi_info_t* info);
    hal_result_t (*uefi_get_variables)(hal_uefi_variable_t* variables, hal_u32_t* count);
    hal_result_t (*uefi_get_variable)(const char* name, hal_uefi_variable_t* variable);
    hal_result_t (*uefi_set_variable)(const char* name, const void* data, hal_size_t size);
    
    // Virtualization support
    hal_result_t (*is_virtualized)(hal_bool_t* virtualized);
    hal_result_t (*get_virtualization_info)(hal_virtualization_info_t* info);
    hal_result_t (*get_hypervisor_info)(hal_hypervisor_info_t* info);
    hal_result_t (*exit_virtualization)(void);
    
    // Security support
    hal_result_t (*is_secure_boot_enabled)(hal_bool_t* enabled);
    hal_result_t (*get_secure_boot_info)(hal_secure_boot_info_t* info);
    hal_result_t (*enable_secure_boot)(void);
    hal_result_t (*disable_secure_boot)(void);
    hal_result_t (*get_tpm_info)(hal_tpm_info_t* info);
    
    // Event handling
    hal_result_t (*register_event_handler)(hal_platform_event_type_t event_type, hal_platform_event_handler_t handler, void* context);
    hal_result_t (*unregister_event_handler)(hal_platform_event_type_t event_type);
    hal_result_t (*handle_event)(hal_platform_event_type_t event_type, hal_u64_t event_data);
    hal_result_t (*send_event)(hal_platform_event_type_t event_type, hal_u64_t event_data);
    
    // Device management
    hal_result_t (*enumerate_devices)(hal_device_info_t* devices, hal_u32_t* count);
    hal_result_t (*get_device_info)(hal_device_id_t device_id, hal_device_info_t* info);
    hal_result_t (*enable_device)(hal_device_id_t device_id);
    hal_result_t (*disable_device)(hal_device_id_t device_id);
    hal_result_t (*reset_device)(hal_device_id_t device_id);
    
    // Platform statistics
    hal_result_t (*get_stats)(hal_platform_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
    
    // Platform debugging
    hal_result_t (*dump_platform_info)(void);
    hal_result_t (*dump_hardware_info)(void);
    hal_result_t (*dump_firmware_info)(void);
    hal_result_t (*dump_device_info)(void);
    hal_result_t (*dump_event_log)(void);
    
    // Platform configuration
    hal_result_t (*get_config)(hal_platform_config_t* config);
    hal_result_t (*set_config)(const hal_platform_config_t* config);
    hal_result_t (*validate_config)(const hal_platform_config_t* config);
    hal_result_t (*save_config)(void);
    hal_result_t (*load_config)(void);
};

// Platform operation convenience functions
hal_result_t hal_platform_init(void);
hal_result_t hal_platform_shutdown(void);
hal_result_t hal_platform_reset(void);

hal_result_t hal_platform_get_info(hal_platform_info_t* info);
const char* hal_platform_get_name(void);
const char* hal_platform_get_manufacturer(void);
const char* hal_platform_get_product_name(void);
const char* hal_platform_get_version(void);
const char* hal_platform_get_serial_number(void);

hal_result_t hal_platform_power_on(void);
hal_result_t hal_platform_power_off(void);
hal_result_t hal_platform_reboot(void);
hal_result_t hal_platform_shutdown_system(void);
hal_result_t hal_platform_enter_sleep(hal_u32_t state);
hal_result_t hal_platform_exit_sleep(hal_u32_t state);

hal_result_t hal_platform_is_virtualized(hal_bool_t* virtualized);
hal_result_t hal_platform_get_virtualization_info(hal_virtualization_info_t* info);

hal_result_t hal_platform_register_event_handler(hal_platform_event_type_t event_type, hal_platform_event_handler_t handler, void* context);
hal_result_t hal_platform_unregister_event_handler(hal_platform_event_type_t event_type);
hal_result_t hal_platform_handle_event(hal_platform_event_type_t event_type, hal_u64_t event_data);

hal_result_t hal_platform_get_stats(hal_platform_stats_t* stats);
hal_result_t hal_platform_reset_stats(void);
hal_result_t hal_platform_dump_stats(void);

// Platform control macros
#define HAL_PLATFORM_POWER_ON() hal_platform_power_on()
#define HAL_PLATFORM_POWER_OFF() hal_platform_power_off()
#define HAL_PLATFORM_REBOOT() hal_platform_reboot()
#define HAL_PLATFORM_SHUTDOWN() hal_platform_shutdown_system()
#define HAL_PLATFORM_SLEEP(state) hal_platform_enter_sleep(state)
#define HAL_PLATFORM_WAKE(state) hal_platform_exit_sleep(state)

// Platform info macros
#define HAL_PLATFORM_GET_INFO(info) hal_platform_get_info(info)
#define HAL_PLATFORM_GET_NAME() hal_platform_get_name()
#define HAL_PLATFORM_GET_MANUFACTURER() hal_platform_get_manufacturer()
#define HAL_PLATFORM_GET_PRODUCT() hal_platform_get_product_name()
#define HAL_PLATFORM_GET_VERSION() hal_platform_get_version()
#define HAL_PLATFORM_GET_SERIAL() hal_platform_get_serial_number()

// Platform virtualization macros
#define HAL_PLATFORM_IS_VIRTUALIZED(virtualized) hal_platform_is_virtualized(virtualized)
#define HAL_PLATFORM_GET_VIRT_INFO(info) hal_platform_get_virtualization_info(info)

// Platform event macros
#define HAL_PLATFORM_REGISTER_HANDLER(event, handler, context) hal_platform_register_event_handler(event, handler, context)
#define HAL_PLATFORM_UNREGISTER_HANDLER(event) hal_platform_unregister_event_handler(event)
#define HAL_PLATFORM_HANDLE_EVENT(event, data) hal_platform_handle_event(event, data)
#define HAL_PLATFORM_SEND_EVENT(event, data) hal_platform_send_event(event, data)

// Platform stats macros
#define HAL_PLATFORM_GET_STATS(stats) hal_platform_get_stats(stats)
#define HAL_PLATFORM_RESET_STATS() hal_platform_reset_stats()
#define HAL_PLATFORM_DUMP_STATS() hal_platform_dump_stats()

// Platform validation macros
#define HAL_PLATFORM_VALIDATE_PARAM(param) HAL_VALIDATE_PARAM(param)
#define HAL_PLATFORM_VALIDATE_STATE(condition) HAL_VALIDATE_STATE(condition)
