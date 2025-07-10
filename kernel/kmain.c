#include "kernel.h"
#include "vga.h"
#include "keyboard.h"
#include "ata.h"

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

    uint8_t status = hdd_status();
    put_hex(status, 0x0f);
    puts("\n\r", 0x0f);

    // read this stuff at the end of the kernel no problem
    extern uint32_t _KERNEL_END_;
    uint8_t *s = _KERNEL_END_;

    for(int i = 0; i < 10; i++)
        put_hex(s[i], 0xf);
    puts("\n\r", 0x0f);

    read_sectors(_KERNEL_END_, 0, 1);

    put_hex(s[510], 0xf);
    put_hex(s[511], 0xf);
    puts("\n\r", 0x0f);

    /*
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
    */

    __asm__("cli\nhlt");
}

void outw(uint16_t port, uint16_t data)
{
    __asm__("outw edx, ax"
            : : "a" (data), "d" (port));
}

uint16_t inw(uint16_t port)
{
    int data;
    __asm__("inw ax, edx"
            : "=a" (data)
            : "d" (port));
    return data;
}

void insw(uint16_t port, void *addr, uint32_t size)
{
    __asm__("rep insw" : : "D" (addr), "d" (port), "c" (size));
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
