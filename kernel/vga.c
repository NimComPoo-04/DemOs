#include "kernel.h"

// We are assuming the VGA mode is 0x03

#define VGA_MEMORY_START 0xB8000
#define VGA_MEMORY_END   0xB8FFF

static uint8_t screen_width = 80;
static uint8_t screen_height = 25;
static uint16_t screen_res = 80 * 25;

static uint16_t *screen = (uint16_t *)VGA_MEMORY_START;
static uint16_t cursor = 0;

static void update_cursor()
{
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t) (cursor & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t) ((cursor >> 8) & 0xFF));
}

static void clear_screen()
{
    for(int i = 0; i < screen_res; i++)
        screen[i] = 0x0F00;
}

void init_screen()
{
    clear_screen();
    cursor = 0;
    update_cursor();
}

int putc(char mem, char mod)
{
    switch(mem)
    {
        case 0:
            return 0;

        case '\n':
            cursor += screen_width;
            break;

        case '\r':
            cursor -= (cursor % screen_width);
            break;

        default:
            screen[cursor++] = (mod << 8) | mem;
    }

    cursor %= screen_res;

    update_cursor();

    return 1;
}

void puts(char *mem, char mod)
{
    while(putc(*mem++, mod));
}
