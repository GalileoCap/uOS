#ifndef __ATA_H__
#define __ATA_H__

#include <utils.h>

#define ATA0 0x1F0
#define ATA1 0x170
#define ATA2 0x1E8
#define ATA3 0x168

#define ATA_DATA        0x0
#define ATA_ERR         0x1
#define ATA_SECTORCOUNT 0x2
#define ATA_LBAlo       0x3
#define ATA_LBAmid      0x4
#define ATA_LBAhi       0x5
#define ATA_DRIVE       0x6
#define ATA_COMSTAT     0x7
#define ATA_STATUS      0x7
#define ATA_SECCOUNT1   0x8
#define ATA_LBA3        0x9
#define ATA_LBA4        0xA
#define ATA_LBA5        0xB
#define ATA_CONTROL     0xC
#define ATA_ALTSTATUS   0xC
#define ATA_DEVADDRESS  0xD

#define ATA_MASTER   0xA0
#define ATA_SLAVE    0xB0

#define ATA_IDENTIFY 0xEC
#define ATA_CACHECLR 0xE7
#define ATA_READRETRY 0x20
#define ATA_WRITERETRY 0x30

#define ATA_STATUS_ERR (1 << 0)
#define ATA_STATUS_DRQ (1 << 3)
#define ATA_STATUS_SRV (1 << 4)
#define ATA_STATUS_DF  (1 << 5)
#define ATA_STATUS_RDY (1 << 6)
#define ATA_STATUS_BSY (1 << 7)

#define ATA_SECTOR_SIZE  512ul
#define ATA_SECTOR_WORDS 256ul

enum ata_t {
  ATA_UNK, ATA_PATA, ATA_SATA, ATA_ATAPI, ATA_SATAPI
};

enum io_dev_t {
  DEV_BLOCK
};

struct io_dev {
  u8 id;
  enum io_dev_t type;
  void *info;
  void *fs;
  char mount[4]; //TODO: Expand
  size_t (*read)(void *buffer, size_t count, size_t offset, struct io_dev *dev);
  size_t (*write)(void *buffer, size_t count, size_t offset, struct io_dev *dev);
};

extern struct io_dev ioDevices[4];

u64 ata_init(void);

#endif // __ATA_H__
