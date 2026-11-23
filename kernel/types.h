// types.h — short names for integer sizes.
// In a normal program you'd #include <stdint.h>. We have no standard
// library, so we define our own. The number = how many bits.
#pragma once

typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long      uint64;   // on 64-bit RISC-V, long is 64 bits
typedef long               int64;
typedef uint64             size_t;

#define NULL ((void *)0)
