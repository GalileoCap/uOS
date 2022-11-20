#ifndef __FAT32_H__
#define __FAT32_H__

#include <vfs.h>
#include <ata.h>
#include <utils.h>

#define FAT32_HEADERSECTOR 0

#define FAT32_MAGIC0 (0x28)
#define FAT32_MAGIC1 (0x29)
#define FAT32_BOOTMAGIC (0xAA55)
#define FAT32_FSINFOMAGIC0 (0x41615252)
#define FAT32_FSINFOMAGIC1 (0x61417272)
#define FAT32_FSINFOMAGIC2 (0xAA550000)

#define FAT32_ATTR_RO      (0x01)
#define FAT32_ATTR_HIDE    (0x02)
#define FAT32_ATTR_SYS     (0x04)
#define FAT32_ATTR_ID      (0x08)
#define FAT32_ATTR_DIR     (0x10)
#define FAT32_ATTR_ARCHIVE (0x20)
#define FAT32_ATTR_LFN     (FAT32_ATTR_RO | FAT32_ATTR_HIDE | FAT32_ATTR_SYS | FAT32_ATTR_ID)

struct fat32_header {
  //U: Boot record
  u8 jmp[3]; //U: jmp instruction, must be 0xEB3C90
  char oem[8]; //U: OEM identifier NOTE: Zero-padded but not NULL-terminated
  u16 bytesPerSector;
  u8 sectorPerCluster;
  u16 reservedSectors;
  u8 fatCount;
  u16 rootEntryCnt;
  u16 sectorCountLow;
  u8 mediaType; //TODO
  u16 sectorsPerFat;
  u16 sectorsPerTrack;
  u16 headCount;
  u32 hiddenSectorCount;
  u32 sectorCountHigh;

  //U: Extended boot record
  u32 fatSz;
  u16 flags;
  u16 version;
  u32 rootCluster;
  u16 fsinfoSector;
  u16 backupSector;
  u8 rsv0[12];
  u8 drive; //TODO
  u8 rsv1; 
  u8 magic; //NOTE: Must be FAT32_MAGIC0 or FAT32_MAGIC1
  u32 serial;
  char label[11]; //NOTE: Padded with spaces, not null-terminated
  char fsID[8]; //NOTE: Always "FAT32  "
  u8 boot[420];
  u16 bootMagic; //NOTE: Must be FAT32_BOOTMAGIC
} __attribute__((packed));

struct fat32_fsinfo {
  u32 magic0; //NOTE: Must be FAT32_FSINFOMAGIC0
  u8 rsv0[480];
  u32 magic1; //NOTE: Must be FAT32_FSINFOMAGIC1
  u32 freeClusterCount; //NOTE: -1 if unknown
  u32 todo1; //TODO
  u8 rsv[1];
  u32 magic2; //NOTE: Must be FAT32_FSINFOMAGIC2
} __attribute__((packed));

struct fat32_dentry {
  char name[8]; //NOTE: Not null-terminated
  char ext[3]; //U: Extension; //NOTE: Not null-terminated
  u8 attributes; 
  u8 rsv;
  u8 createTimeTenths; //U: Tenths of a second of creation time
  u16 createTime; //U: Hour 5-bits, Minutes 6-bits, Seconds 5-bits; NOTE: Multiply seconds by 2, TODO?
  u16 createDate; //U: Year 7-bits, Month 4-bits, Day 5-bits
  u16 accessDate;
  u16 clusterHigh;
  u16 modTime;
  u16 modDate;
  u16 clusterLow;
  u32 size;
} __attribute__((packed));

struct fat32_lfn {
  u8 order; //TODO
  char first[10]; //TODO
  u8 attributes; //NOTE: Always FAT32_ATTR_LFN
  u8 type; //TODO
  u8 checksum; //TODO
  char second[12]; //TODO
  u8 zero[2];
  char third[4]; //TODO
} __attribute__((packed));

errno_t fat32_load(struct io_dev *dev);
size_t fat32_RW(struct vfs_file *file, void *buffer, size_t count, bool write, struct io_dev *dev);

#endif // __FAT32_H__
