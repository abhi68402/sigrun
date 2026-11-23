# Start Here — Operating Systems From Zero

> Read this first. No experience needed. Every idea gets four things:
> **what it is**, **an everyday analogy**, **how Sigrun uses it**, and
> **what we actually build**.

The whole guide uses one analogy: **a computer is a restaurant.**

---

## Part 1 — What a computer actually is

### 1. The CPU (processor)
- **What:** the chip that does the work. It can only do tiny things: add two
  numbers, copy a number, compare, jump to another step.
- **Analogy:** the **chef**. Fast, but only follows recipe steps one at a time.
- **Key fact:** a CPU does *billions* of tiny steps per second. Everything you
  see a computer do is built from those tiny steps.

### 2. Instructions (machine code)
- **What:** the tiny steps the CPU understands, stored as numbers.
  `add a0, a0, a1` = "add these two numbers".
- **Analogy:** **one line of a recipe**: "chop onion", "stir 2 minutes".
- **Assembly** is the human-readable spelling of instructions.
  **C** is a friendlier language that a **compiler** translates into instructions.

### 3. Registers
- **What:** about 32 tiny storage slots *inside* the CPU, each holding one
  number. Super fast, very few.
- **Analogy:** the **chef's hands and cutting board**: what they're working on
  *right now*.
- Special ones: `sp` (stack pointer — where the scratch area is),
  `pc` (program counter — which instruction is next).

### 4. Memory (RAM) and addresses
- **What:** a long row of numbered boxes, each holding one byte. The box
  number is its **address** (`0x80200000`). Programs and their data live here.
- **Analogy:** the **pantry shelves**, every shelf numbered.
- Lost when power goes off.

### 5. Devices and MMIO
- **What:** keyboard, screen, disk, network. The CPU talks to them by reading
  and writing **special addresses** (memory-mapped I/O).
- **Analogy:** **the order window and delivery hatch**: put something in a
  specific slot and the outside world gets it.
- ✅ *Already built:* our UART driver writes to `0x10000000` to print text.

---

## Part 2 — What an operating system is

### 6. Operating system / kernel
- **What:** the program that controls the hardware and **shares it fairly and
  safely** between all other programs. The **kernel** is its core.
- **Analogy:** the **restaurant manager**. Chefs (CPU), shelves (memory) and
  the order window (devices) are shared by many customers (programs).
  The manager decides who gets what, when, and stops anyone from stealing.
- **Sigrun:** *is* a kernel, written from zero.

### 7. Linux, distributions, Windows — where Sigrun fits
```
  Apps (Chrome, games)
  Look & feel (desktop, icons)        ← a Linux "distribution" customizes this
  ───────────────────────────────
  KERNEL (Linux / Windows NT / Sigrun) ← we build this layer
  ───────────────────────────────
  Hardware
```
Most "new OSes" are Linux with a new look. Sigrun is a **new kernel**, the
hardest and rarest layer.

### 8. Privilege modes (kernel mode vs user mode)
- **What:** the CPU has power levels. In **kernel (supervisor) mode** code can
  do anything. In **user mode** dangerous instructions are blocked.
- **Analogy:** **staff badge vs customer**. Customers can't walk into the
  kitchen or open the safe.
- **Sigrun:** today everything runs in kernel mode. In M6 we put programs in
  user mode so they *must* go through Sigrun.

---

## Part 3 — The concepts we'll build, in order

### 9. Traps ✅ (Lesson 1)
- **What:** the CPU suddenly stops what it's doing and jumps into the kernel.
  *Exceptions* come from the current instruction (bad address); *interrupts*
  come from outside (timer, keyboard).
- **Analogy:** a **fire alarm**: everyone stops, the manager handles it, then
  work resumes where it stopped.
- **Sigrun:** ⭐ **this is the heart of our big idea.** A consent fault *is* a
  trap. We already freeze, report and resume on `break`.

### 10. Timer interrupts (M2)
- **What:** a hardware clock that taps the CPU on the shoulder, say, 100 times
  a second, forcing a trap into the kernel.
- **Analogy:** a **kitchen timer** that rings every few minutes so the manager
  can check on things, even if a chef never stops.
- **Why needed:** without it, a program stuck in a loop would hold the CPU
  forever. With it, the kernel always gets control back.
