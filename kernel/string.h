// string.h — tiny versions of the C string functions we need.
#pragma once
#include "types.h"

size_t strlen(const char *s);
int    strcmp(const char *a, const char *b);
void  *memset(void *dst, int value, size_t n);
