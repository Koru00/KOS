
int running;


static inline void outw(unsigned short port, unsigned short val) {
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

void shutdown() {
    outw(0x604, 0x2000);  // QEMU shutdown
    outw(0xB004, 0x2000); // Bochs shutdown
    // Fallback (just in case)
    asm volatile("cli; hlt");
}

void set_running(int state)
{
    running = state;
    if (!running)
    {
        shutdown();
    }
}

int get_running()
{
    return running;
}

