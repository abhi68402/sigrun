// riscv.h — helpers for talking to special CPU registers.
//
// Besides normal registers (a0, sp, ra...), RISC-V has "CSRs":
// Control and Status Registers. They control the CPU itself.
// The ones we use for traps (the "s" means Supervisor mode = kernel mode):
//
//   stvec   - "trap vector": address of the code to run when a trap happens
//   scause  - WHY the trap happened (a number, see trap.c)
//   sepc    - WHERE it happened (address of the instruction that trapped)
//   stval   - extra info, e.g. the bad address we tried to read
#pragma once
#include "types.h"

// Read a CSR, e.g.  uint64 x = csr_read(scause);
#define csr_read(name) ({                                   \
    uint64 _v;                                              \
    asm volatile("csrr %0, " #name : "=r"(_v));             \
    _v;                                                     \
})

// Write a CSR, e.g.  csr_write(stvec, trap_vector);
#define csr_write(name, value) \
    asm volatile("csrw " #name ", %0" : : "r"((uint64)(value)))
