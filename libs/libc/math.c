#include <math.h>

u64 log2(u64 x) {
  u64 res;
  __asm(
    "bsr %[res], %[x]\n\t"
    : [res] "=r" (res)
    : [x] "r" (x)
  );
  return res;
}

u64 log2c(u64 x) {
  u64 right, left;
  __asm(
    "bsr %[right], %[x]\n\t"
    "bsf %[left], %[x]\n\t"
    : [right] "=r" (right), [left] "=r" (left)
    : [x] "r" (x)
  );
  return right + (u64)(right != left);
}

u64 pow2(u64 y) {
  return 1 << y;
}
