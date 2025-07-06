bits 32

global kinit
extern kmain

;; Sets up some states for the kernel
kinit:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax

    mov esp, 0x9000

    jmp kmain

    cli
    hlt

section .filler:
    db "Things you do"
