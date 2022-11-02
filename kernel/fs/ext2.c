#include <ext2.h>
#include <string.h>
#include <vfs.h>

u64 block2Sector(u64 block, u64 blockSz) {
  return (block * blockSz) / ATA_SECTOR_SIZE;
}

u64 inode2BG(u64 inode, u64 iPerBG) {
  return (inode - 1) / iPerBG;
}

u64 inode2IDX(u64 inode, struct ext2_fs *fs) {
  return (inode - 1) % fs->iPerBG;
}

u64 inode2Containing(u64 inode, u64 inodeSz, u64 blockSz) {
  return (inode * inodeSz) / blockSz;
}

struct ext2_dir* nextDirEntry(struct ext2_dir *entry, size_t accum) {
  if ((entry->size + accum) < 1024) //TODO: Blocksz
    return (struct ext2_dir*)(((void*)entry) + entry->size);
  else return NULL;
}

struct ext2_dir* nextFreeDirEntry(struct ext2_dir *entry) {
  size_t accum = 0;
  while (nextDirEntry(entry, accum) != NULL) {
    entry = nextDirEntry(entry, accum);
    accum += entry->size;
  }
  accum = entry->size;
  entry->size = ALIGN_UP(8 + entry->nameLen, 4);
  accum -= entry->size;
  entry = (struct ext2_dir*)(((void*)entry) + entry->size);
  entry->size = accum;
  return entry;
}

u64 getNextBM(u64 *bitmap) {
  u64 i = 0;
  u64 _bitmap = bitmap[0]; //TODO: Check the rest 
  while ((_bitmap & 1) != 0) { _bitmap >>= 1; i++; } //TODO
  bitmap[0] |= 1l << i; //A: Set it
  return i+1;
}

struct ext2_dir* ext2_GetDir(struct ext2_dir *entry, char name[]) {
  size_t accum = 0;
  while (entry != NULL) {
    accum += entry->size;
    if (strcmp(entry->name, name)) return entry;
    entry = nextDirEntry(entry, accum);
  }
  return NULL;
}

errno_t ext2_load(struct io_dev *dev) {
  printf("[ext2_parse] dev=%u\n", dev->id);

  struct ext2_superblock sb;
  if (dev->read(&sb, sizeof(struct ext2_superblock), EXT2_SBSEGMENT, dev) != sizeof(struct ext2_superblock))
    return_errno(ENODEV); //TODO: Better errors

  if (sb.sign != EXT2_MAGIC) return_errno(ENODEV); 
  if (sb.state != 1) { //A: System has errors
    printf("[ext2_load] System has errors, action: ");
    switch (sb.action) {
      case EXT2_IGNORE: printf("Ignoring\n"); break;
      case EXT2_REMOUNT: printf("TODO: Handle EXT2_REMOUNT\n"); return_errno(ENODEV);
      case EXT2_PANIC: printf("TODO: Handle EXT2_PANIC\n"); return_errno(ENODEV);
      default: printf("TODO: Handle EXT2_UNKNOWN\n"); return_errno(ENODEV);
    }
  }

  dev->fs = malloc(sizeof(struct ext2_fs));
  struct ext2_fs *fs = dev->fs;
  fs->totalInodes = sb.totalInodes;
  fs->totalBlocks = sb.totalBlocks;
  fs->freeInodes = sb.freeInodes;
  fs->freeBlocks = sb.freeBlocks;
  fs->blockSz = 1024l << sb.blockSz;
  fs->inodeSz = sb.inodeSz;
  fs->bPerBG = sb.bPerBG;
  fs->iPerBG = sb.iPerBG;
  fs->bgCount = CEIL(fs->totalBlocks, fs->bPerBG);

  fs->blocksPreFile = sb.blocksPreFile;
  fs->blocksPreDir = sb.blocksPreDir;

  fs->optional = sb.optional;
  fs->required = sb.required;
  fs->minimum = sb.minimum;
  fs->compression = sb.compression;

  fs->minor = sb.minor;
  fs->major = sb.major;
  memcpy(fs->id, sb.fsID, sizeof(fs->id));
  memcpy(fs->name, sb.name, sizeof(fs->name));
  
  for (int i = 0; i < EXT2_CACHE_COUNT; i++)
    fs->cache[i] = (struct ext2_block){
      .block = -1,
      .data = malloc(fs->blockSz)
    };
  
  printf("[ext2_load] SUCCESFULL name=%s, version=%u.%u id=%X%X\n", fs->name, fs->major, fs->minor, fs->id[0], fs->id[1]);
  printf("\t* minimum=%X, required=%X, optional=%X\n", fs->minimum, fs->required, fs->optional);
  printf("\t* inodes=%u/%u(%z), blocks=%u/%u(%z)\n", fs->freeInodes, fs->totalInodes, fs->inodeSz, fs->freeBlocks, fs->totalBlocks, fs->blockSz);

  return_errno(EOK);
}

