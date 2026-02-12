#include "kos/process/process.h"
#include "kos/utils/log_stubs.h"
#include "kos/utils/hal_utils.h"
#include "hal/hal_interface_clean.h"

// =============================================================================
// KOS - Process Manager Implementation (HAL-based)
// =============================================================================

// Global process manager instance
static kos_process_manager_t g_process_manager;
static hal_bool_t g_process_manager_initialized = false;

// Process table for quick PID lookup
static kos_process_t* g_process_table[KOS_CONFIG_MAX_PROCESSES];

// =============================================================================
// Process Manager Internal Functions
// =============================================================================

static uint32_t allocate_pid(void) {
    uint32_t pid = g_process_manager.table.next_pid;
    
    // Find next available PID
    do {
        pid = (pid + 1) % KOS_CONFIG_MAX_PROCESSES;
        if (pid == 0) pid = 1; // Skip PID 0 (idle)
        
        if (g_process_table[pid] == NULL) {
            g_process_manager.table.next_pid = pid + 1;
            return pid;
        }
    } while (pid != g_process_manager.table.next_pid);
    
    return 0; // No available PIDs
}

static void free_pid(uint32_t pid) {
    if (pid < KOS_CONFIG_MAX_PROCESSES) {
        g_process_table[pid] = NULL;
    }
}

static hal_result_t add_process_to_table(kos_process_t* process) {
    if (!process || process->pid >= KOS_CONFIG_MAX_PROCESSES) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    g_process_table[process->pid] = process;
    g_process_manager.table.count++;
    
    return HAL_SUCCESS;
}

static hal_result_t remove_process_from_table(kos_process_t* process) {
    if (!process || process->pid >= KOS_CONFIG_MAX_PROCESSES) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    g_process_table[process->pid] = NULL;
    g_process_manager.table.count--;
    
    return HAL_SUCCESS;
}

static void initialize_process_context(kos_process_t* process, void* entry_point, void* stack_top) {
    hal_memset(&process->context, 0, sizeof(kos_cpu_context_t));
    
    // Set up stack pointer
    process->context.rsp = (uint64_t)stack_top;
    process->context.rbp = (uint64_t)stack_top;
    
    // Set up instruction pointer
    process->context.rip = (uint64_t)entry_point;
    
    // Set up segment registers
    process->context.cs = 0x08;  // Kernel code segment
    process->context.ds = 0x10;  // Kernel data segment
    process->context.es = 0x10;
    process->context.fs = 0x10;
    process->context.gs = 0x10;
    process->context.ss = 0x10;  // Kernel stack segment
    
    // Set up flags
    process->context.rflags = 0x202; // Interrupts enabled
    
    // Set up page directory
    process->context.cr3 = (uint64_t)process->page_directory;
}

static kos_process_t* create_idle_process(void) {
    kos_process_t* idle_process = NULL;
    hal_result_t result = kos_process_create(&idle_process, "idle", KOS_PROCESS_PRIORITY_IDLE, KOS_PROCESS_FLAG_KERNEL);
    if (result != HAL_SUCCESS) {
        return NULL;
    }
    
    idle_process->pid = KOS_IDLE_PROCESS_PID;
    idle_process->state = KOS_PROCESS_STATE_RUNNING;
    idle_process->time_slice = KOS_MIN_TIME_SLICE;
    
    // Add to process table
    add_process_to_table(idle_process);
    
    return idle_process;
}

// =============================================================================
// Process Manager API Implementation
// =============================================================================

