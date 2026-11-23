// timer.c — the kernel's heartbeat.
//
// THE PROBLEM: once a program is running on the CPU, the kernel isn't.
// If that program never gives the CPU back (say, it's stuck in a loop),
// the kernel never gets to act again.
//
// THE FIX: ask the hardware to interrupt the CPU on a schedule. Every
// 10 milliseconds the timer "rings", the CPU traps into the kernel, and
// the kernel gets control back — no matter what was running.
// Each ring is called a "tick".

#include "timer.h"
#include "sbi.h"
#include "riscv.h"

// The clock counts 10,000,000 times per second on QEMU's virt machine.
#define CLOCK_HZ       10000000UL
#define TICK_INTERVAL  (CLOCK_HZ / TICKS_PER_SECOND)   // clock counts per tick

// `volatile`: this changes "by itself" (inside the interrupt handler),
// so the compiler must re-read it every time instead of assuming.
static volatile uint64 ticks = 0;

// Set the alarm for one tick from now.
static void set_next_alarm(void) {
    sbi_set_timer(read_time() + TICK_INTERVAL);
}

void timer_init(void) {
    set_next_alarm();               // 1. set the first alarm
    csr_set(sie, SIE_STIE);         // 2. allow timer interrupts
    csr_set(sstatus, SSTATUS_SIE);  // 3. flip the master switch: interrupts ON
}

void timer_tick(void) {
    ticks++;                        // count the beat
    set_next_alarm();               // set the alarm for the next one
}

uint64 timer_ticks(void) {
    return ticks;
}
