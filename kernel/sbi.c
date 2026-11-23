// sbi.c — making SBI calls to the firmware.
#include "sbi.h"

// Each SBI service has an ID number. The timer service's ID spells "TIME"
// in ASCII letters: T=0x54 I=0x49 M=0x4D E=0x45. (Everything is numbers!)
#define SBI_EXT_TIME      0x54494D45
#define SBI_TIME_SET_TIMER 0

void sbi_set_timer(uint64 when) {
    // The rules for an SBI call: put the service ID in register a7,
    // the function number in a6, the argument in a0, then run `ecall`.
    register uint64 a0 asm("a0") = when;
    register uint64 a6 asm("a6") = SBI_TIME_SET_TIMER;
    register uint64 a7 asm("a7") = SBI_EXT_TIME;
    asm volatile("ecall" : "+r"(a0) : "r"(a6), "r"(a7) : "a1", "memory");
}
