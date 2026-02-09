#pragma once

#include "hal_types.h"
#include "hal_interface.h"

// =============================================================================
// KOS - HAL Interrupt Operations Interface (Simple)
// =============================================================================

// Interrupt operations interface structure
struct hal_interrupt_ops {
    // Initialization and cleanup
    hal_result_t (*init)(void);
    hal_result_t (*shutdown)(void);
    hal_result_t (*reset)(void);
    hal_result_t (*cleanup)(void);
    
    // Interrupt information
    hal_result_t (*get_info)(hal_interrupt_info_t* info);
    
    // Interrupt control
    hal_result_t (*enable)(void);
    hal_result_t (*disable)(void);
    hal_result_t (*enable_irq)(uint32_t irq);
    hal_result_t (*disable_irq)(uint32_t irq);
    hal_result_t (*mask_irq)(uint32_t irq);
    hal_result_t (*unmask_irq)(uint32_t irq);
    
    // Interrupt handlers
    hal_result_t (*register_handler)(uint32_t vector, hal_interrupt_handler_t handler);
    hal_result_t (*unregister_handler)(uint32_t vector);
    hal_result_t (*set_priority)(uint32_t vector, hal_u8_t priority);
    hal_result_t (*get_priority)(uint32_t vector, hal_u8_t* priority);
    
    // Interrupt statistics
    hal_result_t (*get_stats)(hal_interrupt_stats_t* stats);
    hal_result_t (*reset_stats)(void);
    hal_result_t (*dump_stats)(void);
};

// Interrupt operation convenience functions
hal_result_t hal_interrupt_init(void);
hal_result_t hal_interrupt_shutdown(void);
hal_result_t hal_interrupt_reset(void);
hal_result_t hal_interrupt_enable(void);
hal_result_t hal_interrupt_disable(void);
hal_result_t hal_interrupt_enable_irq(uint32_t irq);
hal_result_t hal_interrupt_disable_irq(uint32_t irq);
hal_result_t hal_interrupt_mask_irq(uint32_t irq);
hal_result_t hal_interrupt_unmask_irq(uint32_t irq);
hal_result_t hal_interrupt_get_info(hal_interrupt_info_t* info);
hal_result_t hal_interrupt_get_stats(hal_interrupt_stats_t* stats);
hal_result_t hal_interrupt_reset_stats(void);
hal_result_t hal_interrupt_dump_stats(void);
