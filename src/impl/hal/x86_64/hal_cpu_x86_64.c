#include "hal/hal_interface.h"
#include "hal/hal_types.h"
#include "hal/hal_cpu.h"
#include "kos/utils/string.h"
#include "debug/debug.h"

// =============================================================================
// KOS - HAL CPU Operations x86_64 Implementation
// =============================================================================

// Forward declarations
extern struct memory_manager g_memory_manager;

// x86_64-specific CPU information
typedef struct {
    hal_u64_t tsc_frequency;
    hal_u32_t apic_id;
    hal_u32_t acpi_id;
    hal_u32_t local_apic_version;
    hal_u32_t logical_processor_count;
    hal_u32_t core_count;
    hal_u32_t thread_count;
    hal_u32_t cache_line_size;
    hal_u32_t l1_dcache_size;
    hal_u32_t l1_icache_size;
    hal_u32_t l2_cache_size;
    hal_u32_t l3_cache_size;
    hal_u32_t physical_address_width;
    hal_u32_t linear_address_width;
    hal_u32_t cache_hierarchy;
    hal_bool_t hyperthreading_enabled;
    hal_u64_t msr_platform_info;
    hal_u64_t msr_tsc_aux;
} hal_x86_64_cpu_info_t;

// Global x86_64 CPU state
static hal_x86_64_cpu_info_t g_x86_64_cpu_info = {0};
static hal_cpu_info_t g_cpu_info = {0};
static bool g_cpu_initialized = false;

// x86_64 CPU operations implementation
static hal_result_t hal_x86_64_cpu_init(void);
static hal_result_t hal_x86_64_cpu_shutdown(void);
static hal_result_t hal_x86_64_cpu_reset(void);
static hal_result_t hal_x86_64_cpu_cleanup(void);
static hal_result_t hal_x86_64_cpu_get_info(hal_cpu_info_t* info);
static hal_result_t hal_x86_64_cpu_get_features(hal_cpu_features_t* features);
static hal_result_t hal_x86_64_cpu_halt(void);
static hal_result_t hal_x86_64_cpu_get_frequency(hal_frequency_t* frequency);
static hal_result_t hal_x86_64_cpu_save_context(hal_cpu_context_t* context);
static hal_result_t hal_x86_64_cpu_restore_context(const hal_cpu_context_t* context);
static hal_result_t hal_x86_64_cpu_enable_interrupts(void);
static hal_result_t hal_x86_64_cpu_disable_interrupts(void);
static hal_u32_t hal_x86_64_cpu_get_interrupt_state(void);
static hal_result_t hal_x86_64_cpu_get_cycle_count(hal_u64_t* count);
static hal_result_t hal_x86_64_cpu_get_timestamp(hal_timestamp_t* timestamp);
static hal_result_t hal_x86_64_cpu_get_tsc_frequency(hal_frequency_t* frequency);
static hal_result_t hal_x86_64_cpu_get_cpu_count(hal_u32_t* count);
static hal_result_t hal_x86_64_cpu_get_cpu_id(hal_u32_t* id);

// x86_64 CPU operations structure
static hal_cpu_ops_t hal_x86_64_cpu_ops = {
    .init = hal_x86_64_cpu_init,
    .shutdown = hal_x86_64_cpu_shutdown,
    .reset = hal_x86_64_cpu_reset,
    .cleanup = hal_x86_64_cpu_cleanup,
    .get_info = hal_x86_64_cpu_get_info,
    .get_features = hal_x86_64_cpu_get_features,
    .halt = hal_x86_64_cpu_halt,
    .get_frequency = hal_x86_64_cpu_get_frequency,
    .save_context = hal_x86_64_cpu_save_context,
    .restore_context = hal_x86_64_cpu_restore_context,
    .enable_interrupts = hal_x86_64_cpu_enable_interrupts,
    .disable_interrupts = hal_x86_64_cpu_disable_interrupts,
    .get_interrupt_state = hal_x86_64_cpu_get_interrupt_state,
    .get_cycle_count = hal_x86_64_cpu_get_cycle_count,
    .get_timestamp = hal_x86_64_cpu_get_timestamp,
    .get_tsc_frequency = hal_x86_64_cpu_get_tsc_frequency,
    .get_cpu_count = hal_x86_64_cpu_get_cpu_count,
    .get_cpu_id = hal_x86_64_cpu_get_cpu_id
};

