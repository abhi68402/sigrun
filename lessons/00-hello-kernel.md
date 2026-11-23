# Lesson 0 — Your First Kernel

> Goal: understand what happens from "power on" to "Hello from my own kernel!"
> No prior OS knowledge needed.

---

## 1. What is an operating system, really?

Your computer has hardware: a **CPU** (does math, runs instructions), **RAM**
(short-term memory), and **devices** (keyboard, screen, disk, network).

Normal programs (Chrome, Python, games) **never touch hardware directly**.
They ask the OS: "please print this", "please give me memory", "please read
this file". The OS is the program that actually talks to the hardware and
shares it fairly between all other programs.

```
   Your apps   (Chrome, Spotify, games)
       │  "please do X for me"   ← called a "system call"
       ▼
   OPERATING SYSTEM (the kernel)   ← what we're building
       │  reads/writes hardware directly
       ▼
   Hardware   (CPU, RAM, keyboard, screen, disk)
```

The **kernel** is the core of the OS: the part that runs with full power over
the hardware.

---

## 2. Our choices (and why)

| Choice | What we picked | Why |
|---|---|---|
| Language | **C** | Lets us control memory exactly; most OSes (Linux, Windows core, macOS core) are written in C. |
| CPU type | **RISC-V** (64-bit) | A modern, clean CPU design. Much simpler to start on than Intel/AMD x86, which carries 40 years of baggage. |
| Machine | **QEMU** | A fake computer that runs inside your Mac. If our kernel crashes, nothing bad happens — just restart it. |

Your Mac has an ARM (Apple Silicon) CPU, not RISC-V. So we need:
- a **cross-compiler** (`riscv64-elf-gcc`): runs on your Mac, but produces RISC-V machine code.
- an **emulator** (`qemu-system-riscv64`): pretends to be a RISC-V computer.

---

## 3. The boot journey, step by step

```
 You type `make run`
      │
      ▼
 QEMU creates a fake RISC-V computer with 128 MB of RAM
      │
      ▼
 OpenSBI firmware runs (like a BIOS — prints the big OpenSBI logo)
      │   jumps to address 0x80200000
      ▼
 entry.S  — sets up a stack, then calls kmain()
      │
      ▼
 main.c   — kmain() prints "Hello" through the UART, then echoes your keys
```

### What's an "address"?
RAM is like a giant row of numbered boxes, each holding 1 byte. The number of
a box is its **address**. `0x80200000` is just a number written in
hexadecimal (base 16) — it's box number 2,149,580,800. QEMU's RAM starts at
`0x80000000`; OpenSBI uses the first part, and we live right after it.

---

## 4. The files — what each one does

### `kernel/entry.S` — the first 2 instructions
Written in **assembly** (raw CPU instructions), because C can't run yet.

Why can't C run? C functions need a **stack** — a scratch area of memory for
local variables and for remembering "where do I return to after this
function?". At power-on, the stack pointer (`sp`) points at garbage. So:

```asm
la   sp, stack_top   # point sp at a 16 KB area we reserved
call kmain           # now C works — jump into it!
```

### `kernel/kernel.ld` — the linker script
The compiler turns each file into machine code. The **linker** glues them
into one file (`build/kernel.elf`) and decides **where in memory** each part
goes. We tell it: "start at `0x80200000`, and put `_start` first", because
that's exactly where OpenSBI will jump.

It also groups things into **sections**:
- `.text` — the code
- `.rodata` — read-only data (like the text `"Hello"`)
- `.data` — global variables with starting values
- `.bss` — global variables that start at zero

### `kernel/uart.c` — our first device driver
The **UART** is a serial port: the simplest way to send text in and out. In
QEMU it's connected to your terminal.

The big idea here is **memory-mapped I/O**: devices show up at special
addresses. QEMU puts the UART at `0x10000000`. So:

```c
*(volatile unsigned char *)0x10000000 = 'A';   // prints "A"!
```

Writing to that address doesn't store anything in RAM — the hardware
grabs the byte and sends it out. That one idea is how *every* driver works.

`volatile` tells the compiler "don't optimize this away — it's hardware, every
read and write matters."

Before sending, we check the **Line Status Register** (offset 5): bit 5 says
"ready for another byte", bit 0 says "a key was pressed". Repeatedly checking
a status bit like this is called **polling**.

### `kernel/main.c` — `kmain()`
1. **Clears `.bss`** — C promises globals start at 0. Normally the OS does
   that for programs… but we *are* the OS.
2. Prints the welcome message.
3. Loops forever: if a key was pressed, print it back.

Notice: **no `#include <stdio.h>`, no `printf`**. There is no standard
library — that library depends on an OS, and there isn't one yet. Everything
we want, we build.

### `Makefile` — build commands
| Command | What it does |
|---|---|
| `make` | compile everything into `build/kernel.elf` |
| `make run` | boot it in QEMU. **Quit with `Ctrl-A`, then `X`** |
| `make debug` + `make gdb` | pause the kernel and step through it line by line |
| `make clean` | delete the build folder |

---

## 5. Try it yourself 🧪

Open a terminal in this folder and run:

```bash
make run
```

Then try these small experiments (edit, then `make run` again):

1. Change the welcome message in `main.c` to your name.
2. Make every key you type print in **UPPERCASE**.
   *Hint:* lowercase letters are `'a'`–`'z'`; `c - 32` turns `'a'` into `'A'`.
3. Make Enter print `"you pressed enter!"`.
4. **Break it on purpose:** delete the `la sp, stack_top` line in `entry.S`.
   What happens? (Then put it back.) This shows why the stack matters.

---

## 6. Words you learned

| Word | Meaning |
|---|---|
| **Kernel** | The core of the OS; full control over the hardware |
| **Cross-compiler** | Compiler that builds code for a different CPU than the one you're on |
| **Emulator (QEMU)** | Software pretending to be a computer |
| **Firmware (OpenSBI)** | Small program that runs before the kernel and starts it |
| **Address** | The number of a byte in memory |
| **Stack** | Memory scratch space C functions need |
| **Linker script** | Tells the linker where in memory each part goes |
| **MMIO** | Talking to devices by reading/writing special addresses |
| **UART** | Serial port; our text in/out |
| **Polling** | Repeatedly checking a device's status |

## 7. Interview check ✅
You should now be able to answer:
1. *What happens when a computer boots, before the kernel's `main` runs?*
2. *Why does a kernel need some assembly code before C can run?*
3. *What is memory-mapped I/O, and why is `volatile` needed?*

---

**Next up — Lesson 1:** a proper `printf` for the kernel, and making the
kernel *catch its own crashes* (traps & exceptions) instead of freezing.