void ext2_readBlocks(void *buffer, u32 from, u32 count, bool cache, struct io_dev *dev) {
  /*printf("[ext2_readBlocks] %u %u %c\n", from, count, cache ? 'T' : 'F');*/

  struct ext2_fs *fs = dev->fs;
  void *_buffer = malloc(fs->blockSz);

  for (u32 block = from; block < (from + count); block++) {
    //A: Search for this block or a free cache
    u64 idx = -1, lastFree = -1;
    for (u64 i = 0; i < EXT2_CACHE_COUNT; i++)
      if (fs->cache[i].block == block) idx = i;
      else if (fs->cache[i].block == -1) lastFree = i;

    if (idx != -1) memcpy(buffer, fs->cache[idx].data, fs->blockSz); //A: In cache, simply copy
    else { //A: Not in cache, get it
      if (cache) { //A: Save to cache
        if (lastFree == -1) lastFree = 0; //TODO: Replacement scheme
        idx = lastFree;

        fs->cache[idx].block = block;
        dev->read(fs->cache[idx].data, fs->blockSz, block2Sector(block, fs->blockSz), dev); //TODO: Error
        memcpy(buffer, fs->cache[idx].data, fs->blockSz);
      } else dev->read(buffer, fs->blockSz, block2Sector(block, fs->blockSz), dev); //A: Don't save to cache //TODO: Error
    }

    buffer += fs->blockSz;
  }

  free(_buffer);
}

bool ext2_find(struct vfs_file *file, struct ext2_inode *inode, struct io_dev *dev) {
  struct ext2_fs *fs = dev->fs;
  u32 firstBGBlock = (fs->blockSz == 1024 ? 2 : 1); //TODO
  struct ext2_blockGroup *bg = malloc(fs->blockSz); //TODO: Sizeof
  ext2_readBlocks(bg, firstBGBlock, 1, true, dev);
  void *table = calloc(fs->iPerBG, fs->inodeSz);
  ext2_readBlocks(table, bg->table, fs->iPerBG * fs->inodeSz / fs->blockSz, true, dev);
  *inode = *((struct ext2_inode*)(table + fs->inodeSz * inode2IDX(EXT2_ROOT_INODE, fs)));

  void *dir = malloc(fs->blockSz);
  ext2_readBlocks(dir, inode->i_block[0], 1, true, dev);
  struct ext2_dir *entry = ext2_GetDir(dir, "readme");
  ext2_readBlocks(bg, firstBGBlock, 1, true, dev); //TODO: BG
  ext2_readBlocks(table, bg->table, fs->iPerBG * fs->inodeSz / fs->blockSz, true, dev);
  *inode = *((struct ext2_inode*)(table + fs->inodeSz * inode2IDX(entry->inode, fs)));

  //TODO: Follow path

  return true;
}

size_t ext2_readInode(struct ext2_inode *inode, void *buffer, size_t count, size_t offset, struct io_dev *dev) {
  struct ext2_fs *fs = dev->fs;
  void *_buffer = malloc(fs->blockSz);
  ext2_readBlocks(_buffer, inode->i_block[0], 1, false, dev);
  memcpy(buffer, _buffer, count);
  return 0;
}

size_t ext2_read(struct vfs_file *file, void *buffer, size_t count, struct io_dev *dev) {
  struct ext2_inode inode;

  if (ext2_find(file, &inode, dev)) {
    return ext2_readInode(&inode, buffer, count, file->offset, dev);
  } else errno = ENODEV;
  return 0;
}

size_t ext2_write(struct vfs_file *file, void *buffer, size_t count, struct io_dev *dev) {
  return 0;
}
