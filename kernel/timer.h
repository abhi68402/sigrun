// timer.h — the kernel's heartbeat.
#pragma once
#include "types.h"

#define TICKS_PER_SECOND 100    // the heartbeat beats 100 times per second

void   timer_init(void);        // start the heartbeat
void   timer_tick(void);        // called by the trap handler on every beat
uint64 timer_ticks(void);       // how many beats since boot