// Get x86_64 CPU interface
hal_interface_t* hal_x86_64_get_interface(void) {
    static hal_interface_t interface = {0};
    
    interface.arch_name = "x86_64";
    interface.arch_bits = 64;
    interface.architecture = HAL_ARCH_X86_64;
    interface.cpu_ops = &hal_x86_64_cpu_ops;
    
    // Get other interfaces
    extern hal_interface_t* hal_x86_64_get_memory_interface(void);
    extern hal_interface_t* hal_x86_64_get_interrupt_interface(void);
    extern hal_interface_t* hal_x86_64_get_timer_interface(void);
    extern hal_interface_t* hal_x86_64_get_platform_interface(void);
    
    hal_interface_t* mem_interface = hal_x86_64_get_memory_interface();
    hal_interface_t* int_interface = hal_x86_64_get_interrupt_interface();
    hal_interface_t* timer_interface = hal_x86_64_get_timer_interface();
    hal_interface_t* platform_interface = hal_x86_64_get_platform_interface();
    
    if (mem_interface) interface.memory_ops = mem_interface->memory_ops;
    if (int_interface) interface.interrupt_ops = int_interface->interrupt_ops;
    if (timer_interface) interface.timer_ops = timer_interface->timer_ops;
    if (platform_interface) interface.platform_ops = platform_interface->platform_ops;
    
    return &interface;
}

// CPUID wrapper for x86_64
static inline void hal_x86_64_cpuid(hal_u32_t leaf, hal_u32_t subleaf, hal_u32_t* eax, hal_u32_t* ebx, hal_u32_t* ecx, hal_u32_t* edx) {
    asm volatile("cpuid"
                 : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
                 : "a" (leaf), "c" (subleaf));
}

// Read MSR
static inline hal_u64_t hal_x86_64_read_msr(hal_u32_t msr) {
    hal_u32_t low, high;
    asm volatile("rdmsr" : "=a" (low), "=d" (high) : "c" (msr));
    return ((hal_u64_t)high << 32) | low;
}

// Write MSR
static inline void hal_x86_64_write_msr(hal_u32_t msr, hal_u64_t value) {
    hal_u32_t low = value & 0xFFFFFFFF;
    hal_u32_t high = (value >> 32) & 0xFFFFFFFF;
    asm volatile("wrmsr" : : "c" (msr), "a" (low), "d" (high));
}

// Read control register
static inline hal_u64_t hal_x86_64_read_cr(hal_u32_t cr) {
    hal_u64_t value;
    switch (cr) {
        case 0:
            asm volatile("mov %%cr0, %0" : "=r" (value));
            break;
        case 2:
            asm volatile("mov %%cr2, %0" : "=r" (value));
            break;
        case 3:
            asm volatile("mov %%cr3, %0" : "=r" (value));
            break;
        case 4:
            asm volatile("mov %%cr4, %0" : "=r" (value));
            break;
        default:
            value = 0;
            break;
    }
    return value;
}

// Write control register
static inline void hal_x86_64_write_cr(hal_u32_t cr, hal_u64_t value) {
    switch (cr) {
        case 0:
            asm volatile("mov %0, %%cr0" : : "r" (value));
            break;
        case 2:
            asm volatile("mov %0, %%cr2" : : "r" (value));
            break;
        case 3:
            asm volatile("mov %0, %%cr3" : : "r" (value));
            break;
        case 4:
            asm volatile("mov %0, %%cr4" : : "r" (value));
            break;
    }
}

// Get APIC ID
static hal_u32_t hal_x86_64_get_apic_id(void) {
    return (hal_x86_64_read_msr(0x1B) >> 24) & 0xFF;
}

// Get CPU topology
static void hal_x86_64_get_cpu_topology(void) {
    hal_u32_t eax, ebx, ecx, edx;
    
    // Check if extended topology is supported
    hal_x86_64_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    if (eax >= 0xB) {
        // Use extended topology leaf
        hal_x86_64_cpuid(0xB, 0, &eax, &ebx, &ecx, &edx);
        g_x86_64_cpu_info.logical_processor_count = ebx & 0xFFFF;
        g_x86_64_cpu_info.core_count = (ebx >> 16) & 0xFFFF;
        g_x86_64_cpu_info.thread_count = ecx & 0xFFFF;
        g_x86_64_cpu_info.apic_id = edx & 0xFFFFFFFF;
    } else {
        // Use legacy method
        hal_x86_64_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
        g_x86_64_cpu_info.logical_processor_count = (ebx >> 16) & 0xFF;
        g_x86_64_cpu_info.core_count = g_x86_64_cpu_info.logical_processor_count;
        g_x86_64_cpu_info.thread_count = g_x86_64_cpu_info.logical_processor_count;
        g_x86_64_cpu_info.apic_id = hal_x86_64_get_apic_id();
    }
    
    // Check for hyperthreading
    g_x86_64_cpu_info.hyperthreading_enabled = (ecx & (1 << 28)) != 0;
}

