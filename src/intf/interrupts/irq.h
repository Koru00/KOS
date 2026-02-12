#pragma once
#include "isr.h"

void irq_install_handler(int irq, void (*handler)(struct registers *r));
void irq_uninstall_handler(int irq);
void irq_remap(void);
void irq_install(void);