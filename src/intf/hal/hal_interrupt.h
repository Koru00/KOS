#pragma once

#include "hal_types.h"
#include "hal_interface.h"

// =============================================================================
// KOS - HAL Interrupt Operations Interface
// =============================================================================

// Interrupt context structure
typedef struct {
    hal_u32_t vector;
    hal_u32_t error_code;
    hal_u64_t rip;
    hal_u64_t rsp;
    hal_u64_t rflags;
    hal_u64_t cr2;
    hal_u64_t cr8;
    hal_u64_t cr10;
    hal_u64_t cr11;
    hal_u64_t cr12;
    hal_u64_t cr13;
    hal_u64_t cr14;
    hal_u64_t cr15;
    hal_u64_t rax;
    hal_u64_t rbx;
    hal_u64_t rcx;
    hal_u64_t rdx;
    hal_u64_t rsi;
    hal_u64_t rdi;
    hal_u64_t rbp;
    hal_u64_t r8;
    hal_u64_t r9;
    hal_u64_t r10;
    hal_u64_t r11;
    hal_u64_t r12;
    hal_u64_t r13;
    hal_u64_t r14;
    hal_u64_t r15;
    hal_u32_t cs;
    hal_u32_t ss;
    hal_u32_t ds;
    hal_u32_t es;
    hal_u32_t fs;
    hal_u32_t gs;
    hal_u32_t fs_base;
    hal_u32_t gs_base;
} hal_interrupt_context_t;

// Interrupt handler function type
typedef void (*hal_interrupt_handler_t)(uint32_t vector, void* context);

// Interrupt operations interface structure
struct hal_interrupt_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Interrupt information
    hal_result_t (*get_info)(hal_interrupt_info_t* info);
    hal_u32_t (*get_total_interrupts)(void);
    hal_u32_t (*get_available_interrupts)(void);
    hal_u32_t (*get_max_priority)(void);
    
    // Interrupt control
    hal_result_t (*enable_interrupts)(void);
    hal_result_t (*disable_interrupts)(void);
    hal_u32_t (*get_interrupt_state)(void);
    hal_result_t (*save_interrupt_state)(hal_u32_t* flags);
    hal_result_t (*restore_interrupt_state)(hal_u32_t flags);
    
    // Interrupt registration
    hal_result_t (*register_handler)(hal_u32_t vector, hal_interrupt_handler_t handler);
    hal_result_t (*unregister_handler)(hal_u32_t vector);
    hal_interrupt_handler_t (*get_handler)(hal_u32_t vector);
    
    // Interrupt masking
    hal_result_t (*mask_interrupt)(hal_u32_t vector);
    hal_result_t (*unmask_interrupt)(hal_u32_t vector);
    hal_result_t (*mask_all_interrupts)(void);
    hal_result_t (*unmask_all_interrupts)(void);
    
    // Interrupt priority
    hal_result_t (*set_priority)(hal_u32_t vector, hal_u32_t priority);
    hal_u32_t (*get_priority)(hal_u32_t vector);
    hal_result_t (*set_global_priority)(hal_u32_t priority);
    
    // Interrupt handling
    hal_result_t (*handle_interrupt)(hal_u32_t vector);
    hal_result_t (*handle_nmi)(void);
    hal_result_t (*handle_exception)(hal_u32_t vector, hal_u32_t error_code);
    
    // PIC operations
    hal_result_t (*pic_init)(void);
    hal_result_t (*pic_send_eoi)(hal_u32_t vector);
    hal_result_t (*pic_mask_irq)(hal_u32_t irq);
    hal_result_t (*pic_unmask_irq)(hal_u32_t irq);
    
    // APIC operations
    hal_result_t (*apic_init)(void);
    hal_result_t (*apic_send_ipi)(hal_u32_t cpu_id, hal_u32_t vector);
    hal_result_t (*apic_send_eoi)(hal_u32_t vector);
    hal_result_t (*apic_set_timer)(hal_u32_t vector, hal_u32_t initial_count);
    hal_result_t (*apic_enable)(void);
    hal_result_t (*apic_disable)(void);
    
    // MSI operations
    hal_result_t (*msi_enable)(void);
    hal_result_t (*msi_disable)(void);
    hal_result_t (*msi_setup)(hal_u32_t vector, hal_u64_t address, hal_u32_t data);
    hal_result_t (*msi_teardown)(hal_u32_t vector);
    
    // Interrupt statistics
    hal_result_t (*get_stats)(hal_interrupt_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
    
    // Interrupt debugging
    hal_result_t (*dump_interrupt_map)(void);
    hal_result_t (*dump_interrupt_handlers)(void);
    hal_result_t (*dump_interrupt_context)(hal_interrupt_context_t* context);
};

