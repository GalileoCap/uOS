#include <memory.h>

void memset(char *at, char value, size_t count) {
  for (size_t i = 0; i < count; i++)
    at[i] = value;
}
