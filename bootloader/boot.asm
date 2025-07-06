;;
;; Bootloader
;;

org 0x7C00
bits 16

;
; Initializing Hardware
;
init:
    cli 
    ; set up the stack
    mov ax, 0
    mov ss, ax
    mov sp, 0x7C00

    ; set the data segment is zero
    xor ax, ax
    mov ds, ax

    ; setting up the vga text mode
    call set_textmode

    ; write a string
    mov di, MESGTEST
    call print_string

    ; testing hex
    mov di, HEXTEST
    call print_string

    mov di, 0xDEAD
    call print_hex

    mov cx, 4
    .spacert:
        mov di, SPACE
        call print_string
    loop .spacert

    mov edi, 0xDEADBEEF
    call print_hex32

    mov di, NEWLINE
    call print_string

    ; enable a20 line to allow memory access beyond 64k
    in al, 0x92
    or al, 2
    out 0x92, al

    ; read next sector
    mov di, 2
    call read_sectors

    mov di, DISK_READ
    call print_string

    ; launch the kernel
    call load_kernel

;
; Global Descriptor Table
;
GDT_Start:

GDT_Null: dd 0               ; null segment
          dd 0

GDT_Code: dw 0xFFFF          ; limit 0xFFFF, start 0x0000
          dw 0x0000
          db 0x00            ; continuation of base address (0)
          db 0x9A            ; priviledge kernel level
          db 0xCF            ; 32 bit code spec
          db 0x00            ; base addr cont.

GDT_Data: dw 0xFFFF          ; similar to GDT_Code section
          dw 0x0000
          db 0x00
          db 0x92 
          db 0xCF
          db 0x00
GDT_End:

GDT_Size equ (GDT_End - GDT_Start - 1)

GDT_Desc: dw GDT_Size
          dd GDT_Start

;
; kernel located in the next few sectors
;
load_kernel:
    cli
    lgdt [GDT_Desc]

    mov eax, cr0  ; set the protected mode flag
    or  eax, 1
    mov cr0, eax
    
    jmp long 0x08:0x10000 ; perform a long jump


;
; read sectors from disk (di -> how many to load)
;
read_sectors:
    pusha

    mov si, 5
    .loop:

    mov ax, 0x1000      ;; place it right after
    mov es, ax
    mov bx, 0x00

    mov ax, di          ;; how many sectors to read

    mov ah, 0x02        ;; must be 0x02
    mov ch, 0x00        ;; cylinder thing
    mov cl, 0x02        ;; sector number
    mov dh, 0x00        ;; head number
    mov dl, 0x80        ;; drive number

    int 0x13

    dec si
    jnz .loop

    jc .error
        
    popa
    ret

    .error:
        mov di, .thing
        call print_string
        
        mov ah, 0x01
        mov al, 0x00
        int 0x13

        mov di, ax
        call print_hex

        mov di, NEWLINE
        call print_string
        cli
        hlt

    .thing: db "Drive Error Aparently! STATUS: ", 0

;
; set screen mode
;
set_textmode:
    pusha

    mov ah, 0x00
    mov al, 0x03
    int 0x10

    popa
    ret

;
; get string legth, di = string start address, cx = return count
;
strlen:
    push di
    mov cx, 0
    .loop:
        cmp byte [di], 0
        jz .quit
        inc cx
        inc di
        jmp .loop
    .quit:
    pop di
    ret

;
; print a degu string to vga, di = starting address
;
print_string:
    pusha

    mov ah, 0x03    ;; get the current cursor position
    mov bh, 0x00
    int 0x10

    mov ax, 0x00    ;; setting up the string regs
    mov es, ax
    mov bp, di

    call strlen     ;; string length

    mov ah, 0x13    ;; setting up string drawing interrupt
    mov al, 0x01
    mov bh, 0x00
    mov bl, 0x0f    ;; white boi

    int 0x10

    popa
    ret

;
; print a hex string
;
print_hex:
    pusha
    
    mov bx, STRBUFF + 4

    mov byte [bx], 0
    dec bx

    .loop:
    cmp bx, STRBUFF
    jl .quit
        mov si, di
        and si, 0xF
        shr di, 4

        mov al, [si + HEXS]
        mov [bx], al
        dec bx
    jmp .loop
    .quit:

    mov di, STRBUFF
    call print_string

    popa
    ret

;
; print a hex 32 string
;
print_hex32:
    push edi
    shr edi, 16
    call print_hex
    pop edi
    call print_hex
    ret

;; Print constants

CR  equ 0xD
LF  equ 0xA
EOL equ 0x0

NEWLINE: db CR, LF, EOL
SPACE: db " ", EOL

HEXS: db "0123456789ABCDEF", EOL

MESGTEST: db "Hello World, Successful Boot!!", CR, LF, EOL
HEXTEST: db "Testing Hexadecimal! ", EOL

STRBUFF: times 64 db EOL

; Information on the disk

DISK_READ: db "Kernel read successfully!", CR, LF, EOL

times 510 - ($ - $$) db 0
dw 0xAA55
