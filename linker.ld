ENTRY(kinit)

OUTPUT_ARCH(i386)
OUTPUT_FORMAT(binary)

/* This is for safety because my dumb bootloader otherwise might not read the
 * whole kernel */
MEMORY {
    IMG (RW) : ORIGIN = 0x10000, LENGTH = 1K
}

SECTIONS
{
    . = 0x10000;     /* At 64kb */

    .text : ALIGN(8) {
        *(.text);
    } > IMG 

    .data : ALIGN(8) {
        *(.data);
    } > IMG

    .bss : ALIGN(8) {
        *(.bss);
        *(.COMMON);
    } > IMG

    .filler : ALIGN(8) {
        *(.filler);
        . = ORIGIN(IMG) + LENGTH(IMG) - 1;
        BYTE(0xFF);
    } > IMG

    _KERNEL_END_ = .;
}

