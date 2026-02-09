#include "hal/hal_interface.h"
#include "hal/hal_types.h"
#include "kos/utils/string.h"
#include "debug/debug.h"

// =============================================================================
// KOS - HAL Architecture Detection
// =============================================================================

// CPUID instruction wrapper
static inline void hal_cpuid(uint32_t leaf, uint32_t subleaf, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    asm volatile("cpuid"
                 : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
                 : "a" (leaf), "c" (subleaf));
}

// Check if CPUID is supported
static bool hal_has_cpuid(void) {
    uint64_t flags;
    asm volatile("pushf\n\t"
                 "pop %0"
                 : "=r" (flags));
    
    // Copy flags and set bit 21
    uint64_t flags_copy = flags | (1ULL << 21);
    asm volatile("push %0\n\t"
                 "popf\n\t"
                 "pushf\n\t"
                 "pop %0"
                 : "=r" (flags_copy)
                 : "0" (flags_copy));
    
    // Check if bit 21 can be toggled
    return (flags_copy ^ flags) & (1ULL << 21);
}

// Get CPU vendor string
static void hal_get_cpu_vendor(char* vendor, size_t size) {
    if (!vendor || size < 13) {
        return;
    }
    
    uint32_t eax, ebx, ecx, edx;
    hal_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    
    // Copy vendor string (12 bytes + null terminator)
    vendor[0] = (ebx >> 0) & 0xFF;
    vendor[1] = (ebx >> 8) & 0xFF;
    vendor[2] = (ebx >> 16) & 0xFF;
    vendor[3] = (ebx >> 24) & 0xFF;
    vendor[4] = (edx >> 0) & 0xFF;
    vendor[5] = (edx >> 8) & 0xFF;
    vendor[6] = (edx >> 16) & 0xFF;
    vendor[7] = (edx >> 24) & 0xFF;
    vendor[8] = (ecx >> 0) & 0xFF;
    vendor[9] = (ecx >> 8) & 0xFF;
    vendor[10] = (ecx >> 16) & 0xFF;
    vendor[11] = (ecx >> 24) & 0xFF;
    vendor[12] = '\0';
}

// Check if CPU is 64-bit capable
static bool hal_is_64bit_capable(void) {
    if (!hal_has_cpuid()) {
        return false;
    }
    
    uint32_t eax, ebx, ecx, edx;
    hal_cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx);
    
    // Check if extended CPUID is supported
    if (eax < 0x80000001) {
        return false;
    }
    
    hal_cpuid(0x80000001, 0, &eax, &ebx, &ecx, &edx);
    
    // Check LM bit (bit 29 in EDX)
    return (edx & (1 << 29)) != 0;
}

// Check if running in long mode
static bool hal_is_long_mode(void) {
    // Check CS.L bit in segment descriptor
    uint32_t cs;
    asm volatile("mov %%cs, %0" : "=r" (cs));
    
    // This is a simplified check - in reality, we'd need to read the GDT
    // For now, we'll assume we're in long mode if we're compiled for x86_64
#ifdef __x86_64__
    return true;
#else
    return false;
#endif
}

// Check if virtualization is present
static bool hal_is_virtualized(void) {
    if (!hal_has_cpuid()) {
        return false;
    }
    
    uint32_t eax, ebx, ecx, edx;
    
    // Check for VMX (Intel) or SVM (AMD)
    hal_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    
    bool vmx = (ecx & (1 << 5)) != 0;  // VMX bit
    bool svm = (ecx & (1 << 2)) != 0;  // SVM bit
    
    if (vmx || svm) {
        // Check if we're running under a hypervisor
        hal_cpuid(0x40000000, 0, &eax, &ebx, &ecx, &edx);
        
        // If eax is non-zero, we're under a hypervisor
        return eax != 0;
    }
    
    return false;
}

// Get hypervisor signature
static void hal_get_hypervisor_signature(char* signature, size_t size) {
    if (!signature || size < 13) {
        return;
    }
    
    uint32_t eax, ebx, ecx, edx;
    hal_cpuid(0x40000000, 0, &eax, &ebx, &ecx, &edx);
    
    if (eax == 0) {
        kos_strcpy(signature, "None");
        return;
    }
    
    // Copy hypervisor signature
    signature[0] = (ebx >> 0) & 0xFF;
    signature[1] = (ebx >> 8) & 0xFF;
    signature[2] = (ebx >> 16) & 0xFF;
    signature[3] = (ebx >> 24) & 0xFF;
    signature[4] = (ecx >> 0) & 0xFF;
    signature[5] = (ecx >> 8) & 0xFF;
    signature[6] = (ecx >> 16) & 0xFF;
    signature[7] = (ecx >> 24) & 0xFF;
    signature[8] = (edx >> 0) & 0xFF;
    signature[9] = (edx >> 8) & 0xFF;
    signature[10] = (edx >> 16) & 0xFF;
    signature[11] = (edx >> 24) & 0xFF;
    signature[12] = '\0';
}

