#pragma once

#include "../types.h"
#include "../config.h"

// =============================================================================
// KOS - Interrupt Management Interface
// =============================================================================

// CPU state structure (x86-64)
typedef struct kos_cpu_state {
    // General purpose registers
    u64 rax, rbx, rcx, rdx;
    u64 rbp, rsi, rdi;
    u64 r8, r9, r10, r11, r12, r13, r14, r15;
    
    // Interrupt information
    u64 vector_number;
    u64 error_code;
    
    // IRET frame
    u64 iret_rip, iret_cs, iret_flags, iret_rsp, iret_ss;
} kos_cpu_state_t;

// Interrupt handler function type
typedef kos_cpu_state_t* (*kos_interrupt_handler_t)(kos_cpu_state_t* state);

// IRQ handler function type (legacy compatibility)
typedef void (*kos_irq_handler_t)(void);

// Interrupt vector numbers
typedef enum {
    KOS_IRQ_BASE = 32,
    KOS_IRQ_TIMER = KOS_IRQ_BASE + 0,
    KOS_IRQ_KEYBOARD = KOS_IRQ_BASE + 1,
    KOS_IRQ_MAX = KOS_IRQ_BASE + 16
} kos_irq_vector_t;

// Interrupt management functions
kos_result_t kos_interrupts_init(void);
kos_result_t kos_interrupt_register_handler(u8 vector, kos_interrupt_handler_t handler);
kos_result_t kos_interrupt_unregister_handler(u8 vector);

// IRQ-specific functions
kos_result_t kos_irq_init(void);
kos_result_t kos_irq_register_handler(u8 irq, kos_irq_handler_t handler);
kos_result_t kos_irq_unregister_handler(u8 irq);
void kos_irq_enable(void);
void kos_irq_disable(void);

// CPU state access
static inline u64 kos_cpu_get_register(const kos_cpu_state_t* state, const char* reg_name);
static inline void kos_cpu_set_register(kos_cpu_state_t* state, const char* reg_name, u64 value);

// Legacy compatibility
typedef struct registers {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 int_no, err_code;
    u32 eip, cs, eflags;
} legacy_registers_t;
