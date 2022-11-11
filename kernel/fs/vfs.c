#include <vfs.h>
#include <ext2.h>
#include <ata.h>

#define VFS_MAXFILES 1

struct vfs_file* files[VFS_MAXFILES] = {NULL}; //TODO: Arbitrary number of files

errno_t vfs_initFS(struct io_dev *dev) {
  printf("[vfs_initFS] %u\n", dev->id);
  //TODO: More filesystems
  return ext2_load(dev);
}

struct io_dev* vfs_getIODev(const char *restrict path) {
  struct io_dev *dev = NULL;
  char *tmppath = malloc(strlen(path)+1);
  strcpy(tmppath, path);
  strreplace(tmppath, '/', '\0');

  //TODO: More devices
  for (u64 i = 0; i < 4; i++)
    if (strcmp(tmppath+1, ioDevices[i].mount)) dev = &ioDevices[i];

  if (dev == NULL) { errno = ENODEV; goto end; }

  if (dev->fs == NULL && vfs_initFS(dev) != EOK) return NULL;

end:
  free(tmppath);
  return dev;
}

fid_t vfs_open(const char *restrict path, u16 mode) {
  /*printf("[vfs_open] %s. %X\n", path, mode);*/
  //TODO: Check if file exists or create it if on CREATE mode
  //TODO: Anything else?

  for (fid_t fid = 0; fid < VFS_MAXFILES; fid++)
    if (files[fid] == NULL) {
      files[fid] = malloc(sizeof(struct vfs_file));
      *files[fid] = (struct vfs_file){
        .offset = 0,
        .mode = mode,
        .path = malloc(strlen(path)+1)
      };
      strcpy(files[fid]->path, path);
      return fid;
    }

  return -1;
}

size_t vfs_read(fid_t fid, void *buffer, size_t count) {
  /*printf("[vfs_read] %i %p %z\n", fid, buffer, count);*/
  if (fid > VFS_MAXFILES || files[fid] == NULL) { errno = ENODEV; return 0; } //TODO: Better error
  //TODO: Permissions and modes

  struct io_dev *dev = vfs_getIODev(files[fid]->path);
  if (dev == NULL) { errno = ENODEV; return 0; } //TODO: Better error

  //TODO: More filesystems
  return ext2_read(files[fid], buffer, count, dev);
}

size_t vfs_write(fid_t fid, void *buffer, size_t count) {
  //TODO: Repeated code
  printf("[vfs_write] %i %p %z\n", fid, buffer, count);
  if (fid > VFS_MAXFILES || files[fid] == NULL) { errno = ENODEV; return 0; } //TODO: Better error
  //TODO: Permissions and modes

  struct io_dev *dev = vfs_getIODev(files[fid]->path);
  if (dev == NULL) { errno = ENODEV; return 0; } //TODO: Better error

  //TODO: More filesystems
  return ext2_write(files[fid], buffer, count, dev);
}
