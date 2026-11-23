// printf.h — printing and crashing on purpose.
#pragma once

// Like printf from C, but for the kernel. Supports:
//   %d  number            %x  hex number       %c  one character
//   %ld / %lx  64-bit versions                 %s  string
//   %p  pointer (address)                      %%  a literal %
void kprintf(const char *fmt, ...);

// Print an error message and stop the kernel forever.
// `noreturn` tells the compiler this function never comes back.
__attribute__((noreturn)) void panic(const char *fmt, ...);