hal_result_t kos_process_init(kos_process_manager_t* manager) {
    if (g_process_manager_initialized) {
        return HAL_ERROR_ALREADY_INITIALIZED;
    }
    
    log_info("Initializing KOS Process Manager");
    
    // Initialize process manager structure
    hal_memset(&g_process_manager, 0, sizeof(kos_process_manager_t));
    hal_memset(g_process_table, 0, sizeof(g_process_table));
    
    g_process_manager.table.next_pid = 1;
    g_process_manager.scheduler_enabled = false;
    g_process_manager.preemptive_enabled = true;
    g_process_manager.time_slice_default = KOS_DEFAULT_TIME_SLICE;
    g_process_manager.default_priority = KOS_DEFAULT_PRIORITY;
    
    // Create idle process
    g_process_manager.table.idle_process = create_idle_process();
    if (!g_process_manager.table.idle_process) {
        log_error("Failed to create idle process");
        return HAL_ERROR_OUT_OF_MEMORY;
    }
    
    // Set current process to idle
    g_process_manager.table.current_process = g_process_manager.table.idle_process;
    
    g_process_manager_initialized = true;
    
    log_info("Process Manager initialized successfully");
    return HAL_SUCCESS;
}

hal_result_t kos_process_shutdown(kos_process_manager_t* manager) {
    if (!g_process_manager_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    log_info("Shutting down KOS Process Manager");
    
    // Disable scheduler
    g_process_manager.scheduler_enabled = false;
    
    // Terminate all processes except idle
    for (uint32_t i = 1; i < KOS_CONFIG_MAX_PROCESSES; i++) {
        if (g_process_table[i] != NULL) {
            kos_process_terminate(g_process_table[i], 0);
        }
    }
    
    // Clean up idle process
    if (g_process_manager.table.idle_process) {
        kos_process_destroy(g_process_manager.table.idle_process);
    }
    
    g_process_manager_initialized = false;
    
    log_info("Process Manager shutdown completed");
    return HAL_SUCCESS;
}

hal_result_t kos_process_create(kos_process_t** process, const char* name, 
                               kos_process_priority_t priority, kos_process_flags_t flags) {
    if (!process || !name) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Allocate process structure
    kos_process_t* new_process = (kos_process_t*)hal_memalign(sizeof(kos_process_t), 16);
    if (!new_process) {
        return HAL_ERROR_OUT_OF_MEMORY;
    }
    
    // Initialize process structure
    hal_memset(new_process, 0, sizeof(kos_process_t));
    
    // Set basic information
    new_process->pid = allocate_pid();
    if (new_process->pid == 0) {
        hal_free(new_process);
        return HAL_ERROR_OUT_OF_MEMORY; // No available PIDs
    }
    
    hal_strncpy(new_process->name, name, KOS_CONFIG_MAX_NAME_LENGTH - 1);
    hal_strcpy(new_process->description, "KOS Process");
    
    new_process->state = KOS_PROCESS_STATE_CREATED;
    new_process->priority = priority;
    new_process->flags = flags;
    new_process->ppid = g_process_manager.table.current_process ? 
                       g_process_manager.table.current_process->pid : 0;
    
    // Set default values
    new_process->time_slice = g_process_manager.time_slice_default;
    new_process->quantum = g_process_manager.time_slice_default;
    new_process->remaining_time = g_process_manager.time_slice_default;
    new_process->nice_value = KOS_DEFAULT_NICE;
    
    // Initialize file descriptor table
    for (uint32_t i = 0; i < KOS_CONFIG_MAX_FILE_DESCRIPTORS; i++) {
        new_process->fd_table[i].file_object = NULL;
        new_process->fd_table[i].flags = 0;
        new_process->fd_table[i].ref_count = 0;
        new_process->fd_table[i].offset = 0;
    }
    new_process->fd_count = 0;
    
    // Allocate memory for process
    size_t stack_size = 64 * 1024; // 64KB stack
    new_process->memory.stack_start = (uintptr_t)hal_malloc(stack_size);
    new_process->memory.stack_end = new_process->memory.stack_start + stack_size;
    new_process->memory.total_size = stack_size;
    new_process->memory.used_size = 0;
    
    if (!new_process->memory.stack_start) {
        hal_free(new_process);
        return HAL_ERROR_OUT_OF_MEMORY;
    }
    
    // Initialize statistics
    hal_memset(&new_process->stats, 0, sizeof(kos_process_stats_t));
    new_process->stats.creation_time = hal_get_timestamp();
    
    // Set up parent-child relationships
    new_process->parent = g_process_manager.table.current_process;
    if (new_process->parent) {
        new_process->next_sibling = new_process->parent->first_child;
        if (new_process->parent->first_child) {
            new_process->parent->first_child->prev_sibling = new_process;
        }
        new_process->parent->first_child = new_process;
    }
    
    // Initialize context (will be set properly when starting)
    initialize_process_context(new_process, (void*)0x100000, // Default entry point
                              (void*)(new_process->memory.stack_end - 8));
    
    // Set magic number and initialized flag
    new_process->magic = KOS_PROCESS_MAGIC;
    new_process->initialized = true;
    
    // Add to process table
    hal_result_t result = add_process_to_table(new_process);
    if (result != HAL_SUCCESS) {
        hal_free((void*)new_process->memory.stack_start);
        hal_free(new_process);
        return result;
    }
    
    *process = new_process;
    
    log_info("Created process '%s' with PID %u", name, new_process->pid);
    return HAL_SUCCESS;
}

hal_result_t kos_process_destroy(kos_process_t* process) {
    if (!process || !process->initialized || process->magic != KOS_PROCESS_MAGIC) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    log_info("Destroying process '%s' (PID %u)", process->name, process->pid);
    
    // Remove from parent's child list
    if (process->parent) {
        if (process->parent->first_child == process) {
            process->parent->first_child = process->next_sibling;
        }
        if (process->prev_sibling) {
            process->prev_sibling->next_sibling = process->next_sibling;
        }
        if (process->next_sibling) {
            process->next_sibling->prev_sibling = process->prev_sibling;
        }
    }
    
    // Terminate all children
    kos_process_t* child = process->first_child;
    while (child) {
        kos_process_t* next_child = child->next_sibling;
        kos_process_terminate(child, 0);
        child = next_child;
    }
    
    // Free process memory
    if (process->memory.stack_start) {
        hal_free((void*)process->memory.stack_start);
    }
    
    // Remove from process table
    remove_process_from_table(process);
    
    // Clear magic and initialized flag
    process->magic = 0;
    process->initialized = false;
    
    // Free process structure
    hal_free(process);
    
    return HAL_SUCCESS;
}

hal_result_t kos_process_start(kos_process_t* process) {
    if (!process || !process->initialized || process->magic != KOS_PROCESS_MAGIC) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (process->state != KOS_PROCESS_STATE_CREATED) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    log_info("Starting process '%s' (PID %u)", process->name, process->pid);
    
    process->state = KOS_PROCESS_STATE_READY;
    process->stats.start_time = hal_get_timestamp();
    
    return HAL_SUCCESS;
}

hal_result_t kos_process_terminate(kos_process_t* process, int32_t exit_code) {
    if (!process || !process->initialized || process->magic != KOS_PROCESS_MAGIC) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    if (process->state == KOS_PROCESS_STATE_TERMINATED || 
        process->state == KOS_PROCESS_STATE_ZOMBIE) {
        return HAL_ERROR_INVALID_STATE;
    }
    
    log_info("Terminating process '%s' (PID %u) with exit code %d", 
              process->name, process->pid, exit_code);
    
    process->state = KOS_PROCESS_STATE_TERMINATED;
    process->exit_code = exit_code;
    process->stats.total_run_time = hal_get_timestamp() - process->stats.start_time;
    
    // If this is the current process, schedule next one
    if (process == g_process_manager.table.current_process) {
        kos_process_schedule();
    }
    
    return HAL_SUCCESS;
}

hal_result_t kos_process_schedule(void) {
    if (!g_process_manager_initialized || !g_process_manager.scheduler_enabled) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    kos_process_t* current = g_process_manager.table.current_process;
    kos_process_t* next = NULL;
    
    // Simple round-robin scheduling for now
    // Find next ready process
    uint32_t start_pid = current ? current->pid + 1 : 1;
    for (uint32_t i = 0; i < KOS_CONFIG_MAX_PROCESSES; i++) {
        uint32_t pid = (start_pid + i) % KOS_CONFIG_MAX_PROCESSES;
        if (pid == 0) pid = 1; // Skip idle
        
        kos_process_t* candidate = g_process_table[pid];
        if (candidate && candidate->state == KOS_PROCESS_STATE_READY) {
            next = candidate;
            break;
        }
    }
    
    // If no ready process found, use idle process
    if (!next) {
        next = g_process_manager.table.idle_process;
    }
    
    // If we're already running the selected process, nothing to do
    if (next == current) {
        return HAL_SUCCESS;
    }
    
    // Update scheduler statistics
    g_process_manager.scheduler_ticks++;
    g_process_manager.scheduler_time = hal_get_timestamp();
    
    // Update current process statistics
    if (current) {
        current->stats.last_run_time = g_process_manager.scheduler_time;
        current->stats.cpu_time += current->time_slice;
        current->stats.context_switches++;
        
        if (current->state == KOS_PROCESS_STATE_RUNNING) {
            current->state = KOS_PROCESS_STATE_READY;
        }
    }
    
    // Switch to next process
    next->state = KOS_PROCESS_STATE_RUNNING;
    next->stats.context_switches++;
    next->remaining_time = next->quantum;
    
    g_process_manager.table.current_process = next;
    
    // Perform context switch (this would call assembly function)
    // kos_context_switch(&current->context, &next->context);
    
    log_debug("Scheduled process '%s' (PID %u)", next->name, next->pid);
    
    return HAL_SUCCESS;
}

hal_result_t kos_process_yield(void) {
    if (!g_process_manager_initialized) {
        return HAL_ERROR_NOT_INITIALIZED;
    }
    
    kos_process_t* current = g_process_manager.table.current_process;
    if (current && current->state == KOS_PROCESS_STATE_RUNNING) {
        current->state = KOS_PROCESS_STATE_READY;
        return kos_process_schedule();
    }
    
    return HAL_SUCCESS;
}

// =============================================================================
// Process Utility Functions
// =============================================================================

uint32_t kos_process_get_pid(kos_process_t* process) {
    return process ? process->pid : 0;
}

uint32_t kos_process_get_ppid(kos_process_t* process) {
    return process ? process->ppid : 0;
}

kos_process_state_t kos_process_get_state(kos_process_t* process) {
    return process ? process->state : KOS_PROCESS_STATE_CREATED;
}

kos_process_priority_t kos_process_get_priority(kos_process_t* process) {
    return process ? process->priority : KOS_PROCESS_PRIORITY_NORMAL;
}

const char* kos_process_get_name(kos_process_t* process) {
    return process ? process->name : "unknown";
}

hal_result_t kos_process_get_by_pid(uint32_t pid, kos_process_t** process) {
    if (!process || pid >= KOS_CONFIG_MAX_PROCESSES) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *process = g_process_table[pid];
    return *process ? HAL_SUCCESS : HAL_ERROR_INVALID_PARAM;
}

hal_result_t kos_process_get_current(kos_process_t** process) {
    if (!process || !g_process_manager_initialized) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    *process = g_process_manager.table.current_process;
    return HAL_SUCCESS;
}

hal_result_t kos_process_get_stats(kos_process_t* process, kos_process_stats_t* stats) {
    if (!process || !stats || !process->initialized || process->magic != KOS_PROCESS_MAGIC) {
        return HAL_ERROR_INVALID_PARAM;
    }
    
    // Update current statistics
    if (process->state == KOS_PROCESS_STATE_RUNNING) {
        uint64_t current_time = hal_get_timestamp();
        process->stats.total_run_time = current_time - process->stats.start_time;
    }
    
    *stats = process->stats;
    return HAL_SUCCESS;
}
