CC = i686-elf-gcc
LD = i686-elf-ld
AS = nasm

CFLAGS = -ffreestanding -m32 -O2 -Wall -Wextra -fno-builtin -nostdlib
LDFLAGS = -T linker.ld -nostdlib -m elf_i386

OBJS = entry.o kernel.o vga.o serial.o idt.o isr.o pic.o irq.o


.PHONY: all clean


all: kernel.elf


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

entry.o: entry.asm
	$(AS) -f elf32 entry.asm -o entry.o


isr.o: isr.asm
	$(AS) -f elf32 isr.asm -o isr.o

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o kernel.elf

clean:
	rm -f *.o kernel.elf
