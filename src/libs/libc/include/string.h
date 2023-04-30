#ifndef __STRING_H__
#define __STRING_H__

#include <defines.h>

void strrev(char *s);
char* strcpy(char *dst, const char *src);
void strreplace(char *s, char x, char y);

bool strcmp(const char *x, const char *y); 
bool strncmp(const char *x, const char *y, size_t count); 
size_t strlen(const char *s);

void utoa(u64 n, char *buffer, u8 base);
void itoa(u64 n, char *buffer);
void ztoa(size_t n, char *buffer, u8 unit);

//TODO: String to value
//TODO: Strerror

#endif // __STRING_H__
