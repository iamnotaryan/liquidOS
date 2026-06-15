#include "serial.h"
#include "pic.h"
#include "vga.h"

#define INPUT_MAX 128

static char input_buffer[INPUT_MAX];
static int input_pos = 0;

static const char scancode_map[128] = {
	0,27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
	'\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
	0,	'a','s','d','f','g','h','j','k','l',';','\'','`',
	0,	'\\','z','x','c','v','b','n','m',',','.','/',
	0,	'*',
	0,	' ',
};
static const char shift_map[128] = {
    0,27,
    '!','@','#','$','%','^','&','*','(',')',
    '_','+','\b',
    '\t',
    'Q','W','E','R','T','Y','U','I','O','P',
    '{','}','\n',
    0,
    'A','S','D','F','G','H','J','K','L',
    ':','"','~',
    0,
    '|',
    'Z','X','C','V','B','N','M',
    '<','>','?',
    0,'*',
    0,' ',
};
static inline unsigned char inb(unsigned short port) {
	unsigned char ret;
	__asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

static int tick = 0;
static int shift_pressed = 0;
static int caps_lock = 0;
int strcmp(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return *a - *b;
        a++; b++;
    }
    return *a - *b;
}

int strncmp(const char *a, const char *b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
        if (a[i] == '\0') return 0;
    }
    return 0;
}

void irq0_handler(void) {
	tick++;
	if (tick % 100 ==0 && input_pos == 0) {
		vga_print(".");
	}
	pic_send_eoi(0);
}
void irq1_handler(void) {
	unsigned char scancode = inb(0x60);
	if (scancode == 42) {
		shift_pressed = 1;
		pic_send_eoi(1);
		return;
	}
	if (scancode == 170) {
		shift_pressed = 0;
		pic_send_eoi(1);
		return;
	}
	if (scancode == 54) {
		shift_pressed = 1;
		pic_send_eoi(1);
		return;
	}
	if (scancode == 182) {
		shift_pressed = 0;
		pic_send_eoi(1);
		return;
	}
	if (scancode == 58) {
		caps_lock = !caps_lock;
		pic_send_eoi(1);
		return;
	}

	if (scancode & 0x80) {
		pic_send_eoi(1);
		return;
	}
	char c;
	if (shift_pressed)
		c = shift_map[scancode];
	else
		c = scancode_map[scancode];
	
	if (caps_lock && !shift_pressed) {
		if ( c >= 'a' && c <= 'z')
			c -= 32;
	}
	if (!c) {
		pic_send_eoi(1);
		return;
	}
	if (c == '\b') {
    	if (input_pos > 0) {
        	input_pos--;
        	vga_putc('\b');
    }
	}
	else if (c == '\n') {
    vga_putc('\n');

    input_buffer[input_pos] = '\0';

    // ---- COMMAND HANDLING ----

    if (strncmp(input_buffer, "echo ", 5) == 0) {
        vga_print(input_buffer + 5);
        vga_putc('\n');
    }
    else if (strcmp(input_buffer, "help") == 0) {
        vga_print("Commands:\n");
		vga_print("  help\n");
		vga_print("  echo\n");
		vga_print("  clear\n");
		vga_print("  uptime\n");
    }
    else if (strcmp(input_buffer, "clear") == 0) {
    vga_clear();
	}
	else if (strcmp(input_buffer, "uptime")==0){
		vga_print("Ticks:");
		int t = tick;
		char buf[16];
		int i = 0;

		if (t==0) {
			vga_putc('0');
		} else {
			while (t > 0 ) {
				buf[i++] = '0' + (t % 10);
				t /= 10;
			}
			while (i--) {
				vga_putc(buf[i]);
			}
		}
		vga_putc('\n');
	}

    input_pos = 0;

    // print prompt again
    vga_print("liquid> ");
	}

	else {
		if (input_pos < INPUT_MAX - 1) {
			input_buffer[input_pos++] = c;
			vga_putc(c);
		}
	}
	pic_send_eoi(1);
}
