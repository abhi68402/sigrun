// string.c — our own mini string library (there's no libc in a kernel!).
#include "string.h"

size_t strlen(const char *s) {
    size_t n = 0;
    while (s[n]) n++;
    return n;
}

// Returns 0 if the two strings are equal.
int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

// Fill n bytes at dst with value. (The compiler sometimes calls this
// behind our back to zero out arrays, so we MUST provide it.)
void *memset(void *dst, int value, size_t n) {
    uint8 *p = dst;
    while (n--) *p++ = (uint8)value;
    return dst;
}
