bits 32

global isr_test
global irq0

global irq1
extern irq1_handler

extern irq0_handler

isr_test:
	pusha
	popa
	iret
irq0:
	pusha
	call irq0_handler
	popa
	iret

irq1:
	pusha
	call irq1_handler
	popa
	iret
