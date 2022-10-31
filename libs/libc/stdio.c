#include <stdio.h>
#include <string.h>

size_t vsprintf(char *restrict bout, const char *restrict format, va_list vlist) {
  //TODO: Flags
  //TODO: n$
  size_t res = 0;
  char buffer[72], *tmp; //NOTE: Long enough for anything
  for (u64 i = 0; format[i]; i++) {
    buffer[0] = '\0'; //A: Default to zero length
    if (format[i] != '%') bout[res++] = format[i];
    else switch (format[++i]) {
      case 'd':
      case 'i':
        itoa(va_arg(vlist, u64), buffer);
        if (bout != NULL) strcpy(bout + res, buffer);
        break;

      case 'u':
        utoa(va_arg(vlist, u64), buffer, 10);
        if (bout != NULL) strcpy(bout + res, buffer);
        break;

      case 'z':
        ztoa(va_arg(vlist, u64), buffer, -1);
        if (bout != NULL) strcpy(bout + res, buffer);
        break;

      case 'x': //TODO: Lower-case
      case 'X':
        utoa(va_arg(vlist, u64), buffer, 16);
        if (bout != NULL) strcpy(bout + res, buffer);
        break;

      case 'p':
        utoa((u64)va_arg(vlist, void*), buffer, 16); //TODO: Treat as %016X
        if (bout != NULL) strcpy(bout + res, buffer);
        break;

      //TODO: floats

      case 's':
        tmp = va_arg(vlist, char*);
        if (bout != NULL) strcpy(bout + res, tmp);
        res += strlen(tmp);
        break;

      case 'c':
        if (bout != NULL) bout[res] = (char)va_arg(vlist, int);
        res++;
        break;

      case '%': 
      default:
        if (bout != NULL) bout[res] = format[i];
        res++;
        break;
    }
    res += strlen(buffer);
  }
  res++;
  if (bout != NULL) bout[res] = '\0';
  return res;
}

size_t sprintf(char *restrict buffer, const char *restrict format, ...) {
  va_list vlist;
  va_start(vlist, format);
  size_t res = vsprintf(buffer, format, vlist);
  va_end(vlist);
  return res;
}

#ifdef KERNEL

#include <serial.h>
#include <i686.h>

size_t vprintf(const char *restrict format, va_list vlist) {
  char buffer[0x500]; //TODO: kmalloc enough space
  size_t res = vsprintf(buffer, format, vlist);
  serial_puts(COM1, buffer, res-1);
  return res;
}

size_t printf(const char *restrict format, ...) {
  va_list vlist;
  va_start(vlist, format);
  size_t res = vprintf(format, vlist);
  va_end(vlist);
  return res;
}

void panic(const char *restrict format, ...) {
  cli();
  //TODO: Print on screen
  char buffer[0x500];
  printf("\t---PANIC---\t\n");

  va_list vlist;
  va_start(vlist, format);
  size_t res = vsprintf(buffer, format, vlist);
  serial_puts(COM1, buffer, res-1);
  va_end(vlist);

  printf("\t---END OF PANIC---\t\n");

  //TODO: Reboot
  while (true);
}

#else // USER

size_t printf(const char *restrict format, ...);
size_t vprintf(const char *restrict format, va_list vlist);

#endif // KERNEL