// Detect CPU family and model
static void hal_get_cpu_family_model(uint32_t* family, uint32_t* model) {
    if (!family || !model) {
        return;
    }
    
    if (!hal_has_cpuid()) {
        *family = 0;
        *model = 0;
        return;
    }
    
    uint32_t eax, ebx, ecx, edx;
    hal_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    
    // Extract family and model
    uint32_t base_family = (eax >> 8) & 0xF;
    uint32_t base_model = (eax >> 4) & 0xF;
    uint32_t extended_family = (eax >> 20) & 0xFF;
    uint32_t extended_model = (eax >> 16) & 0xF;
    
    *family = base_family;
    *model = base_model;
    
    if (base_family == 0xF) {
        *family += extended_family;
        *model += (extended_model << 4);
    }
}

// Get CPU stepping
static uint32_t hal_get_cpu_stepping(void) {
    if (!hal_has_cpuid()) {
        return 0;
    }
    
    uint32_t eax, ebx, ecx, edx;
    hal_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    
    return eax & 0xF;
}

// Check if CPU supports specific features
static bool hal_has_feature(uint32_t feature_bit) {
    if (!hal_has_cpuid()) {
        return false;
    }
    
    uint32_t eax, ebx, ecx, edx;
    hal_cpuid(1, 0, &eax, &ebx, &ecx, &edx);
    
    // Check standard features
    if (feature_bit < 32) {
        return (edx & (1 << feature_bit)) != 0;
    } else if (feature_bit < 64) {
        return (ecx & (1 << (feature_bit - 32))) != 0;
    }
    
    // Check extended features
    hal_cpuid(0x80000001, 0, &eax, &ebx, &ecx, &edx);
    if (feature_bit < 96) {
        return (edx & (1 << (feature_bit - 64))) != 0;
    } else if (feature_bit < 128) {
        return (ecx & (1 << (feature_bit - 96))) != 0;
    }
    
    return false;
}

// Get CPU brand string
static void hal_get_cpu_brand(char* brand, size_t size) {
    if (!brand || size < 49) {
        return;
    }
    
    if (!hal_has_cpuid()) {
        kos_strcpy(brand, "Unknown CPU");
        return;
    }
    
    uint32_t eax, ebx, ecx, edx;
    
    // Check if brand string is supported
    hal_cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx);
    if (eax < 0x80000004) {
        kos_strcpy(brand, "Unknown CPU");
        return;
    }
    
    // Get brand string parts
    hal_cpuid(0x80000002, 0, &eax, &ebx, &ecx, &edx);
    brand[0] = (eax >> 0) & 0xFF;
    brand[1] = (eax >> 8) & 0xFF;
    brand[2] = (eax >> 16) & 0xFF;
    brand[3] = (eax >> 24) & 0xFF;
    brand[4] = (ebx >> 0) & 0xFF;
    brand[5] = (ebx >> 8) & 0xFF;
    brand[6] = (ebx >> 16) & 0xFF;
    brand[7] = (ebx >> 24) & 0xFF;
    brand[8] = (ecx >> 0) & 0xFF;
    brand[9] = (ecx >> 8) & 0xFF;
    brand[10] = (ecx >> 16) & 0xFF;
    brand[11] = (ecx >> 24) & 0xFF;
    brand[12] = (edx >> 0) & 0xFF;
    brand[13] = (edx >> 8) & 0xFF;
    brand[14] = (edx >> 16) & 0xFF;
    brand[15] = (edx >> 24) & 0xFF;
    
    hal_cpuid(0x80000003, 0, &eax, &ebx, &ecx, &edx);
    brand[16] = (eax >> 0) & 0xFF;
    brand[17] = (eax >> 8) & 0xFF;
    brand[18] = (eax >> 16) & 0xFF;
    brand[19] = (eax >> 24) & 0xFF;
    brand[20] = (ebx >> 0) & 0xFF;
    brand[21] = (ebx >> 8) & 0xFF;
    brand[22] = (ebx >> 16) & 0xFF;
    brand[23] = (ebx >> 24) & 0xFF;
    brand[24] = (ecx >> 0) & 0xFF;
    brand[25] = (ecx >> 8) & 0xFF;
    brand[26] = (ecx >> 16) & 0xFF;
    brand[27] = (ecx >> 24) & 0xFF;
    brand[28] = (edx >> 0) & 0xFF;
    brand[29] = (edx >> 8) & 0xFF;
    brand[30] = (edx >> 16) & 0xFF;
    brand[31] = (edx >> 24) & 0xFF;
    
    hal_cpuid(0x80000004, 0, &eax, &ebx, &ecx, &edx);
    brand[32] = (eax >> 0) & 0xFF;
    brand[33] = (eax >> 8) & 0xFF;
    brand[34] = (eax >> 16) & 0xFF;
    brand[35] = (eax >> 24) & 0xFF;
    brand[36] = (ebx >> 0) & 0xFF;
    brand[37] = (ebx >> 8) & 0xFF;
    brand[38] = (ebx >> 16) & 0xFF;
    brand[39] = (ebx >> 24) & 0xFF;
    brand[40] = (ecx >> 0) & 0xFF;
    brand[41] = (ecx >> 8) & 0xFF;
    brand[42] = (ecx >> 16) & 0xFF;
    brand[43] = (ecx >> 24) & 0xFF;
    brand[44] = (edx >> 0) & 0xFF;
    brand[45] = (edx >> 8) & 0xFF;
    brand[46] = (edx >> 16) & 0xFF;
    brand[47] = (edx >> 24) & 0xFF;
    brand[48] = '\0';
    
    // Trim trailing spaces
    for (int i = 47; i >= 0 && brand[i] == ' '; i--) {
        brand[i] = '\0';
    }
}

