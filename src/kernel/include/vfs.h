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
#define FID_INVALID ((fid_t)-1)

struct vfs_filePath {
  char *part;
  struct vfs_filePath *next;
};

struct vfs_file {
  fid_t fid;
  struct vfs_filePath *path;
  size_t offset;
  u16 mode;

  struct vfs_file *prev, *next;
};

fid_t vfs_open(const char *path, u16 mode);
size_t vfs_read(fid_t fid, void *buffer, size_t count);
size_t vfs_write(fid_t fid, void *buffer, size_t count);

#endif // __VFS_H__
