#ifndef _VGA_H_
#define _VGA_H_

void init_screen();
static void clear_screen();
static void update_cursor();

void puts(char *mem, char mod);
int putc(char mem, char mod);

#endif