// Get cache information
static void hal_x86_64_get_cache_info(void) {
    hal_u32_t eax, ebx, ecx, edx;
    
    // Check if cache information is available
    hal_x86_64_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    if (eax >= 4) {
        // Get cache line size
        hal_x86_64_cpuid(4, 0, &eax, &ebx, &ecx, &edx);
        g_x86_64_cpu_info.cache_line_size = ((ebx & 0xFFF) + 1);
        
        // Get L1 data cache
        hal_x86_64_cpuid(4, 1, &eax, &ebx, &ecx, &edx);
        if ((eax & 0x1F) == 1) { // Data cache
            g_x86_64_cpu_info.l1_dcache_size = ((ebx & 0xFFF) + 1) * ((ebx >> 12) & 0x3FF) + 1;
        }
        
        // Get L1 instruction cache
        hal_x86_64_cpuid(4, 2, &eax, &ebx, &ecx, &edx);
        if ((eax & 0x1F) == 2) { // Instruction cache
            g_x86_64_cpu_info.l1_icache_size = ((ebx & 0xFFF) + 1) * ((ebx >> 12) & 0x3FF) + 1;
        }
        
        // Get L2 cache
        hal_x86_64_cpuid(4, 3, &eax, &ebx, &ecx, &edx);
        if ((eax & 0x1F) == 3) { // Unified cache
            g_x86_64_cpu_info.l2_cache_size = ((ebx & 0xFFF) + 1) * ((ebx >> 12) & 0x3FF) + 1;
        }
        
        // Get L3 cache
        hal_x86_64_cpuid(4, 4, &eax, &ebx, &ecx, &edx);
        if ((eax & 0x1F) == 4) { // Unified cache
            g_x86_64_cpu_info.l3_cache_size = ((ebx & 0xFFF) + 1) * ((ebx >> 12) & 0x3FF) + 1;
        }
    }
}

// Get address widths
static void hal_x86_64_get_address_widths(void) {
    hal_u32_t eax, ebx, ecx, edx;
    
    hal_x86_64_cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx);
    if (eax >= 0x80000008) {
        hal_x86_64_cpuid(0x80000008, 0, &eax, &ebx, &ecx, &edx);
        g_x86_64_cpu_info.physical_address_width = eax & 0xFF;
        g_x86_64_cpu_info.linear_address_width = (eax >> 8) & 0xFF;
    } else {
        g_x86_64_cpu_info.physical_address_width = 36;
        g_x86_64_cpu_info.linear_address_width = 48;
    }
}

// Get TSC frequency
static void hal_x86_64_get_tsc_frequency(void) {
    // Try to get TSC frequency from MSR
    g_x86_64_cpu_info.msr_platform_info = hal_x86_64_read_msr(0xCE);
    
    // Extract TSC frequency from platform info (if available)
    hal_u64_t ratio = (g_x86_64_cpu_info.msr_platform_info >> 8) & 0xFF;
    if (ratio != 0) {
        // Assume base frequency of 100 MHz (this is a simplification)
        g_x86_64_cpu_info.tsc_frequency = ratio * 100000000ULL;
    } else {
        // Use a reasonable default
        g_x86_64_cpu_info.tsc_frequency = 2500000000ULL; // 2.5 GHz
    }
}

