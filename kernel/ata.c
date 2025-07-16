#include "kernel.h"
#include "ata.h"
#include "vga.h"

static uint32_t sectors_count = 0;
static uint32_t sectors_lba = 0;

uint8_t hdd_status()
{
    // Identify the drive first

    outb(ATA_DRIVE_SELECT, 0xE0);

    outb(ATA_LBA_LO, 0); // low
    outb(ATA_LBA_MI, 0); // mid
    outb(ATA_LBA_HI, 0); // high

    outb(ATA_IO_COMMAND, 0xEC); // identify command

    // reading the number of addressible LBA's on the drive

    uint16_t *scratch = _KERNEL_END_;

    // read all the bytes bro
    insw(ATA_IO_BASE, scratch, 256);

    uint32_t drive = scratch[59];
    drive = (drive << 16) | (scratch[60]);

    // last read lba
    sectors_lba = 0;
    sectors_count = drive;

    puts("Sectors in drive: ", 0xa);
    put_hex((drive & 0xFF00) >> 8, 0x0f);
    put_hex(drive & 0xFF, 0x0f);
    puts("\n\r", 0x0f);

    int status = ATA_IO_STATUS;
    uint8_t stat = 0;

    for(int i = 0; i < 15; i++)
        stat = inb(status);

    puts("Disk Status: ", 0xa);
    put_hex(stat, 0x0f);
    puts("\n\r", 0x0f);

    return stat;
}

uint32_t get_total_sectors()
{
    return sectors_count;
}

// each sectors is 512 bytes long,
//
uint32_t read_sectors(void *where, uint32_t lba, int count)
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
    uint32_t out = insw(ATA_IO_BASE, where, count * 256);

    /*
    puts("Out: ", 0xf);
    put_hex(out >> 24, 0xf);
    put_hex(out >> 16, 0xf);
    put_hex(out >> 8, 0xf);
    put_hex(out, 0xf);
    puts("\n\r", 0xf);

    uint32_t who = where;
    puts("Who: ", 0xf);
    put_hex(who >> 24, 0xf);
    put_hex(who >> 16, 0xf);
    put_hex(who >> 8, 0xf);
    put_hex(who, 0xf);
    puts("\n\r", 0xf);
    */

    sectors_lba = lba + 1;

    return out;
}

uint32_t read_consec_sectors(void *where, int count)
{
    uint32_t out = read_sectors(where, sectors_lba, count);
    sectors_lba += count;
    return out;
}
