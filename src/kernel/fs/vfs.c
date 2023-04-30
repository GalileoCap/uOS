#include <vfs.h>
#include <ext2.h>
#include <ata.h>

errno_t vfs_initFS(struct io_dev *dev) {
  printf("[vfs_initFS] %u\n", dev->id);
  //TODO: More filesystems
  return ext2_load(dev);
}

struct io_dev* vfs_getIODev(struct vfs_filePath *path) {
  struct io_dev *dev = NULL;

  //TODO: More devices
  for (u64 i = 0; i < 4; i++)
    if (strcmp(path->part, ioDevices[i].mount))
      dev = &ioDevices[i];

  if (dev == NULL) {
    errno = ENODEV;
    return NULL;
  }

  if (dev->fs == NULL && vfs_initFS(dev) != EOK) {
    errno = ENODEV; //TODO: Better error
    return NULL;
  }

  return dev;
}

struct vfs_file *files_head = NULL,
                *files_tail = NULL;

struct vfs_file* getFileByFid(fid_t fid) {
  struct vfs_file *file = files_head;
  while (file != NULL && file->fid != fid)
    file = file->next;
  return file;
}

struct vfs_filePath *splitPathIntoParts(const char *restrict path) {
  char *tmppath = zalloc(strlen(path)+1);
  strcpy(tmppath, path);
  strreplace(tmppath, '/', '\0');

  struct vfs_filePath *res, *part;
  for (size_t i = 1; i < strlen(path); i += strlen(tmppath+i)+1) {
    if (i == 1) {
      part = zalloc(sizeof(struct vfs_filePath));
      res = part;
    } else {
      part->next = zalloc(sizeof(struct vfs_filePath));
      part = part->next;
    }

    part->part = zalloc(strlen(tmppath+i)+1);
    strcpy(part->part, tmppath+i);
  }

  free(tmppath);
  return res;
}

struct vfs_file* newFile(const char *restrict path, u16 mode) {
  if (strlen(path) <= 1 || path[0] != '/')
    return NULL; //TODO: Errno

  struct vfs_file *file = malloc(sizeof(struct vfs_file));
  file->offset = 0;
  file->mode = mode;
  file->path = splitPathIntoParts(path);
  
  if (vfs_getIODev(file->path) == NULL)
    return -1; //A: Check if device exists //TODO: MOve this to newFile

  if (files_head == NULL) {
    file->fid = 3;
    files_head = file;
    files_tail = file;
  } else {
    file->fid = files_tail->fid + 1; //TODO: Alloc fid's
    file->prev = files_tail;

    files_tail->next = file;
    files_tail = file;
  }

  return file;
}

void freeFile(fid_t fid) {
  struct vfs_file *file = getFileByFid(fid);
  
  if (file->prev != NULL)
    file->prev->next = file->next;

  if (file->next != NULL)
    file->next->prev = file->prev;

  if (file == files_head)
    files_head = file->next;
  if (file == files_tail)
    files_tail = file->prev;

  free(file);
}

fid_t vfs_open(const char *restrict path, u16 mode) {
  printf("[vfs_open] \"%s\" %X\n", path, mode);

  struct vfs_file *file = newFile(path, mode);
  if (file == NULL)
    return -1;
  //TODO: Check if file exists or create it if on CREATE mode

  return file->fid;
}

size_t vfs_read(fid_t fid, void *buffer, size_t count) {
  /*printf("[vfs_read] %i %p %z\n", fid, buffer, count);*/
  struct vfs_file *file = getFileByFid(fid);
  if (file == NULL) {
    errno = ENODEV; //TODO: Better error
    return 0;
  }
  //TODO: Permissions and modes

  struct io_dev *dev = vfs_getIODev(file->path);
  if (dev == NULL) {
    errno = ENODEV; //TODO: Better error
    return 0;
  }

  //TODO: More filesystems
  return ext2_RW(file, buffer, count, false, dev);
}

size_t vfs_write(fid_t fid, void *buffer, size_t count) {
  //TODO: Repeated code with vfs_read
  printf("[vfs_write] %i %p %z\n", fid, buffer, count);

  struct vfs_file *file = getFileByFid(fid);
  if (file == NULL) {
    errno = ENODEV; //TODO: Better error
    return 0;
  }
  //TODO: Permissions and modes

  struct io_dev *dev = vfs_getIODev(file->path);
  if (dev == NULL) {
    errno = ENODEV; //TODO: Better error
    return 0;
  }

  //TODO: More filesystems
  return ext2_RW(file, buffer, count, false, dev);
}