- **We build:** a heartbeat counter and an `uptime` command.

### 11. Memory allocator (M3)
- **What:** code that hands out free chunks of RAM and takes them back.
- **Analogy:** the **pantry keeper** with a list of empty shelves.
- **We build:** `kalloc()` / `kfree()` for 4 KB pages, and a `mem` command
  showing free memory.

### 12. Paging / virtual memory (M4)
- **What:** each program gets its **own fake address map**. Address
  `0x1000` in program A and in program B point to different real memory.
  A **page table** translates fake → real. Touching an address that isn't in
  the map causes a **page fault** (a trap).
- **Analogy:** every customer gets a **personal menu with their own table
  numbers**. "Table 3" means a different real table for each customer, so
  nobody can reach someone else's food.
- **Sigrun:** ⭐ **first consent fault.** A program touches memory it wasn't
  given → page fault → Sigrun freezes it and asks you → maps it → resumes.

### 13. Processes and the scheduler (M5)
- **What:** a **process** is a running program (its code, memory and saved
  registers). The **scheduler** switches the CPU between processes so fast
  they all seem to run at once. A **context switch** = save one process's
  registers, load another's (the same save/restore trick as traps).
- **Analogy:** one chef cooking **five orders**, switching every few seconds
  and keeping notes on where each one was.
- **Sigrun:** a frozen (consent-faulted) process simply isn't scheduled
  until you answer; everything else keeps running.
- **We build:** several programs running "at the same time", and a `ps` command.

### 14. System calls (M6)
- **What:** the official way a user-mode program asks the kernel for help
  ("open this file", "print this"). It uses a special instruction (`ecall`)
  that causes a **trap** into the kernel.
- **Analogy:** a customer can't enter the kitchen; they **ring the bell and
  ask the waiter**.
- **Sigrun:** ⭐ **consent faults for real programs.** Every request is
  checked against the program's keys. No key → frozen → you decide.

### 15. Filesystem and shell (M7)
- **What:** a **filesystem** organizes the disk into files and folders.
  A **shell** is the text program where you type commands.
- **Analogy:** the **recipe book archive** and the **front desk**.
- **We build:** files you can create/read/delete, and a real shell running
  as a user program.

---

## Part 4 — Sigrun's own ideas

### 16. Keys (capabilities) (M8)
- **What:** instead of "this user can do everything", each program holds a
  list of **keys**, each opening exactly one thing ("read `photos`").
- **Analogy:** a **hotel key card** that opens only your room and the gym,
  and expires at checkout.
- **We build:** allow once / for this task / deny, and taking a key back anytime.

### 17. Consent faults ⭐ (our big idea)
- **What:** programs start with **zero keys**. The moment one touches
  something new, the CPU traps, Sigrun **freezes it mid-instruction**, asks
  you, and resumes the exact instruction if you allow it.
- **Analogy:** a **guard who stops a guest at every new door**, calls you,
  and only then opens it. The guest can't sneak past, because the doors
  themselves are locked.
- **Why it's new:** existing AI-agent approval systems rely on the agent's
  own code choosing to ask. In Sigrun the **hardware forces the question**,
  so it works on any program.

### 18. Least-privilege report (M9)
- **What:** after a run, Sigrun lists exactly which keys a program needed,
  and can lock it to just those next time.
- **Analogy:** after a week, the hotel knows the guest only ever used their
  room and the gym, so it issues a card for only those.

### 19. Agent bridge + browser demo (M10–M11)
- **What:** an AI running outside (on your Mac or online) controls programs
  inside Sigrun, and gets frozen whenever it touches something new. Then we
  make Sigrun bootable from a web link.

---

## How it all connects

```
 Lesson 1  traps ───────────────┐  (freeze → handle → resume)
 M2  timer  (kernel always gets control back)
 M3  allocator → M4 paging ─────┤  page fault  = consent fault #1
 M5  processes (freeze one, run the rest)
 M6  user mode + syscalls ──────┤  ecall trap  = consent fault #2
 M7  files + shell              │
 M8  keys ──────────────────────┘→ ⭐ CONSENT FAULTS
 M9  least-privilege report
 M10 AI agent bridge → M11 browser demo
```

**Every foundation lesson is a building block of the big idea.** Nothing we
build is "just theory".
