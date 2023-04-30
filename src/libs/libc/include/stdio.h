#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdarg.h>
#include <defines.h>

size_t printf(const char *format, ...);
size_t vprintf(const char *format, va_list vlist);
size_t sprintf(char *buffer, const char *format, ...);
size_t vsprintf(char *buffer, const char *format, va_list vlist);

#ifdef KERNEL

void panic(const char *format, ...);

#endif // KERNEL

//TODO: Get
//TODO: Files
//TODO: User-side

#endif // __STDIO_H__
