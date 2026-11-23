# Lesson 2 — The Heartbeat (Timer Interrupts)

> Read one part at a time. Each part is one small idea.

---

## Part 1 — The problem

The CPU can only do **one thing at a time**.

When some code is running, the kernel is **not** running.
The kernel only gets a turn when the running code gives the CPU back.

What if the code **never** gives it back? For example, a loop that never ends:

```c
while (1) { }     // runs forever
```

Then the kernel never gets a turn again. It can't stop that code. It can't
run anything else. The computer is stuck.

In Sigrun this matters a lot: to **freeze** a program and ask you something,
the kernel first needs a turn.

---

## Part 2 — The idea: an alarm clock

Fix: put an **alarm clock** in the hardware.

- The kernel sets the alarm: "ring in 10 milliseconds".
- When it rings, the CPU **stops** whatever it is doing, even an endless loop.
- The CPU jumps into the kernel. *(This is a trap, from Lesson 1.)*
- The kernel does its work, sets the next alarm, and lets the code continue.

Each ring is called a **tick**. Sigrun's alarm rings **100 times per second**.
Together, the ticks are the kernel's **heartbeat**.

A trap caused by the outside (like an alarm) is called an **interrupt**.

---

## Part 3 — The clock and the alarm

The CPU has a **clock**: a number that goes up by itself, all the time.
On QEMU it goes up **10,000,000 times per second**.

```c
uint64 now = read_time();      // read the clock
```

The alarm is: "interrupt me when the clock reaches this number".
For one tick from now:

```
now + 10,000,000 / 100   =   now + 100,000
```

**File:** `kernel/timer.c`
```c
static void set_next_alarm(void) {
    sbi_set_timer(read_time() + TICK_INTERVAL);
}
```

---

## Part 4 — Asking the firmware

Setting the alarm is a very powerful setting. Even the kernel is not allowed
to touch it directly. Only the **firmware (OpenSBI)** can.

So the kernel **asks** the firmware, with the `ecall` instruction:

```
program  ──ask──▶  kernel  ──ask──▶  firmware
```

**File:** `kernel/sbi.c`. The service's ID number is `0x54494D45`. Those
bytes are the ASCII letters **T I M E**. Everything is numbers!

---

## Part 5 — Turning interrupts on

Three steps, in `timer_init()`:

```c
set_next_alarm();               // 1. set the first alarm
csr_set(sie, SIE_STIE);         // 2. allow timer interrupts
csr_set(sstatus, SSTATUS_SIE);  // 3. master switch: interrupts ON
```

Two switches, like a light that needs both the room switch **and** the main
power switch to be on.

---

## Part 6 — What happens on each tick

When the alarm rings, the CPU jumps to our trap handler (Lesson 1).
`trap.c` checks the reason. Reason **5** means "timer":

```c
if (code == IRQ_TIMER) {
    timer_tick();     // count it, set the next alarm
    return;           // let the interrupted code continue
}
```

```c
void timer_tick(void) {
    ticks++;            // count the beat
    set_next_alarm();   // set the alarm for the next one
}
```

The interrupted code never notices. It just continues where it stopped.
*That "continue where it stopped" is exactly what a consent fault will need.*

---

## Part 7 — Try it

```bash
make run
```

| Command | What you see |
|---|---|
| `uptime` | how long Sigrun has been running, and how many heartbeats |
| `stuck` | a loop that never gives the CPU back, and how many times the timer still interrupted it |

Real output:
```
> stuck
Running a busy loop that never gives the CPU back...
Done. While it ran, the timer interrupted it 70 times,
and each time the kernel got control back.
```

---

## Words you learned
| Word | Meaning |
|---|---|
| **Interrupt** | a trap caused by something outside, like the alarm |
| **Tick** | one ring of the alarm |
| **Heartbeat** | the ticks together; the kernel's regular turn |
| **Firmware call (SBI call)** | the kernel asking the firmware for help, using `ecall` |

## Why this matters for Sigrun
- Later, many programs will share the CPU. The heartbeat lets the kernel
  **switch** between them (Milestone 5).
- A frozen program can stay frozen while others keep running, because the
  kernel always gets its turn.
