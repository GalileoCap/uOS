#include <errno.h>

u32 error = EOK;

u32* _errno(void) {
  return &error;
}
