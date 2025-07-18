#ifndef _FAT_H_
#define _FAT_H_

#include "kernel.h"
#include "ata.h"

// directory entry
typedef struct dirent_t
{
    char filename[8];
    char extention[3];

    uint8_t attrib;
    uint8_t reserved1;

    uint8_t  ctime_milis;
    uint16_t ctime;
    uint16_t cdate;

    uint16_t adate;
    uint16_t reserved2;

    uint16_t mtime;
    uint16_t mdate;

    uint16_t fat_cluster;
    uint32_t file_size;
} __attribute__((packed)) dirent_t;

void load_filesystem();

typedef struct file_t
{
    struct file_t *next;
    uint16_t fat;
    char payload[512];
} file_t;

// read a file in the root directory
file_t *read_root_file(char *name);

#endif
