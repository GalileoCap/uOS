#include <errno.h>

errno_t error = EOK;

errno_t* _errno(void) {
  return &error;
}
