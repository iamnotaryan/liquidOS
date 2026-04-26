
#include "vga.h"
#include "serial.h"
#include "idt.h"
#include "pic.h"


void kernel_main(void) {
	serial_init();
	serial_write("[KERNEL] serial initalized\n");

	idt_init();
	pic_remap();

	__asm__ volatile ("sti");
	serial_write("[KERNEL] interrupts enabled\n");

	vga_clear();
	vga_print("VGA OK\n");
	vga_print("liquid> ");

	serial_write("[KERNEL] VGA ready\n");
	serial_write("[KERNEL] entering idle\n");

	while (1) {
		__asm__ volatile ("hlt");
	}
}
