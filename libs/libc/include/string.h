#ifndef __STRING_H__
#define __STRING_H__

#include <defines.h>

void strrev(char *restrict s);
char* strcpy(char *restrict dst, const char *restrict src);
void strreplace(char *restrict s, char x, char y);

bool strcmp(const char *restrict x, const char *restrict y); 
bool strncmp(const char *restrict x, const char *restrict y, size_t count); 
size_t strlen(const char *restrict s);

void utoa(u64 n, char *restrict buffer, u8 base);
void itoa(u64 n, char *restrict buffer);
void ztoa(u64 n, char *restrict buffer, u8 unit);

//TODO: String to value
//TODO: Strerror

#endif // __STRING_H__
