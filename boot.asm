bits 32

section .multiboot
align 4
dd 0x1BADB002
dd 0
dd -(0x1BADB002)

section .text
global _start

_start:
    mov esp, stack_top

    ; write 'K' to VGA text buffer safely
    mov dword [0xB8000], 0x07204B

.halt:
    hlt
    jmp .halt

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
