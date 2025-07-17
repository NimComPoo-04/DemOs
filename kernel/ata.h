#ifndef _ATA_H_
#define _ATA_H_

#define ATA_IO_BASE   0x1F0
#define ATA_CTRL_BASE 0x3F6

#define ATA_DRIVE_SELECT (ATA_IO_BASE + 6)
#define ATA_SECTOR_COUNT (ATA_IO_BASE + 2)

#define ATA_LBA_LO (ATA_IO_BASE + 3)
#define ATA_LBA_MI (ATA_IO_BASE + 4)
#define ATA_LBA_HI (ATA_IO_BASE + 5)

#define ATA_IO_STATUS (ATA_IO_BASE + 7)
#define ATA_IO_COMMAND (ATA_IO_BASE + 7)

uint8_t hdd_status();
uint32_t read_sectors(void *where, uint32_t lba, int count);
uint32_t read_consec_sectors(void *where, int count);
uint32_t get_total_sectors();
uint16_t get_current_lba();

#endif
