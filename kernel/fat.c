#include "fat.h"
#include "mem.h"

static uint8_t *fat_mem = 0;
static uint16_t fat_sector_start = 17;
static uint16_t fat_total_sectors = 0;

static uint8_t *BPB = 0;

// Stores the fat partition
// Information on all the sectors
static uint8_t *FAT = 0;
static uint32_t FAT_length = 0;
static uint32_t FAT_sectors = 0;

// The root directory clusters, we read this completely too
static dirent_t *ROOT = 0;
static uint32_t ROOT_count = 0;
static uint32_t ROOT_sectors = 0;

static uint16_t DATA_sector_start = 0;

void load_filesystem()
{
    fat_mem = _KERNEL_END_;
    fat_total_sectors = get_total_sectors();

    BPB = allocate(512);

    // Read the whole harddisk because its a measely 128K of memory
    read_sectors(BPB, fat_sector_start, 1);

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

    FAT_length = FAT_sectors * 512;     // each record is 2 bytes long, 512 bytes each
    FAT = allocate(FAT_length);
    read_consec_sectors(FAT, FAT_sectors);

    puts("FAT Cluster Start: ", 0xa);
    for(int i = 0; i < 8; i++)
        put_hex(FAT[i], 0xf);
    puts("\n\r", 0xf);

    // Now read the root sectors
    ROOT_sectors = ROOT_count * 32 / 512; // (ROOT_count * 31 >> 9) + ((ROOT_count * 32 & 0x3FF) ? 1 : 0);
    ROOT = allocate(ROOT_sectors * 512);
    read_consec_sectors(ROOT, ROOT_sectors);

    /*
    puts("ROOT Entries Start: ", 0xa);
    puts(ROOT[0].filename, 0xf);
    puts("\n\r", 0xf);
    */

    // BPB + FAT + ROOT then u get the datasectors
    DATA_sector_start = fat_sector_start + 1 + FAT_sectors + ROOT_sectors;

    puts("\n\r", 0xf);
    put_hex(DATA_sector_start >> 8, 0xf);
    put_hex(DATA_sector_start, 0xf);
    puts("\n\r", 0xf);

    read_root_file("TEST    TXT");
}

int filenamecmp(char *k, char *m)
{
    for(int i = 0; i < 11; i++)
    {
        if(k[i] != m[i])
            return 0;
    }
    return 1;
}

// read a file in the root directory
file_t *read_root_file(char *name)
{
    dirent_t *search = ROOT;

    uint16_t fat_sector = search->fat_cluster;

    put_hex(fat_sector >> 8, 0xf);
    put_hex(fat_sector & 0xFF, 0xf);
    puts("\n\r", 0xf);

    file_t *head = 0;
    file_t *tail = 0;

    char thing[512];

    while(fat_sector < 0xFF8)
    {
        // Read the sectors and done
        file_t *f = allocate(sizeof(file_t));
        f->fat = fat_sector;
        f->next = 0;
        read_sectors(thing, DATA_sector_start + (fat_sector - 2), 1);

        puts(thing, 0xf);

        put_hex((fat_sector - 2 + DATA_sector_start) >> 8, 0xf);
        put_hex(fat_sector - 2 + DATA_sector_start, 0xf);

        if(head == 0)
            head = f;

        if(tail == 0)
            tail = f;
        else
        {
            tail->next = f;
            tail = f;
        }

        // The next sector
        uint16_t next;
        if((fat_sector & 1) == 0)
        {
            put_hex((FAT[fat_sector + 1] & 0xF0) >> 4, 0xf);
            put_hex(FAT[fat_sector], 0xf);

            uint8_t hi = (FAT[fat_sector + 1] & 0xF0) >> 4;
            uint8_t lo = FAT[fat_sector];

            next = (hi << 8) | lo;
        }
        else
        {
            put_hex(FAT[fat_sector + 1] & 0xF, 0xf);
            put_hex(FAT[fat_sector], 0xf);

            uint8_t hi = FAT[fat_sector + 1] & 0xF;
            uint8_t lo = FAT[fat_sector];

            next = (hi << 8) | lo;
        }

        fat_sector = next;
    }

    puts("\n\r", 0xf);

    file_t *f = head;
    while(f)
    {
        put_hex(f->fat, 0xf);
        puts(f->payload, 0xf);
        f = f->next;
    }

    puts("\n\r", 0xf);

    return head;
}
