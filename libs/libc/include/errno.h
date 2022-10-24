#ifndef __ERRNO_H__
#define __ERRNO_H__

#include <defines.h>

typedef u32 errno_t;

#define EOK 0 //U: Success
#define EPERM 1 //U: Operation not permited
#define EAGAIN 2 //U: Resource temporarily unavailable
#define EINVAL 3 //U: Invalid argument
#define ENODEV 4 //U: No such device

#define errno (*_errno())
u32* _errno(void);

/*
 * U: Macro for returning with _num errno
 */
#define return_errno(_num) \
  {                        \
    errno = _num;          \
    return -errno;         \
  }

#endif // __ERRNO_H__
