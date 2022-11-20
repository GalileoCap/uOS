#include <fat32.h>

void fat32_toUpper(char *name) {

}

errno_t fat32_load(struct io_dev *dev) {
  printf("[fat32_load] dev=%u\n", dev->id);

  struct fat32_header *header = malloc(sizeof(*header));
  dev->read(header, sizeof(*header), FAT32_HEADERSECTOR, dev);

  //A: Check valid fs
  if (
    (header->magic != FAT32_MAGIC0 && header->magic != FAT32_MAGIC1)
    //TODO: More checks
  ) { errno = ENODEV; goto end; }

  //TODO: Save fsinfo in dev
  //TODO: What else?

  u32 fatSector = header->reservedSectors;
  u32 fatSectorCnt = header->fatSz * header->fatCount;

  u32 rootSector = fatSector + fatSectorCnt;
  u32 rootSectorCnt = (sizeof(struct fat32_dentry) * header->rootEntryCnt + header->bytesPerSector - 1) / header->bytesPerSector; //TODO: Always 0?

  u32 dataSector = rootSector + rootSectorCnt;
  u32 dataSectors = header->sectorCountHigh - dataSector;

  u32 *fat = malloc(header->fatSz);
  dev->read(fat, header->fatSz, fatSector, dev);

  struct fat32_dentry *root = malloc(ATA_SECTOR_SIZE), *file; //TODO: Size?
  dev->read(root, ATA_SECTOR_SIZE, rootSector, dev);

  file = &root[2];
  u32 cluster = (file->clusterHigh << 16) | file->clusterLow;
  char name[12] = {'\0'};
  for (u64 i = 0; i < 4; i++)
    name[i] = file->name[2*i + 1];
  printf("\t name=%s, attributes=%X, sz=%z, cluster=%X(%X:%X)\n", name, file->attributes, file->size, cluster, file->clusterHigh, file->clusterLow);

  free(fat);
  free(root);

  errno = EOK;
end:
  free(header);
  return_errno(errno);
}

size_t fat32_RW(struct vfs_file *file, void *buffer, size_t count, bool write, struct io_dev *dev) {
  return 0;
}
