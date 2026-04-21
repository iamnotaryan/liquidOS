#include "pic.h"
#include "serial.h"

static inline void outb(uint16_t port, uint8_t val) {
	__asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
	uint8_t ret;
	__asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

void pic_remap(void) {
	serial_write("[PIC] remapping\n");

	uint8_t a1 = inb(PIC1_DATA);
	uint8_t a2 = inb(PIC2_DATA);

	outb(PIC1, 0x11);
	outb(PIC2, 0x11);

	outb(PIC1_DATA, 0x20);
	outb(PIC2_DATA, 0x28);

	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);


	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);

	outb(PIC1_DATA, a1);
	outb(PIC2_DATA,a2);

	serial_write("[PIC] remap done\n");

}

void pic_send_eoi(uint8_t irq) {
	if (irq >= 8) outb(PIC2, PIC_EOI);
	outb(PIC1, PIC_EOI);
}
