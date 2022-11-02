#ifndef __EXT2_H__
#define __EXT2_H__

#include <vfs.h>
#include <ata.h>
#include <utils.h>

#define EXT2_SBSEGMENT 2
#define EXT2_ROOT_INODE 2

#define EXT2_MAGIC 0xEF53

#define EXT2_IGNORE  1
#define EXT2_REMOUNT 2
#define EXT2_PANIC   3

#define EXT2_TYPE_FIFO  0x1000
#define EXT2_TYPE_CHAR  0x2000
#define EXT2_TYPE_DIR   0x4000
#define EXT2_TYPE_BLOCK 0x6000
#define EXT2_TYPE_FILE  0x8000
#define EXT2_TYPE_SYML  0xA000
#define EXT2_TYPE_SOCK  0xC000

#define EXT2_PERM_OEXEC 0x0001
#define EXT2_PERM_OWRIT 0x0002
#define EXT2_PERM_OREAD 0x0004
#define EXT2_PERM_GEXEC 0x0008
#define EXT2_PERM_GWRIT 0x0010
#define EXT2_PERM_GREAD 0x0020
#define EXT2_PERM_UEXEC 0x0040
#define EXT2_PERM_UWRIT 0x0080
#define EXT2_PERM_UREAD 0x0100
#define EXT2_PERM_STICK 0x0200
#define EXT2_PERM_STGID 0x0400
#define EXT2_PERM_STUID 0x0800
#define EXT2_PERM_DFLT (EXT2_PERM_OREAD | EXT2_PERM_GREAD | EXT2_PERM_UREAD)

#define EXT2_CACHE_COUNT 256

struct ext2_superblock {
  u32 totalInodes; //U: Total number of inodes in the filesystem
  u32 totalBlocks; //U: Total number of blocks in the filesystem
  u32 suBlockCnt; //U: Number of blocks reserved for superuser
  u32 freeBlocks; //U: Number of unallocated blocks
  u32 freeInodes; //U: Number of unallocated inodes
  u32 superblockBlock; //U: Block number of the superblock
  u32 blockSz; //U: Block size
  u32 fragSz; //U: Fragment size
  u32 bPerBG; //U: Number of blocks in each block group
  u32 fPerBG; //U: Number of fragments in each block group
  u32 iPerBG; //U: Number of inodes in each block group
  u32 mountT; //U: Last mount time (POSIX)
  u32 writeT; //U: Last write time (POSIX)
  u16 mounts; //U: Number of times the volume's been mounted since last consistency check
  u16 mountsMax; //U: Number of mountes allowed before a consistency check
  u16 sign; //U: 0xEF53, signature
  u16 state; //U: File system state
  u16 action; //U: What to do when an error is detected
  u16 minor; //U: Minor portion of version
  u32 lastCheck; //U: Last check time (POSIX)
  u32 forceCheck; //U: Interval between forced checks (POSIX)
  u32 osID; //U: ID of the OS from which it was created
  u32 major; //U: Major portion of version
  u16 suid; //U: User ID that can use reserved blocks
  u16 sgid; //U: Group ID that can use reserved blocks

  //NOTE: These are only present if major >= 1
  u32 firstInode; //U: First non-reserved inode
  u16 inodeSz;
  u16 blockGroup; //U: If backup copy, block group this superblock is part of
  u32 optional; //U: Optional features present //TODO
  u32 required; //U: Required features present //TODO
  u32 minimum; //U: Features present or read-only //TODO
  u64 fsID[2]; //U: Filesystem ID (blkid)
  char name[16]; //NOTE: Null-terminated
  char lastMount[64]; //U: Last mount path //NOTE: Null-terminated
  u32 compression; //U: Compression algorithms used //TODO
  u8 blocksPreFile; //U: Number of blocks to preallocate for files //TODO
  u8 blocksPreDir; //U: Number of blocks to preallocate for directories //TODO
  u8 ign0[2];
  u64 journalID[2];
  u32 journalInode;
  u32 journalDevice;
  u32 orphans; //U: Head of oprhan inode list
  u8 ign1[788];
} __attribute__((packed));

struct ext2_blockGroup { //U: Block Group Descriptor
  u32 bUsageBM; //U: Block address of block usage bitmap
  u32 iUsageBM; //U: Block address of inode usage bitmap
  u32 table; //U: Block address of inode table
  u16 freeBlocks;
  u16 freeInodes;
  u16 dirs;
};

struct ext2_inode {
  u16 i_mode; //U: Type and permissions //TODO
  u16 i_uid;
  u32 i_size; //U: Lower 32-bits of size
  u32 i_atime; //U: POSIX time
  u32 i_ctime;
  u32 i_mtime;
  u32 i_dtime;
  u16 i_gid;
  u16 i_links_count;
  u32 i_blocks; //U: Count of disk sectors used by this inode (not counting this structure or directory entries linking to the inode)
  u32 i_flags; //TODO
  u32 i_osd1; //U: OS specific value //TODO
  u32 i_block[15];
  u32 i_generation; //TODO
  u32 i_file_acl; //NOTE: Since v1.0 //TODO
  u32 i_dir_acl; //U: Higher 32-bits of size //NOTE: Since v1.0
  u32 i_faddr; //U: Block address of fragment
  u32 i_osd2[3]; //TODO
  //u16 extraSz;
  //u16 checkHi;
  //u32 ctimeExtra;
  //u32 mtimeExtra;
  //u32 atimeExtra;
  //u32 crtime;
  //u32 crtimeExtra;
  //u32 verHi;
  //u32 projid;
  //u64 pad[13];
};

struct ext2_dir {
  u32 inode;
  u16 size; //U: Total size including subfields
  u8 nameLen; //U: Name length least-significant 8-bits
  u8 type; //NOTE: Only if feature is set //TODO
  char name[];
};

struct ext2_block{
  u32 block;
  u8 *data;
};

struct ext2_fs {
  u32 totalBlocks; //U: Total number of blocks in the filesystem
  u32 totalInodes; //U: Total number of inodes in the filesystem
  u32 freeBlocks; //U: Number of unallocated blocks
  u32 freeInodes; //U: Number of unallocated inodes
  u32 sbBlock; //U: Block number of the superblock
  u32 blockSz;
  u16 inodeSz;
  u32 bPerBG; //U: Number of blocks in each block group
  u32 iPerBG; //U: Number of inodes in each block group
  u32 bgCount;

  u8 blocksPreFile; //U: Number of blocks to preallocate for files //TODO
  u8 blocksPreDir; //U: Number of blocks to preallocate for directories //TODO

  u32 optional; //U: Optional features present //TODO
  u32 required; //U: Required features present //TODO
  u32 minimum; //U: Features present or read-only //TODO
  u32 compression; //U: Compression algorithms used //TODO

  u16 minor; //U: Minor portion of version
  u32 major; //U: Major portion of version
  u64 id[2]; //U: Filesystem ID (blkid)
  char name[16]; //NOTE: Null-terminated

  struct ext2_block cache[EXT2_CACHE_COUNT]; //TODO: Number is arbitrary
};

errno_t ext2_load(struct io_dev *dev);
size_t ext2_read(struct vfs_file *file, void *buffer, size_t count, struct io_dev *dev);
size_t ext2_write(struct vfs_file *file, void *buffer, size_t count, struct io_dev *dev);

#endif // __EXT2_H__
