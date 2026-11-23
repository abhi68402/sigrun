# OS — a small RISC-V operating system, built from scratch

A hobby operating system kernel written in **C** for **64-bit RISC-V**, running
in the **QEMU** emulator. No libraries, no existing OS underneath: every
piece, from printing text to catching crashes, is written here.

**End goal:** a capability-secure OS that runs sandboxed WebAssembly apps and
boots right in your browser.

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
1. **Apps are WebAssembly**: sandboxed, and the same app file runs on any CPU.
2. **Capability security**: apps can only touch what they're explicitly given; there is no all-powerful root user.
3. **Boots in a browser tab**: try it from a link, nothing to install.
4. **Documented as lessons**: every milestone is explained in `lessons/`.

See [PROGRESS.md](PROGRESS.md) for the roadmap.
