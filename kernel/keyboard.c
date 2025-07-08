#include "kernel.h"
#include "keyboard.h"

#define KBD_DATA_PORT    0x60
#define KBD_COMMAND_PORT 0x64

void kbd_init()
{
    outb(0x64, 0xAE);                   // Enable thing
    for(int i = 0; i < 1000; i++);      // wait a while

    outb(0x64, 0xFF);                   // Reset signal
    for(int i = 0; i < 1000; i++);      // wait a while
}

char scancode[2] = {0};

// read a scancode, returns scancode size
char *kbd_poll()
{
    scancode[0] = 0;
    scancode[1] = 0;

    if((inb(0x64) & 1) == 0)
        return 0;
    else
    {
        unsigned char k0 = inb(0x60);
        scancode[0] = k0;

        if(k0 == 0xF0 || k0 == 0xE0)
        {
            scancode[1] = inb(0x60);
        }
    }

    return scancode;
}

static char *kbd_scancode       = "\e1234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;'`\0\\zxcvbnm,./";
static char *kbd_shift_scancode = "\e!@#$%^&*()_+\b\tQWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?";

static inline char toupper(char c)
{
    return (c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c);
}

keycode_t kbd_convert(char *scancode)
{
    if(scancode == 00)
        return (keycode_t){0};

    static int shift_mod = RELEASED;
    static int caps_mod = 0;

    keycode_t k;

    k.scancode[0] = scancode[0];
    k.scancode[1] = scancode[1];

    if((unsigned char)scancode[0] != 0xE0)
    {
        k.state  = (scancode[0] >> 7 ? RELEASED : PRESSED);
        char scan = (scancode[0] & ~0x80);

        // if shift is pressed keep track
        if(scan == 0x36 || scan == 0x2A)
        {
            shift_mod = k.state;
            k.mod_key = SHIFT;
        }

        // Set the capslock
        if(scan == 0x3A && k.state == PRESSED)
            caps_mod = !caps_mod;

        if(scan >= 0x01 && scan <= 0x36)
        {
            if(shift_mod == PRESSED)
                k.ascii_key = kbd_shift_scancode[(unsigned int)(scan - 1)];
            else
                k.ascii_key = kbd_scancode[(unsigned int)(scan - 1)];

            if(caps_mod)
                k.ascii_key = toupper(k.ascii_key);
        }

        return k;
    }
    else
    {
        // Device Control ascii character will be used for arrow keys
        // up dev-1, right dev-2, down dev-3, left dev-4

        k.state  = (scancode[1] >> 7 ? RELEASED : PRESSED);

        switch(scancode[1] & 0x7F)
        {
            case 0x48: k.ascii_key = 0x11; break; // up
            case 0x4D: k.ascii_key = 0x12; break; // right
            case 0x50: k.ascii_key = 0x13; break; // down
            case 0x4B: k.ascii_key = 0x14; break; // left
            default: k.ascii_key = 0x00;
        }

        return k;
    }
}
