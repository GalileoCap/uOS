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

u32 ext2_getInodeBlock(struct ext2_inode *inode, u32 block, struct io_dev *dev) {
  if (block < 12) return inode->i_block[block]; //A: Direct
  block -= 12;

  //A: Indirect
  struct ext2_fs *fs = dev->fs;
  u64 pointersPerBlock = fs->blockSz / sizeof(u32),
      ptrsPerSingly = pointersPerBlock,
      ptrsPerDoubly = pow(pointersPerBlock, 2),
      ptrsPerTriply = pow(pointersPerBlock, 3);

  u32 *indirect = malloc(fs->blockSz);
  u32 blockNum;

  if (block < ptrsPerSingly) { //A: Singly indirect
    ext2_readBlocks(indirect, inode->i_block[12], 1, false, dev); //TODO: Cache indirect?
    blockNum = indirect[block];
  } else if ((block - ptrsPerSingly) < ptrsPerDoubly) { //A: Doubly indirect
    block -= ptrsPerSingly;

    ext2_readBlocks(indirect, inode->i_block[13], 1, false, dev);
    blockNum = indirect[block / ptrsPerSingly];
    ext2_readBlocks(indirect, blockNum, 1, false, dev);
    blockNum = indirect[block % ptrsPerSingly];
  } else { //A: Triply indirect
    block -= ptrsPerDoubly + ptrsPerSingly;

    ext2_readBlocks(indirect, inode->i_block[14], 1, false, dev); 
    blockNum = indirect[block / ptrsPerDoubly];
    ext2_readBlocks(indirect, blockNum, 1, false, dev); 
    blockNum = indirect[block % ptrsPerDoubly];
    ext2_readBlocks(indirect, blockNum, 1, false, dev); 

    block -= (block / ptrsPerDoubly) * ptrsPerDoubly;
    blockNum = indirect[block / ptrsPerSingly];
    ext2_readBlocks(indirect, blockNum, 1, false, dev);
    blockNum = indirect[block % ptrsPerSingly];
  }

  free(indirect);
  return blockNum;
}

errno_t ext2_readInodeBlock(struct ext2_inode *inode, void *buffer, u32 block, bool cache, struct io_dev *dev) {
  struct ext2_fs *fs = dev->fs;
  if (ALIGN_UP(inode->i_size, fs->blockSz) < fs->blockSz * block) return_errno(ENODEV); //A: Asking for more than possible //TODO: Better error
  u32 blockNum = ext2_getInodeBlock(inode, block, dev);
  ext2_readBlocks(buffer, blockNum, 1, cache, dev); 
  return_errno(EOK);
}

size_t ext2_readInode(struct ext2_inode *inode, void *buffer, size_t count, size_t offset, struct io_dev *dev) {
  if (inode->i_size < (count + offset)) { errno = ENODEV; return 0; } //A: Asking for more than possible //TODO: Better error

  struct ext2_fs *fs = dev->fs;
  void *_buffer = malloc(fs->blockSz);
  size_t total = 0;

  u32 startBlock = offset / fs->blockSz, endBlock = CEIL(offset + count, fs->blockSz);
  for (u32 block = startBlock; block < endBlock; block++) {
    size_t _count = MIN(count, fs->blockSz);

    if (ext2_readInodeBlock(inode, _buffer, block, false, dev) != EOK) goto end;
    memcpy(buffer, _buffer + offset, _count);

    offset = 0;
    count -= _count;
    buffer += _count;
    total += _count;
  }

end:
  free(_buffer);
  return total;
}

size_t ext2_read(struct vfs_file *file, void *buffer, size_t count, struct io_dev *dev) {
  struct ext2_inode inode;

  if (ext2_find(file, &inode, dev)) {
    return ext2_readInode(&inode, buffer, count, file->offset, dev);
  } else {
    errno = ENODEV;
    return 0;
  }
}

size_t ext2_write(struct vfs_file *file, void *buffer, size_t count, struct io_dev *dev) {
  return 0;
}
