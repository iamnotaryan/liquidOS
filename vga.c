#include "vga.h"

static inline void outb(uint16_t port, uint8_t value) {
	__asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
} 
static uint16_t* const vga_buffer = (uint16_t*)VGA_ADDRESS;
static uint32_t cursor = 0;
static uint8_t color = 0x07;

static void vga_scroll(void) {
	if (cursor < VGA_WIDTH * VGA_HEIGHT)
		return;
	for (uint16_t y=1; y < VGA_HEIGHT; y++) {
		for (uint16_t x = 0; x< VGA_WIDTH; x++) {
			vga_buffer[(y-1) * VGA_WIDTH + x] =
				vga_buffer[y * VGA_WIDTH + x];
		}
	}
	for (uint16_t x = 0; x < VGA_WIDTH; x++) {
		vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
			(color << 8) | ' ';
	}
	cursor = (VGA_HEIGHT - 1) * VGA_WIDTH;
}

static void vga_update_hw_cursor(void) {
	uint16_t pos = cursor;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));

	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_putc(char c){
	if (c == '\n') {
		cursor += VGA_WIDTH - (cursor % VGA_WIDTH);
	}
	else if (c == '\b') {
		if (cursor > 0) {
			cursor--;
			vga_buffer[cursor] = (color << 8) | ' ';
		}
	}
	else {
		vga_buffer[cursor++] = (color << 8) | c;
	}

	vga_scroll();
	vga_update_hw_cursor();
}
void vga_clear(void){
	for (uint16_t i=0; i < VGA_WIDTH * VGA_HEIGHT; i++){
		vga_buffer[i] = (color << 8) | ' ';
	}
	cursor = 0;
	vga_update_hw_cursor();
}

void vga_print(const char* s) {
	while (*s) {
		vga_putc(*s++);
	}
}
