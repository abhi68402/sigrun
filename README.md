# Sigrun — a small RISC-V operating system, built from scratch

*Named after Sigrún, a Valkyrie from Norse myth. Valkyries were "choosers",
and an OS kernel's job is choosing too: which program runs next, and what
each one is allowed to touch.*

Sigrun is an operating system kernel written in **C** for **64-bit RISC-V**, running
in the **QEMU** emulator. No libraries, no existing OS underneath: every
piece, from printing text to catching crashes, is written here.

## The idea: consent faults
**Permission works like memory.** Nothing is granted up front. The first time
any program or AI agent touches something new, the CPU traps, Sigrun freezes
it mid-instruction, asks you, and resumes the exact same instruction if you
allow it.

```
⏸  agent "organizer" was frozen while trying to:
     DELETE  downloads/old.zip
   [allow once]  [allow for this task]  [deny]  [deny + stop agent]
```

Existing "ask before acting" systems for AI agents work inside the app, so
the agent has to choose to ask. In Sigrun the **hardware forces the
question**, so it works on any program, unmodified.

## What works today
- Boots on QEMU's `virt` machine via OpenSBI firmware
- Serial console driver (UART, memory-mapped I/O)
- Kernel `printf` and `panic`
- Trap handling: crashes produce a readable report; breakpoints are recovered from
- A small command console (`help`, `info`, `break`, `crash`, `illegal`)

```
> crash
Reading from 0xdead0000, where no memory exists...

--------------- CRASH REPORT ---------------
 what : Load access fault (read from a bad address) (scause=5)
 where: instruction at 0x802002c2
 info : stval=0xdead0000
--------------------------------------------
```

## Run it (macOS)
```bash
brew install qemu riscv64-elf-gcc riscv64-elf-gdb
make run          # quit QEMU with Ctrl-A, then X
```

| Command | What it does |
|---|---|
| `make` | build `build/kernel.elf` |
| `make run` | boot in QEMU |
| `make debug` + `make gdb` | step through the kernel with GDB |
| `make clean` | remove build output |

## Project layout
```
kernel/
  entry.S     first instructions: set up the stack, jump to C
  kernel.ld   linker script: where everything goes in memory
  main.c      kmain() and the command console
  uart.c      serial port driver
  printf.c    kprintf / panic
  trap.S      saves/restores registers on a trap
  trap.c      decides what to do with each trap
lessons/      a beginner-friendly write-up of every milestone
PROGRESS.md   roadmap
```

## What makes it different (planned)
1. **Consent faults**: the CPU freezes a program the moment it touches something new; Sigrun asks you, then resumes it.
2. **Zero access by default**: every program and agent starts with no keys; there is no all-powerful root user.
3. **Least privilege for free**: Sigrun records exactly which keys a program needed and can lock it to that list.
4. **Boots in a browser tab**: try it from a link, nothing to install.
5. **Documented as lessons**: every milestone is explained in `lessons/`.

New to operating systems? Start with [lessons/START-HERE.md](lessons/START-HERE.md).
See [PROGRESS.md](PROGRESS.md) for the roadmap.
