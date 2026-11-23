# Lesson 1 — A Kernel `printf`, and Catching Crashes

> Goal: the kernel can print anything, and when something goes wrong it
> **tells us what happened** instead of silently freezing.

---

## Part A — Building our own `printf`

In Lesson 0 we could only print plain text. Real debugging needs numbers:
"what address crashed?", "how big is this?". Normally you'd use `printf`, but
`printf` lives in the C standard library, and that library needs an OS
underneath. So we wrote **`kprintf`** (k = kernel) in `kernel/printf.c`.

### How does it work?
It walks through the text one character at a time:

```
kprintf("Size: %d KB", 80);
         │││││││ └── '%' found! look at the next letter: 'd' = number
         └┴┴┴┴┴── normal characters: print them as-is
```

### How do you print a number?
The UART can only send **characters**, not numbers. So `80` must become the
characters `'8'` and `'0'`. The trick:

```
80 % 10 = 0   → '0'   (% = remainder after dividing)
80 / 10 = 8
 8 % 10 = 8   → '8'
 8 / 10 = 0   → stop
```
That gives the digits **backwards** (`'0','8'`), so we store them and print
them in reverse. For hex we do the same with 16 instead of 10.

### `panic()` — stopping on purpose
When the kernel finds something impossible, carrying on could corrupt
everything. `panic()` prints a message and stops the CPU forever. Linux has
the exact same thing — the famous "kernel panic".

### `string.c` — no libc means DIY
We also wrote `strlen`, `strcmp` (compare two strings) and `memset` (fill
memory). Fun fact: the compiler sometimes secretly calls `memset` to zero
arrays, so a kernel *must* provide it, even if you never call it yourself.

---

## Part B — Traps: the CPU's emergency button

### What is a trap?
Normally the CPU runs instructions one after another. A **trap** is when the
CPU suddenly **stops and jumps into the kernel**. Two kinds:

| Kind | Cause | Example |
|---|---|---|
| **Exception** | the current instruction itself | reading a bad address, a garbage instruction, a breakpoint |
| **Interrupt** | something outside | the timer ticking, a key pressed (Lesson 2) |

Analogy: you're reading a book (running code). An **exception** is hitting a
page with a printing error. An **interrupt** is your phone ringing.

### What happened before this lesson?
If our kernel read a bad address, the CPU jumped to... wherever `stvec`
pointed — nothing set up — and the kernel froze with no explanation.

### The 4 special registers (CSRs)
| Register | Meaning |
|---|---|
| `stvec` | **where** to jump when a trap happens (we set it in `trap_init`) |
| `scause` | **why** it happened (a number: 2 = illegal instruction, 5 = bad read...) |
| `sepc` | **which instruction** was running when it happened |
| `stval` | **extra detail**, e.g. the bad address |

### The journey of a trap
```
 your code runs ... *bad   ← reading 0xdead0000 (no memory there!)
        │
        ▼   CPU: scause=5, sepc=<this instruction>, stval=0xdead0000
 trap.S   — saves ALL 31 registers to the stack (the "trap frame")
        │
        ▼
 trap.c   kernel_trap() — reads scause and decides:
        │     breakpoint?  → print, skip the instruction, carry on
        │     anything else → print a CRASH REPORT, panic()
        ▼
 trap.S   — restores all registers, `sret` = jump back to sepc
```

### Why save every register?
The interrupted code was in the middle of using the registers (like a
person mid-sentence). Our C handler will overwrite them. If we didn't save
and restore them, the interrupted code would continue with scrambled data.
This **save → handle → restore** pattern is the heart of every OS — it's
also exactly how we'll switch between programs later (Milestone 5).

### Why can the breakpoint continue but a crash can't?
For a breakpoint, we move `sepc` forward past the `ebreak` instruction, so
`sret` returns to the *next* instruction. Debuggers like GDB use this exact
trick. For a bad memory read, there's no sensible value to pretend we read,
so the safest thing is to stop.

---

## Try it yourself 🧪

```bash
make run
```

Then type these commands:

| Command | What you'll see |
|---|---|
| `help` | the list of commands |
| `info` | where the kernel lives in memory, and its size |
| `break` | a breakpoint trap that the kernel **recovers** from |
| `crash` | a full **crash report** for reading a bad address |
| `illegal` | a crash report for a garbage instruction |

After `crash` or `illegal` the kernel is halted — quit with **Ctrl-A, X** and
run again.

### Experiments
1. Add a command `whoami` that prints your name using `kprintf("%s", ...)`.
2. Add a command `count` that prints the numbers 1 to 10 with `%d`.
3. Make a crash by **writing** instead of reading:
   `*(volatile unsigned long *)0xdead0000UL = 42;`
   What exception number (`scause`) do you get? Look it up in `trap.c`.

---

## Words you learned
| Word | Meaning |
|---|---|
| **Variadic function** | a function that takes any number of arguments (`...`) |
| **Panic** | the kernel stopping on purpose because continuing is unsafe |
| **Trap** | the CPU jumping into the kernel because of an event |
| **Exception** | a trap caused by the running instruction |
| **Interrupt** | a trap caused by outside hardware |
| **CSR** | a special CPU register that controls the CPU itself |
| **Trap frame** | the saved copy of all registers during a trap |

## Interview check ✅
1. *What's the difference between an exception and an interrupt?*
2. *What does a trap handler need to save, and why?*
3. *How does a debugger breakpoint work at the CPU level?*

---

**Next up — Lesson 2:** timer interrupts. The kernel gets a "heartbeat" that
ticks many times per second, even when no one presses a key. This is what
later lets the OS switch between programs.
