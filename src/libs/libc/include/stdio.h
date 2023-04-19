#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdarg.h>
#include <defines.h>

size_t printf(const char *restrict format, ...);
size_t vprintf(const char *restrict format, va_list vlist);
size_t sprintf(char *restrict buffer, const char *restrict format, ...);
size_t vsprintf(char *restrict buffer, const char *restrict format, va_list vlist);

#ifdef KERNEL

void panic(const char *restrict format, ...);

#endif // KERNEL

//TODO: Get
//TODO: Files
//TODO: User-side

#endif // __STDIO_H__