// CPU initialization
static hal_result_t hal_x86_64_cpu_init(void) {
    if (g_cpu_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    log_info("Initializing x86_64 CPU operations");
    
    // Get CPU topology
    hal_x86_64_get_cpu_topology();
    
    // Get cache information
    hal_x86_64_get_cache_info();
    
    // Get address widths
    hal_x86_64_get_address_widths();
    
    // Get TSC frequency
    hal_x86_64_get_tsc_frequency();
    
    // Initialize CPU info structure
    kos_memset(&g_cpu_info, 0, sizeof(hal_cpu_info_t));
    
    // Get basic CPU information
    hal_u32_t eax, ebx, ecx, edx;
    hal_x86_64_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    
    // Get vendor string
    g_cpu_info.vendor[0] = (ebx >> 0) & 0xFF;
    g_cpu_info.vendor[1] = (ebx >> 8) & 0xFF;
    g_cpu_info.vendor[2] = (ebx >> 16) & 0xFF;
    g_cpu_info.vendor[3] = (ebx >> 24) & 0xFF;
    g_cpu_info.vendor[4] = (edx >> 0) & 0xFF;
    g_cpu_info.vendor[5] = (edx >> 8) & 0xFF;
    g_cpu_info.vendor[6] = (edx >> 16) & 0xFF;
    g_cpu_info.vendor[7] = (edx >> 24) & 0xFF;
    g_cpu_info.vendor[8] = (ecx >> 0) & 0xFF;
    g_cpu_info.vendor[9] = (ecx >> 8) & 0xFF;
    g_cpu_info.vendor[10] = (ecx >> 16) & 0xFF;
    g_cpu_info.vendor[11] = (ecx >> 24) & 0xFF;
    g_cpu_info.vendor[12] = '\0';
    
    // Get brand string
    hal_x86_64_cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx);
    if (eax >= 0x80000004) {
        hal_x86_64_cpuid(0x80000002, 0, &eax, &ebx, &ecx, &edx);
        kos_memcpy(g_cpu_info.model, &eax, 4);
        kos_memcpy(g_cpu_info.model + 4, &ebx, 4);
        kos_memcpy(g_cpu_info.model + 8, &ecx, 4);
        kos_memcpy(g_cpu_info.model + 12, &edx, 4);
        
        hal_x86_64_cpuid(0x80000003, 0, &eax, &ebx, &ecx, &edx);
        kos_memcpy(g_cpu_info.model + 16, &eax, 4);
        kos_memcpy(g_cpu_info.model + 20, &ebx, 4);
        kos_memcpy(g_cpu_info.model + 24, &ecx, 4);
        kos_memcpy(g_cpu_info.model + 28, &edx, 4);
        
        hal_x86_64_cpuid(0x80000004, 0, &eax, &ebx, &ecx, &edx);
        kos_memcpy(g_cpu_info.model + 32, &eax, 4);
        kos_memcpy(g_cpu_info.model + 36, &ebx, 4);
        kos_memcpy(g_cpu_info.model + 40, &ecx, 4);
        kos_memcpy(g_cpu_info.model + 44, &edx, 4);
        g_cpu_info.model[48] = '\0';
        
        // Trim trailing spaces
        for (int i = 47; i >= 0 && g_cpu_info.model[i] == ' '; i--) {
            g_cpu_info.model[i] = '\0';
        }
    }
    
    // Get family and model
    hal_x86_64_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    hal_u32_t base_family = (eax >> 8) & 0xF;
    hal_u32_t base_model = (eax >> 4) & 0xF;
    hal_u32_t extended_family = (eax >> 20) & 0xFF;
    hal_u32_t extended_model = (eax >> 16) & 0xF;
    
    // Convert family and model to strings
    if (base_family == 0xF) {
        kos_strcpy(g_cpu_info.family, "Extended");
        kos_strcpy(g_cpu_info.model, "Extended");
    } else {
        kos_strcpy(g_cpu_info.family, "Standard");
        kos_strcpy(g_cpu_info.model, "Standard");
    }
    
    // Set cores and threads
    g_cpu_info.cores = g_x86_64_cpu_info.core_count;
    g_cpu_info.threads = g_x86_64_cpu_info.thread_count;
    
    // Set frequency (this is a simplified approach)
    g_cpu_info.frequency = g_x86_64_cpu_info.tsc_frequency / 1000000; // Convert to MHz
    
    // Check features
    g_cpu_info.has_fpu = (edx & (1 << 0)) != 0;
    g_cpu_info.has_mmx = (edx & (1 << 23)) != 0;
    g_cpu_info.has_sse = (edx & (1 << 25)) != 0;
    g_cpu_info.has_sse2 = (edx & (1 << 26)) != 0;
    g_cpu_info.has_sse3 = (ecx & (1 << 0)) != 0;
    g_cpu_info.has_sse4_1 = (ecx & (1 << 19)) != 0;
    g_cpu_info.has_sse4_2 = (ecx & (1 << 20)) != 0;
    g_cpu_info.has_avx = (ecx & (1 << 28)) != 0;
    g_cpu_info.has_avx2 = (ebx & (1 << 5)) != 0;
    
    // Build features bitmask
    if (g_cpu_info.has_fpu) g_cpu_info.features |= HAL_CPU_FEATURE_FPU;
    if (g_cpu_info.has_mmx) g_cpu_info.features |= HAL_CPU_FEATURE_MMX;
    if (g_cpu_info.has_sse) g_cpu_info.features |= HAL_CPU_FEATURE_SSE;
    if (g_cpu_info.has_sse2) g_cpu_info.features |= HAL_CPU_FEATURE_SSE2;
    if (g_cpu_info.has_sse3) g_cpu_info.features |= HAL_CPU_FEATURE_SSE3;
    if (g_cpu_info.has_sse4_1) g_cpu_info.features |= HAL_CPU_FEATURE_SSE4_1;
    if (g_cpu_info.has_sse4_2) g_cpu_info.features |= HAL_CPU_FEATURE_SSE4_2;
    if (g_cpu_info.has_avx) g_cpu_info.features |= HAL_CPU_FEATURE_AVX;
    if (g_cpu_info.has_avx2) g_cpu_info.features |= HAL_CPU_FEATURE_AVX2;
    
    g_cpu_initialized = true;
    
    log_info("x86_64 CPU operations initialized");
    log_info("CPU: %s", g_cpu_info.model);
    log_info("Cores: %u, Threads: %u", g_cpu_info.cores, g_cpu_info.threads);
    log_info("Frequency: %u MHz", g_cpu_info.frequency);
    log_info("TSC Frequency: %llu Hz", g_x86_64_cpu_info.tsc_frequency);
    
    return HAL_SUCCESS;
}

