#pragma once

#include "hal/hal_core.h"
#include "../types.h"
#include "../config.h"

// =============================================================================
// KOS - Process Management Interface
// =============================================================================

// Process states
typedef enum {
    KOS_PROCESS_STATE_CREATED = 0,
    KOS_PROCESS_STATE_READY,
    KOS_PROCESS_STATE_RUNNING,
    KOS_PROCESS_STATE_BLOCKED,
    KOS_PROCESS_STATE_TERMINATED,
    KOS_PROCESS_STATE_ZOMBIE
} kos_process_state_t;

// Process priority levels
typedef enum {
    KOS_PROCESS_PRIORITY_IDLE = 0,
    KOS_PROCESS_PRIORITY_LOW = 1,
    KOS_PROCESS_PRIORITY_NORMAL = 2,
    KOS_PROCESS_PRIORITY_HIGH = 3,
    KOS_PROCESS_PRIORITY_CRITICAL = 4,
    KOS_PROCESS_PRIORITY_REALTIME = 5
} kos_process_priority_t;

// Process flags
typedef enum {
    KOS_PROCESS_FLAG_NONE = 0x00000000,
    KOS_PROCESS_FLAG_KERNEL = 0x00000001,
    KOS_PROCESS_FLAG_USER = 0x00000002,
    KOS_PROCESS_FLAG_SYSTEM = 0x00000004,
    KOS_PROCESS_FLAG_DAEMON = 0x00000008,
    KOS_PROCESS_FLAG_INTERACTIVE = 0x00000010,
    KOS_PROCESS_FLAG_BACKGROUND = 0x00000020,
    KOS_PROCESS_FLAG_FOREGROUND = 0x00000040,
    KOS_PROCESS_FLAG_DEBUG = 0x00000080
} kos_process_flags_t;

// Process statistics
typedef struct {
    uint64_t creation_time;
    uint64_t start_time;
    uint64_t total_run_time;
    uint64_t last_run_time;
    uint64_t cpu_time;
    uint64_t user_time;
    uint64_t kernel_time;
    uint64_t context_switches;
    uint64_t page_faults;
    uint64_t system_calls;
    uint64_t wakeups;
    uint64_t preemptions;
} kos_process_stats_t;

// Process memory layout
typedef struct {
    uintptr_t code_start;
    uintptr_t code_end;
    uintptr_t data_start;
    uintptr_t data_end;
    uintptr_t heap_start;
    uintptr_t heap_end;
    uintptr_t stack_start;
    uintptr_t stack_end;
    size_t total_size;
    size_t used_size;
} kos_process_memory_t;

// CPU context structure (x86_64)
typedef struct {
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip;
    uint64_t rflags;
    uint64_t cs, ss, ds, es, fs, gs;
    uint64_t cr3;
} kos_cpu_context_t;

// File descriptor table entry
typedef struct {
    void* file_object;
    uint32_t flags;
    uint32_t ref_count;
    uint64_t offset;
} kos_fd_entry_t;

// Process Control Block (PCB)
typedef struct kos_process {
    // Process identification
    uint32_t pid;
    uint32_t ppid;
    uint32_t uid;
    uint32_t gid;
    char name[KOS_CONFIG_MAX_NAME_LENGTH];
    char description[KOS_CONFIG_MAX_DESC_LENGTH];
    
    // Process state and scheduling
    kos_process_state_t state;
    kos_process_priority_t priority;
    kos_process_flags_t flags;
    int32_t exit_code;
    
    // CPU context
    kos_cpu_context_t context;
    
    // Memory management
    kos_process_memory_t memory;
    void* page_directory;
    
    // Process relationships
    struct kos_process* parent;
    struct kos_process* next_sibling;
    struct kos_process* prev_sibling;
    struct kos_process* first_child;
    struct kos_process* next;
    struct kos_process* prev;
    
    // Scheduling information
    uint64_t time_slice;
    uint64_t remaining_time;
    uint64_t quantum;
    int32_t nice_value;
    
    // File descriptors
    kos_fd_entry_t fd_table[KOS_CONFIG_MAX_FILE_DESCRIPTORS];
    uint32_t fd_count;
    
    // Signal handling
    uint64_t pending_signals;
    void* signal_handlers[32];
    
    // Process statistics
    kos_process_stats_t stats;
    
    // Security and permissions
    uint32_t capabilities;
    uint32_t allowed_syscalls;
    
    // Process resources
    uint32_t max_memory;
    uint32_t max_files;
    uint32_t max_processes;
    
    // Internal state
    hal_bool_t initialized;
    uint32_t magic;
} kos_process_t;

// Process table
typedef struct {
    kos_process_t* processes[KOS_CONFIG_MAX_PROCESSES];
    uint32_t count;
    uint32_t next_pid;
    kos_process_t* current_process;
    kos_process_t* idle_process;
    // hal_mutex_t lock; // Commented out for now
} kos_process_table_t;

