#ifndef VGA_H
#define VGA_H

#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

void vga_clear(void);
void vga_putc(char c);
void vga_print(const char* s);

#endif