// CPU shutdown
static hal_result_t hal_x86_64_cpu_shutdown(void) {
    if (!g_cpu_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    g_cpu_initialized = false;
    log_info("x86_64 CPU operations shutdown");
    return HAL_SUCCESS;
}

// CPU reset
static hal_result_t hal_x86_64_cpu_reset(void) {
    if (!g_cpu_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    // Reset CPU state
    hal_x86_64_cpu_shutdown();
    return hal_x86_64_cpu_init();
}

// CPU cleanup
static hal_result_t hal_x86_64_cpu_cleanup(void) {
    return hal_x86_64_cpu_shutdown();
}

// Get CPU information
static hal_result_t hal_x86_64_cpu_get_info(hal_cpu_info_t* info) {
    if (!g_cpu_initialized || !info) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *info = g_cpu_info;
    return HAL_SUCCESS;
}

// Get CPU features
static hal_result_t hal_x86_64_cpu_get_features(hal_cpu_features_t* features) {
    if (!g_cpu_initialized || !features) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *features = g_cpu_info.features;
    return HAL_SUCCESS;
}

// CPU halt
static hal_result_t hal_x86_64_cpu_halt(void) {
    if (!g_cpu_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    asm volatile("hlt");
    return HAL_SUCCESS;
}

// Get CPU frequency
static hal_result_t hal_x86_64_cpu_get_frequency(hal_frequency_t* frequency) {
    if (!g_cpu_initialized || !frequency) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *frequency = g_cpu_info.frequency;
    return HAL_SUCCESS;
}

// Save CPU context
static hal_result_t hal_x86_64_cpu_save_context(hal_cpu_context_t* context) {
    if (!g_cpu_initialized || !context) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Save general purpose registers
    asm volatile("mov %%rax, %0" : "=m" (context->rax));
    asm volatile("mov %%rbx, %0" : "=m" (context->rbx));
    asm volatile("mov %%rcx, %0" : "=m" (context->rcx));
    asm volatile("mov %%rdx, %0" : "=m" (context->rdx));
    asm volatile("mov %%rsi, %0" : "=m" (context->rsi));
    asm volatile("mov %%rdi, %0" : "=m" (context->rdi));
    asm volatile("mov %%rbp, %0" : "=m" (context->rbp));
    asm volatile("mov %%rsp, %0" : "=m" (context->rsp));
    
    // Save extended registers
    asm volatile("mov %%r8, %0" : "=m" (context->r8));
    asm volatile("mov %%r9, %0" : "=m" (context->r9));
    asm volatile("mov %%r10, %0" : "=m" (context->r10));
    asm volatile("mov %%r11, %0" : "=m" (context->r11));
    asm volatile("mov %%r12, %0" : "=m" (context->r12));
    asm volatile("mov %%r13, %0" : "=m" (context->r13));
    asm volatile("mov %%r14, %0" : "=m" (context->r14));
    asm volatile("mov %%r15, %0" : "=m" (context->r15));
    
    // Save control registers
    context->cr0 = hal_x86_64_read_cr(0);
    context->cr2 = hal_x86_64_read_cr(2);
    context->cr3 = hal_x86_64_read_cr(3);
    context->cr4 = hal_x86_64_read_cr(4);
    
    // Save flags
    asm volatile("pushf; pop %0" : "=m" (context->rflags));
    
    return HAL_SUCCESS;
}

// Restore CPU context
static hal_result_t hal_x86_64_cpu_restore_context(const hal_cpu_context_t* context) {
    if (!g_cpu_initialized || !context) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Restore control registers
    hal_x86_64_write_cr(0, context->cr0);
    hal_x86_64_write_cr(2, context->cr2);
    hal_x86_64_write_cr(3, context->cr3);
    hal_x86_64_write_cr(4, context->cr4);
    
    // Restore general purpose registers
    asm volatile("mov %0, %%rax" : : "m" (context->rax));
    asm volatile("mov %0, %%rbx" : : "m" (context->rbx));
    asm volatile("mov %0, %%rcx" : : "m" (context->rcx));
    asm volatile("mov %0, %%rdx" : : "m" (context->rdx));
    asm volatile("mov %0, %%rsi" : : "m" (context->rsi));
    asm volatile("mov %0, %%rdi" : : "m" (context->rdi));
    asm volatile("mov %0, %%rbp" : : "m" (context->rbp));
    asm volatile("mov %0, %%rsp" : : "m" (context->rsp));
    
    // Restore extended registers
    asm volatile("mov %0, %%r8" : : "m" (context->r8));
    asm volatile("mov %0, %%r9" : : "m" (context->r9));
    asm volatile("mov %0, %%r10" : : "m" (context->r10));
    asm volatile("mov %0, %%r11" : : "m" (context->r11));
    asm volatile("mov %0, %%r12" : : "m" (context->r12));
    asm volatile("mov %0, %%r13" : : "m" (context->r13));
    asm volatile("mov %0, %%r14" : : "m" (context->r14));
    asm volatile("mov %0, %%r15" : : "m" (context->r15));
    
    // Restore flags
    asm volatile("push %0; popf" : : "m" (context->rflags));
    
    return HAL_SUCCESS;
}

// Enable interrupts
static hal_result_t hal_x86_64_cpu_enable_interrupts(void) {
    if (!g_cpu_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    asm volatile("sti");
    return HAL_SUCCESS;
}

// Disable interrupts
static hal_result_t hal_x86_64_cpu_disable_interrupts(void) {
    if (!g_cpu_initialized) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    asm volatile("cli");
    return HAL_SUCCESS;
}

// Get interrupt state
static hal_u32_t hal_x86_64_cpu_get_interrupt_state(void) {
    if (!g_cpu_initialized) {
        return 0;
    }
    
    hal_u64_t flags;
    asm volatile("pushf; pop %0" : "=r" (flags));
    return (flags & (1 << 9)) ? 1 : 0; // IF flag is bit 9
}

// Get cycle count
static hal_result_t hal_x86_64_cpu_get_cycle_count(hal_u64_t* count) {
    if (!g_cpu_initialized || !count) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    asm volatile("rdtsc" : "=A" (*count));
    return HAL_SUCCESS;
}

// Get timestamp
static hal_result_t hal_x86_64_cpu_get_timestamp(hal_timestamp_t* timestamp) {
    if (!g_cpu_initialized || !timestamp) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    asm volatile("rdtsc" : "=A" (*timestamp));
    return HAL_SUCCESS;
}

// Get TSC frequency
static hal_result_t hal_x86_64_cpu_get_tsc_frequency(hal_frequency_t* frequency) {
    if (!g_cpu_initialized || !frequency) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *frequency = g_x86_64_cpu_info.tsc_frequency / 1000000; // Convert to MHz
    return HAL_SUCCESS;
}

// Get CPU count
static hal_result_t hal_x86_64_cpu_get_cpu_count(hal_u32_t* count) {
    if (!g_cpu_initialized || !count) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *count = g_x86_64_cpu_info.logical_processor_count;
    return HAL_SUCCESS;
}

// Get CPU ID
static hal_result_t hal_x86_64_cpu_get_cpu_id(hal_u32_t* id) {
    if (!g_cpu_initialized || !id) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *id = g_x86_64_cpu_info.apic_id;
    return HAL_SUCCESS;
}
