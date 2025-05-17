#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void pit_install();
uint32_t get_timer_ticks();

#endif
