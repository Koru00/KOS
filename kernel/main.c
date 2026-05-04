// kernel/main.c

//extern void idt_init();

void kernel_main(void) {
    // At this point:
    // - You are in long mode
    // - Paging is active
    // - Stack is valid

    //idt_init();

    while (1) {
        __asm__ volatile ("hlt");
    }
}
