#include "kernel.h"
#include "ata.h"
#include "vga.h"

#define ATA_IO_BASE   0x1F0
#define ATA_CTRL_BASE 0x3F6

#define ATA_DRIVE_SELECT (ATA_IO_BASE + 6)
#define ATA_SECTOR_COUNT (ATA_IO_BASE + 2)

#define ATA_LBA_LO (ATA_IO_BASE + 3)
#define ATA_LBA_MI (ATA_IO_BASE + 4)
#define ATA_LBA_HI (ATA_IO_BASE + 5)

#define ATA_IO_STATUS (ATA_IO_BASE + 7)
#define ATA_IO_COMMAND (ATA_IO_BASE + 7)

static uint32_t sectors_count = 0;

uint8_t hdd_status()
{
    // Identify the drive first

    outb(ATA_DRIVE_SELECT, 0xE0);

    outb(ATA_LBA_LO, 0); // low
    outb(ATA_LBA_MI, 0); // mid
    outb(ATA_LBA_HI, 0); // high

    outb(ATA_IO_COMMAND, 0xEC); // identify command

    // reading the number of addressible LBA's on the drive

    extern uint16_t *_KERNEL_END_;
    uint16_t *scratch = _KERNEL_END_;

    // read all the bytes bro
    insw(ATA_IO_BASE, scratch, 256);

    uint32_t drive = scratch[59];
    drive = (drive << 16) | (scratch[60]);

    sectors_count = drive;

    puts("Sectors in drive = ", 0x0f);
    put_hex((drive & 0xFF00) >> 8, 0x0f);
    put_hex(drive & 0xFF, 0x0f);
    puts("\n\r", 0x0f);

    int status = ATA_IO_STATUS;
    uint8_t stat = 0;

    for(int i = 0; i < 15; i++)
        stat = inb(status);

    return stat;
}

// each sectors is 512 bytes long,
//
void read_sectors(void *where, uint32_t lba, int count)
{
    if(lba >= sectors_count)
        return;

    outb(ATA_DRIVE_SELECT, 0xE0 | ((lba >> 24) & 0xF));

    outb(ATA_LBA_LO, lba         & 0xFF); // low
    outb(ATA_LBA_MI, (lba >> 8)  & 0xFF); // mid
    outb(ATA_LBA_HI, (lba >> 16) & 0xFF); // high

    // Read sectors
    outb(ATA_IO_COMMAND, 0x20);

    // Check status
    int status = ATA_IO_STATUS;

    for(int i = 0; i < 15; i++) inb(status);

    // Read sectors (don't do this, do what the spec says)
    insw(ATA_IO_BASE, where, count * 256);
}
