#pragma once
#include "isr.h"

void timer_phase(int hz);
void timer_handler(struct registers *r);
void timer_install();
void timer_wait(int ticks);
