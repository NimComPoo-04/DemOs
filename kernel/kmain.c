#include "kernel.h"
#include "vga.h"

void kmain()
{
    init_screen();

    char col = 0x10;
    for(int i = 0; i < 16; i++)
    {
        puts("Hello, World From the Kernel!!!!\n\r", col);
        col++;
    }

    __asm__("cli\nhlt");
}

void outb(uint16_t port, uint8_t data)
{
    __asm__("outb edx, al"
            : : "a" (data), "d" (port));
}

uint8_t inb(uint16_t port)
{
    int data;
    __asm__("inb al, edx"
            : "=a" (data)
            : "d" (port));
    return data;
}
