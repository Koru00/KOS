#pragma once

// Arch specific put char will be implemented by the arch specific code
extern void arch_putchar(char c);

void kprint(const char* fmt);
