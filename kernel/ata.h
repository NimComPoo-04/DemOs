#ifndef _ATA_H_
#define _ATA_H_

uint8_t hdd_status();
void read_sectors(void *where, uint32_t lba, int count);

#endif
