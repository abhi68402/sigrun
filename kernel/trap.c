// trap.c — deciding what to do when the CPU "traps".
//
// A TRAP is when the CPU stops normal execution and jumps into the kernel.
// Two kinds:
//   * EXCEPTION: caused by the current instruction (bad memory access,
//                garbage instruction, a deliberate breakpoint...)
//   * INTERRUPT: caused by something outside (timer, keyboard...) — Lesson 2
//
// Without this file, any crash would make the CPU jump to address 0 and the
// kernel would silently freeze. Now we get a clear crash report instead.

#include "trap.h"
#include "riscv.h"
#include "printf.h"
#include "timer.h"

extern void trap_vector(void);   // in trap.S

// The top bit of scause says "this was an interrupt"; the rest is the code.
#define SCAUSE_INTERRUPT (1UL << 63)
#define IRQ_TIMER        5           // interrupt code 5 = supervisor timer

// RISC-V's official list of exception codes (from the RISC-V spec).
static const char *exception_name(uint64 code) {
    switch (code) {
    case 0:  return "Instruction address misaligned";
    case 1:  return "Instruction access fault";
    case 2:  return "Illegal instruction";
    case 3:  return "Breakpoint";
    case 4:  return "Load address misaligned";
    case 5:  return "Load access fault (read from a bad address)";
    case 6:  return "Store address misaligned";
    case 7:  return "Store access fault (wrote to a bad address)";
    case 8:  return "System call from user mode";
    case 9:  return "System call from kernel mode";
    case 12: return "Instruction page fault";
    case 13: return "Load page fault";
    case 15: return "Store page fault";
    default: return "Unknown exception";
    }
}

void trap_init(void) {
    csr_write(stvec, trap_vector);   // "CPU: on any trap, jump here"
}

// How long is the instruction at `addr`? RISC-V has 4-byte instructions
// and (with the "C" extension) compressed 2-byte ones. The lowest 2 bits
// tell us: 0b11 means 4 bytes, anything else means 2 bytes.
static uint64 instruction_length(uint64 addr) {
    uint16 first_half = *(uint16 *)addr;
    return (first_half & 0x3) == 0x3 ? 4 : 2;
}

// Called by trap.S with a pointer to the saved registers.
void kernel_trap(struct trap_frame *tf) {
    uint64 scause = csr_read(scause);
    uint64 sepc   = csr_read(sepc);
    uint64 stval  = csr_read(stval);

    if (scause & SCAUSE_INTERRUPT) {
        uint64 code = scause & ~SCAUSE_INTERRUPT;
        if (code == IRQ_TIMER) {
            timer_tick();   // heartbeat: count it, set the next alarm
            return;         // trap.S restores registers; code continues
        }
        panic("unexpected interrupt, code %ld", code);
    }

    // A breakpoint is harmless: report it, skip the instruction, carry on.
    // Debuggers use exactly this trick to pause programs.
    if (scause == 3) {
        kprintf("[trap] breakpoint at %p -- skipping it and continuing!\n",
                (void *)sepc);
        csr_write(sepc, sepc + instruction_length(sepc));
        return;   // trap.S restores registers and `sret` resumes the code
    }

    // Anything else in the kernel is a real bug: print a crash report.
    kprintf("\n--------------- CRASH REPORT ---------------\n");
    kprintf(" what : %s (scause=%ld)\n", exception_name(scause), scause);
    kprintf(" where: instruction at %p\n", (void *)sepc);
    kprintf(" info : stval=%p\n", (void *)stval);
    kprintf(" regs : ra=%p sp=%p\n", (void *)tf->regs[1], (void *)tf->regs[2]);
    kprintf("        a0=%p a1=%p\n", (void *)tf->regs[10], (void *)tf->regs[11]);
    kprintf("--------------------------------------------\n");
    panic("unrecoverable exception in kernel");
}
