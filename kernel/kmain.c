#include "kernel.h"
#include "vga.h"
#include "keyboard.h"

void kmain()
{
    for(int i = 0; i < 800000; i++);
    init_screen();
    kbd_init();

    char col = 0x10;
    for(int i = 0; i < 16; i++)
    {
        puts("Hello, World From the Kernel!!!!\n\r", col);
        col++;
    }

    while(1)
    {
        char *thing = kbd_poll();

        if(thing)
        {
            keycode_t k = kbd_convert(thing);
            put_hex(k.ascii_key, 0x0f);
            put_hex(k.state, 0x0f);
            putc(k.ascii_key, 0x0f);
            puts("   ", 0x0f);
        }
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

void panik(void)
{
    puts("!!!! Kernel Panik !!!!\n\r", 0x0C);
    while(1)
        __asm__("cli\nhlt");
}