// Process manager
typedef struct {
    kos_process_table_t table;
    uint32_t scheduler_ticks;
    uint64_t scheduler_time;
    hal_bool_t scheduler_enabled;
    hal_bool_t preemptive_enabled;
    uint32_t time_slice_default;
    kos_process_priority_t default_priority;
} kos_process_manager_t;

// =============================================================================
// Process Management API
// =============================================================================

// Process lifecycle management
hal_result_t kos_process_init(kos_process_manager_t* manager);
hal_result_t kos_process_shutdown(kos_process_manager_t* manager);

// Process creation and destruction
hal_result_t kos_process_create(kos_process_t** process, const char* name, 
                               kos_process_priority_t priority, kos_process_flags_t flags);
hal_result_t kos_process_destroy(kos_process_t* process);
hal_result_t kos_process_clone(kos_process_t* parent, kos_process_t** child);

// Process state management
hal_result_t kos_process_start(kos_process_t* process);
hal_result_t kos_process_stop(kos_process_t* process);
hal_result_t kos_process_suspend(kos_process_t* process);
hal_result_t kos_process_resume(kos_process_t* process);
hal_result_t kos_process_terminate(kos_process_t* process, int32_t exit_code);

// Process scheduling
hal_result_t kos_process_schedule(void);
hal_result_t kos_process_yield(void);
hal_result_t kos_process_set_priority(kos_process_t* process, kos_process_priority_t priority);
hal_result_t kos_process_set_nice(kos_process_t* process, int32_t nice_value);

// Process information
hal_result_t kos_process_get_by_pid(uint32_t pid, kos_process_t** process);
hal_result_t kos_process_get_current(kos_process_t** process);
hal_result_t kos_process_get_parent(kos_process_t* process, kos_process_t** parent);
hal_result_t kos_process_get_children(kos_process_t* process, kos_process_t** children, uint32_t* count);

// Process utilities
uint32_t kos_process_get_pid(kos_process_t* process);
uint32_t kos_process_get_ppid(kos_process_t* process);
kos_process_state_t kos_process_get_state(kos_process_t* process);
kos_process_priority_t kos_process_get_priority(kos_process_t* process);
const char* kos_process_get_name(kos_process_t* process);

// Process statistics
hal_result_t kos_process_get_stats(kos_process_t* process, kos_process_stats_t* stats);
hal_result_t kos_process_reset_stats(kos_process_t* process);

// =============================================================================
// Process Management Constants
// =============================================================================

#define KOS_PROCESS_MAGIC          0x4B4F5350  // "KOSP"
#define KOS_IDLE_PROCESS_PID      0
#define KOS_INIT_PROCESS_PID      1
#define KOS_KERNEL_PROCESS_PID    2

#define KOS_MIN_TIME_SLICE       1000    // 1ms
#define KOS_DEFAULT_TIME_SLICE   10000   // 10ms
#define KOS_MAX_TIME_SLICE       100000  // 100ms

#define KOS_MIN_PRIORITY         KOS_PROCESS_PRIORITY_IDLE
#define KOS_MAX_PRIORITY         KOS_PROCESS_PRIORITY_REALTIME
#define KOS_DEFAULT_PRIORITY    KOS_PROCESS_PRIORITY_NORMAL

#define KOS_MIN_NICE           -20
#define KOS_MAX_NICE           19
#define KOS_DEFAULT_NICE       0

// =============================================================================
// Process Management Macros
// =============================================================================

#define KOS_PROCESS_IS_KERNEL(p)     ((p)->flags & KOS_PROCESS_FLAG_KERNEL)
#define KOS_PROCESS_IS_USER(p)       ((p)->flags & KOS_PROCESS_FLAG_USER)
#define KOS_PROCESS_IS_SYSTEM(p)     ((p)->flags & KOS_PROCESS_FLAG_SYSTEM)
#define KOS_PROCESS_IS_DAEMON(p)      ((p)->flags & KOS_PROCESS_FLAG_DAEMON)
#define KOS_PROCESS_IS_INTERACTIVE(p) ((p)->flags & KOS_PROCESS_FLAG_INTERACTIVE)

#define KOS_PROCESS_IS_READY(p)       ((p)->state == KOS_PROCESS_STATE_READY)
#define KOS_PROCESS_IS_RUNNING(p)     ((p)->state == KOS_PROCESS_STATE_RUNNING)
#define KOS_PROCESS_IS_BLOCKED(p)      ((p)->state == KOS_PROCESS_STATE_BLOCKED)
#define KOS_PROCESS_IS_TERMINATED(p)   ((p)->state == KOS_PROCESS_STATE_TERMINATED)
#define KOS_PROCESS_IS_ZOMBIE(p)      ((p)->state == KOS_PROCESS_STATE_ZOMBIE)

#define KOS_PROCESS_SET_STATE(p, s)    do { (p)->state = (s); } while(0)
#define KOS_PROCESS_SET_PRIORITY(p, pr)  do { (p)->priority = (pr); } while(0)