// Main architecture detection function
hal_architecture_t hal_detect_architecture_detailed(void) {
    char vendor[13];
    hal_get_cpu_vendor(vendor, sizeof(vendor));
    
    log_debug("CPU Vendor: %s", vendor);
    
    // Check if it's an x86 CPU
    if (kos_strcmp(vendor, "GenuineIntel") == 0 || 
        kos_strcmp(vendor, "AuthenticAMD") == 0 ||
        kos_strcmp(vendor, "CentaurHauls") == 0 ||
        kos_strcmp(vendor, "GenuineTMx86") == 0) {
        
        // It's an x86 CPU, check if it's 64-bit
        if (hal_is_64bit_capable()) {
            log_debug("64-bit capable CPU detected");
            
            // Check if we're actually running in 64-bit mode
            if (hal_is_long_mode()) {
                log_debug("Running in long mode (64-bit)");
                return HAL_ARCH_X86_64;
            } else {
                log_debug("64-bit capable but running in 32-bit mode");
                return HAL_ARCH_I386;
            }
        } else {
            log_debug("32-bit only CPU detected");
            return HAL_ARCH_I386;
        }
    }
    
    // Check for ARM CPUs
    if (kos_strcmp(vendor, "ARM") == 0) {
        log_debug("ARM CPU detected");
        // TODO: Implement ARM detection
        return HAL_ARCH_UNKNOWN;
    }
    
    // Check for RISC-V CPUs
    if (kos_strcmp(vendor, "RISC-V") == 0) {
        log_debug("RISC-V CPU detected");
        // TODO: Implement RISC-V detection
        return HAL_ARCH_UNKNOWN;
    }
    
    log_debug("Unknown CPU vendor: %s", vendor);
    return HAL_ARCH_UNKNOWN;
}

// Get detailed CPU information
void hal_get_detailed_cpu_info(hal_cpu_info_t* info) {
    if (!info) {
        return;
    }
    
    kos_memset(info, 0, sizeof(hal_cpu_info_t));
    
    // Get vendor
    hal_get_cpu_vendor((char*)info->vendor, sizeof(info->vendor));
    
    // Get brand string
    hal_get_cpu_brand((char*)info->model, sizeof(info->model));
    
    // Get family and model
    hal_get_cpu_family_model((uint32_t*)&info->family, (uint32_t*)&info->model);
    
    // Get stepping
    info->stepping = hal_get_cpu_stepping();
    
    // Check features
    info->has_fpu = hal_has_feature(0);     // FPU
    info->has_mmx = hal_has_feature(23);    // MMX
    info->has_sse = hal_has_feature(25);    // SSE
    info->has_sse2 = hal_has_feature(26);   // SSE2
    info->has_sse3 = hal_has_feature(32);   // SSE3
    info->has_sse4_1 = hal_has_feature(51); // SSE4.1
    info->has_sse4_2 = hal_has_feature(52); // SSE4.2
    info->has_avx = hal_has_feature(58);    // AVX
    info->has_avx2 = hal_has_feature(87);   // AVX2
    
    // Build features bitmask
    if (info->has_fpu) info->features |= HAL_CPU_FEATURE_FPU;
    if (info->has_mmx) info->features |= HAL_CPU_FEATURE_MMX;
    if (info->has_sse) info->features |= HAL_CPU_FEATURE_SSE;
    if (info->has_sse2) info->features |= HAL_CPU_FEATURE_SSE2;
    if (info->has_sse3) info->features |= HAL_CPU_FEATURE_SSE3;
    if (info->has_sse4_1) info->features |= HAL_CPU_FEATURE_SSE4_1;
    if (info->has_sse4_2) info->features |= HAL_CPU_FEATURE_SSE4_2;
    if (info->has_avx) info->features |= HAL_CPU_FEATURE_AVX;
    if (info->has_avx2) info->features |= HAL_CPU_FEATURE_AVX2;
    
    // TODO: Get actual frequency, cores, threads from CPUID or other methods
    info->frequency = 0;  // Will be set by architecture-specific code
    info->cores = 1;      // Will be set by architecture-specific code
    info->threads = 1;   // Will be set by architecture-specific code
}