// Interrupt operation convenience functions
hal_result_t hal_interrupt_init(void);
hal_result_t hal_interrupt_shutdown(void);
hal_result_t hal_interrupt_reset(void);

hal_result_t hal_interrupt_enable(void);
hal_result_t hal_interrupt_disable(void);
hal_u32_t hal_interrupt_get_state(void);

hal_result_t hal_interrupt_register_handler(hal_u32_t vector, hal_interrupt_handler_t handler);
hal_result_t hal_interrupt_unregister_handler(hal_u32_t vector);
hal_interrupt_handler_t hal_interrupt_get_handler(hal_u32_t vector);

hal_result_t hal_interrupt_mask(hal_u32_t vector);
hal_result_t hal_interrupt_unmask(hal_u32_t vector);
hal_result_t hal_interrupt_mask_all(void);
hal_result_t hal_interrupt_unmask_all(void);

hal_result_t hal_interrupt_handle(hal_u32_t vector);
hal_result_t hal_interrupt_handle_nmi(void);
hal_result_t hal_interrupt_handle_exception(hal_u32_t vector, hal_u32_t error_code);

hal_result_t hal_interrupt_get_info(hal_interrupt_info_t* info);
hal_u32_t hal_interrupt_get_total_interrupts(void);
hal_u32_t hal_interrupt_get_available_interrupts(void);
hal_u32_t hal_interrupt_get_max_priority(void);

// Interrupt control macros
#define HAL_INTERRUPT_ENABLE() hal_interrupt_enable()
#define HAL_INTERRUPT_DISABLE() hal_interrupt_disable()
#define HAL_INTERRUPT_GET_STATE() hal_interrupt_get_state()
#define HAL_INTERRUPT_SAVE_STATE(flags) hal_interrupt_save_interrupt_state(flags)
#define HAL_INTERRUPT_RESTORE_STATE(flags) hal_interrupt_restore_interrupt_state(flags)

// Interrupt handler macros
#define HAL_INTERRUPT_REGISTER(vector, handler) hal_interrupt_register_handler(vector, handler)
#define HAL_INTERRUPT_UNREGISTER(vector) hal_interrupt_unregister_handler(vector)
#define HAL_INTERRUPT_GET_HANDLER(vector) hal_interrupt_get_handler(vector)

// Interrupt masking macros
#define HAL_INTERRUPT_MASK(vector) hal_interrupt_mask(vector)
#define HAL_INTERRUPT_UNMASK(vector) hal_interrupt_unmask(vector)
#define HAL_INTERRUPT_MASK_ALL() hal_interrupt_mask_all()
#define HAL_INTERRUPT_UNMASK_ALL() hal_interrupt_unmask_all()

// Interrupt handling macros
#define HAL_INTERRUPT_HANDLE(vector) hal_interrupt_handle(vector)
#define HAL_INTERRUPT_HANDLE_NMI() hal_interrupt_handle_nmi()
#define HAL_INTERRUPT_HANDLE_EXCEPTION(vector, error) hal_interrupt_handle_exception(vector, error)

// Interrupt info macros
#define HAL_INTERRUPT_GET_INFO(info) hal_interrupt_get_info(info)
#define HAL_INTERRUPT_GET_TOTAL() hal_interrupt_get_total_interrupts()
#define HAL_INTERRUPT_GET_AVAILABLE() hal_interrupt_get_available_interrupts()
#define HAL_INTERRUPT_GET_MAX_PRIORITY() hal_interrupt_get_max_priority()

// Interrupt validation macros
#define HAL_INTERRUPT_VALIDATE_PARAM(param) HAL_VALIDATE_PARAM(param)
#define HAL_INTERRUPT_VALIDATE_STATE(condition) HAL_VALIDATE_STATE(condition)
