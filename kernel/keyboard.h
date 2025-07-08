#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

void kbd_init();
char *kbd_poll();

typedef struct keycode_t
{
    enum {PRESSED, RELEASED} state ;
    char ascii_key;
    enum { NOTMOD, SHIFT } mod_key;
    char scancode[2];
} keycode_t;

keycode_t kbd_convert(char *scancode);

void kbd_init();
char *kbd_poll();

#endif
