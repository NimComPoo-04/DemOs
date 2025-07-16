#include "fat.h"

static uint8_t *fat_mem = 0;
static uint16_t fat_sector_start = 17;
static uint16_t fat_total_sectors = 0;

static uint8_t *BPB = 0;

// Stores the fat partition
// Information on all the sectors
static uint16_t *FAT = 0;
static uint32_t FAT_length = 0;
static uint32_t FAT_sectors = 0;

// The root directory clusters, we read this completely too
static dirent_t *ROOT = 0;
static uint32_t ROOT_count = 0;
static uint32_t ROOT_sectors = 0;

void load_filesystem()
{
    fat_mem = 1024 * 1024;
    fat_total_sectors = get_total_sectors();

    put_hex(fat_total_sectors >> 8, 0xf);
    put_hex(fat_total_sectors, 0xf);

    // Read the whole harddisk because its a measely 128K of memory
    read_sectors(fat_mem, fat_sector_start, 1);

    BPB = fat_mem;

    puts("OEM Name: ", 0xa);
    puts(BPB+0x3, 0xF);
    puts("\n\r", 0xf);

    puts("Fat Count: ", 0xa);
    put_hex(BPB[0x10], 0xF);
    puts("\n\r", 0xf);

    puts("Sector Count: ", 0xa);
    put_hex(BPB[0x14], 0xF);
    put_hex(BPB[0x13], 0xF);
    puts("\n\r", 0xf);

    puts("Sectors Per Fat: ", 0xa);
    put_hex(BPB[0x17], 0xF);
    put_hex(BPB[0x16], 0xF);
    puts("\n\r", 0xf);

    puts("Root Directory Entries: ", 0xa);
    put_hex(BPB[0x12], 0xF);
    put_hex(BPB[0x11], 0xF);
    puts("\n\r", 0xf);

    // Getting all the correct sectors
    FAT_sectors = ((uint32_t)BPB[0x17] << 8) | BPB[0x16];

    puts("Fat Sector Count: ", 0xa);
    put_hex(FAT_sectors >> 8, 0xF);
    put_hex(FAT_sectors, 0xF);
    puts("\n\r", 0xf);

    ROOT_count =  ((uint32_t)BPB[0x12] << 8) | BPB[0x11];

    puts("Root Directory Count: ", 0xa);
    put_hex(ROOT_count>> 8, 0xF);
    put_hex(ROOT_count, 0xF);
    puts("\n\r", 0xf);

    puts("Check Sector: ", 0xa);
    put_hex(BPB[511], 0xF);
    put_hex(BPB[510], 0xF);
    puts("\n\r", 0xf);

    // Now read all the fat clusters

    read_consec_sectors(fat_mem + 512, FAT_sectors);
    FAT = fat_mem + 512;
    FAT_length = FAT_sectors * 512;     // each record is 2 bytes long, 512 bytes each

    puts("FAT Cluster Start: ", 0xa);
    for(int i = 0; i < 10; i++)
        put_hex(FAT[i], 0xf);
    puts("\n\r", 0xf);

    // Now read the root sectors
    read_consec_sectors(fat_mem + 512 + FAT_length, ROOT_count * 32 / 512);
    ROOT = fat_mem + 512 + FAT_length;

    puts("ROOT Entries Start: ", 0xa);
    puts(ROOT[0].filename, 0xf);
    puts("\n\r", 0xf);
}
