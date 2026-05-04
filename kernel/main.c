// kernel/main.c

extern void idt_init();
extern char stack_top;

void kernel_main(void) {
    // At this point:
    // - You are in long mode
    // - Paging is active
    // - Stack is valid

    idt_init();

    volatile char *video = (volatile char*)0xB8000;
    video[0] = 'O';
    video[1] = 0x07;
    
    __asm__ volatile ("mov $'P', %al");
    __asm__ volatile ("out %al, $0xE9");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
