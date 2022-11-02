#ifndef __VFS_H__
#define __VFS_H__

#include <utils.h>

#define VFS_FSTYPE_UNKN 0x00
#define VFS_FSTYPE_EXT2 0x01

#define VFS_MODE_READ   0x0001
#define VFS_MODE_WRITE  0x0002
#define VFS_MODE_APPEND 0x0004
#define VFS_MODE_CREATE 0x0008

typedef u64 fid_t;

struct vfs_file {
  char *path;
  size_t offset;
  u16 mode;
};

fid_t vfs_open(const char *restrict path, u16 mode);
size_t vfs_read(fid_t fid, void *buffer, size_t count);

#endif // __VFS_H__
