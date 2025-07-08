#ifndef _VGA_H_
#define _VGA_H_

void init_screen();
void clear_screen();
void update_cursor();

void puts(char *mem, char mod);
int putc(char mem, char mod);

void put_hex(char mem, char mod);

#endif
