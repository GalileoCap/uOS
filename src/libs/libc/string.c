#include <string.h>
#include <errno.h>

size_t strlen(const char *restrict s) {
  size_t i = 0;
  while (s[i]) i++;
  return i;
}

void strrev(char *s) {
  for (char *end = s + strlen(s) - 1; s < end; s++, end--) {
    char c = *s;
    *s = *end;
    *end = c;
  }
}

char* strcpy(char *restrict dst, const char *restrict src) {
  size_t i = 0;
  do {
    dst[i] = src[i];
  } while (src[i++]);
  return dst;
}

bool strncmp(const char *restrict x, const char *restrict y, size_t count) {
  size_t i = 0;
  do {
    if (x[i] != y[i]) return false;
    i++;
  } while (i < count && x[i] && y[i]);
  return i == count || (x[i] == '\0' && y[i] == '\0');
}

bool strcmp(const char *restrict x, const char *restrict y) {
  return strncmp(x, y, -1);
}

void strreplace(char *restrict s, char x, char y) {
  while (*s) {
    if (*s == x) *s = y;
    s++;
  }
}

void utoa(u64 n, char *restrict out, u8 base) {
  if (base == 0 || base > 16) { errno = EINVAL; return; }

  char format[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  size_t i = 0;

  do {
    out[i++] = format[n % base];
    n /= base;
  } while (n > 0);
  out[i] = '\0';
  strrev(out);
}

void itoa(u64 n, char *restrict out) {
  bool neg = n < 0;
  if (neg) n = -n;

  utoa(n, out + neg, 10);
  if (neg) out[0] = '-';
}

void ptoa(u64 n, char *restrict out) {
  out[0] = '0'; out[1] = 'x';
  out += 2;

  u64 pad = 64 / 4, tmp = n;
  while (tmp > 0) {
    tmp >>= 4;
    pad--;
  }
  while (pad-- > 0) {
    *out++ = '0';
  }

  utoa(n, out, 16);
}

void ztoa(size_t n, char *restrict buffer, u8 unit) {
  if (unit != (u8)-1 && unit > 4) { errno = EINVAL; return; }

  char* units[] = {"B", "KiB", "MiB", "GiB", "TiB"}; //TODO: More units
  
  size_t tmp = n;
  if (unit == (u8)-1) { //A: Largest non-zero unit
    do {
      tmp /= 1024;
      unit++;
    } while (tmp != 0);
  }

  for (u8 i = 0; i < unit; i++) n /= 1024;
  utoa(n, buffer, 10);

  strcpy(buffer + strlen(buffer), units[unit]);
}
