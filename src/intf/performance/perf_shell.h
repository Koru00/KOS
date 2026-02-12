#pragma once

#include "perf_monitor.h"

// Performance shell commands
void perf_shell_init(void);
void perf_cmd_stats(void);
void perf_cmd_reset(void);
void perf_cmd_monitor(void);
void perf_cmd_alerts(void);
void perf_cmd_benchmark(void);

// Shell integration
void perf_register_shell_commands(void);

// Command help
void perf_show_help(void);
