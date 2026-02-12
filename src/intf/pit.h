#pragma once
#include "interrupts/isr.h"

// Timer management functions
void timer_phase(int hz);
void timer_handler(struct registers *r);
void timer_install();
void timer_wait(int ticks);
void timer_callback(void);
unsigned long timer_get_ticks(void);
