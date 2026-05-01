// kernel/main.c

extern char stack_top;

void kernel_main(void) {
    // At this point:
    // - You are in long mode
    // - Paging is active
    // - Stack is valid

    volatile char *video = (volatile char*)0xB8000;
    video[0] = 'O';
    video[1] = 0x07;

    while (1) {
        __asm__ volatile ("hlt");
    }
}