// Check if we're running under a specific hypervisor
bool hal_is_hypervisor(const char* hypervisor_name) {
    if (!hypervisor_name) {
        return false;
    }
    
    char signature[13];
    hal_get_hypervisor_signature(signature, sizeof(signature));
    
    return kos_strcmp(signature, hypervisor_name) == 0;
}

// Get hypervisor information
void hal_get_hypervisor_info(hal_hypervisor_info_t* info) {
    if (!info) {
        return;
    }
    
    kos_memset(info, 0, sizeof(hal_hypervisor_info_t));
    
    if (hal_is_virtualized()) {
        hal_get_hypervisor_signature((char*)info->name, sizeof(info->name));
        info->present = true;
        
        // TODO: Get hypervisor version and features
        info->version = 0;
        info->features = 0;
    } else {
        info->present = false;
        kos_strcpy((char*)info->name, "None");
    }
}

// Validate CPUID support
bool hal_validate_cpuid_support(void) {
    return hal_has_cpuid();
}

// Get CPUID maximum basic leaf
uint32_t hal_get_cpuid_max_basic_leaf(void) {
    if (!hal_has_cpuid()) {
        return 0;
    }
    
    uint32_t eax, ebx, ecx, edx;
    hal_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    
    return eax;
}

// Get CPUID maximum extended leaf
uint32_t hal_get_cpuid_max_extended_leaf(void) {
    if (!hal_has_cpuid()) {
        return 0;
    }
    
    uint32_t eax, ebx, ecx, edx;
    hal_cpuid(0x80000000, 0, &eax, &ebx, &ecx, &edx);
    
    return eax;
}

// Check if specific CPUID leaf is supported
bool hal_is_cpuid_leaf_supported(uint32_t leaf) {
    if (!hal_has_cpuid()) {
        return false;
    }
    
    if (leaf < 0x80000000) {
        return leaf <= hal_get_cpuid_max_basic_leaf();
    } else {
        return leaf <= hal_get_cpuid_max_extended_leaf();
    }
}

// Dump CPUID information for debugging
void hal_dump_cpuid_info(void) {
    if (!hal_has_cpuid()) {
        log_error("CPUID not supported");
        return;
    }
    
    uint32_t max_basic = hal_get_cpuid_max_basic_leaf();
    uint32_t max_extended = hal_get_cpuid_max_extended_leaf();
    
    log_info("=== CPUID Information ===");
    log_info("Max Basic Leaf: 0x%08x", max_basic);
    log_info("Max Extended Leaf: 0x%08x", max_extended);
    
    // Dump basic CPUID leaves
    for (uint32_t leaf = 0; leaf <= max_basic && leaf < 10; leaf++) {
        uint32_t eax, ebx, ecx, edx;
        hal_cpuid(leaf, 0, &eax, &ebx, &ecx, &edx);
        log_info("CPUID 0x%08x: EAX=0x%08x EBX=0x%08x ECX=0x%08x EDX=0x%08x", leaf, eax, ebx, ecx, edx);
    }
    
    // Dump extended CPUID leaves
    for (uint32_t leaf = 0x80000000; leaf <= max_extended && leaf < 0x80000005; leaf++) {
        uint32_t eax, ebx, ecx, edx;
        hal_cpuid(leaf, 0, &eax, &ebx, &ecx, &edx);
        log_info("CPUID 0x%08x: EAX=0x%08x EBX=0x%08x ECX=0x%08x EDX=0x%08x", leaf, eax, ebx, ecx, edx);
    }
}
