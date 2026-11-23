// trap.h — catching crashes and special events.
#pragma once
#include "types.h"

// All 32 CPU registers, saved by trap.S. regs[N] holds register xN.
// Nicknames: x1=ra (return address), x2=sp (stack pointer),
//            x10..x17 = a0..a7 (function arguments)
struct trap_frame {
    uint64 regs[32];
};

void trap_init(void);   // tell the CPU where our trap handler lives
