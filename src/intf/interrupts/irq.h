#pragma once
#include "isr.h"

void irq_install_handler(int irq, void (*handler)(struct registers *r));
void irq_install();