# Sigrun OS — Progress

Stack: **C + RISC-V 64 + QEMU (virt machine, OpenSBI firmware)**

## The big idea: consent faults
> **Permission works like memory.** Nothing is granted up front. The first
> time any program or AI agent touches something new, the CPU traps, Sigrun
> freezes it mid-instruction, asks you, and — if you allow it — resumes the
> exact same instruction as if nothing happened.

Why it's different: existing "ask before acting" systems for AI agents work at
the app level, so the agent's own code has to choose to ask. In Sigrun the
**hardware forces the question**, so it works on any program, unmodified.
(Researched Nov 2026: no OS found that pauses programs at the CPU trap level
for consent and resumes them.)

## Roadmap
### Phase 1 — Foundation (every OS needs this)
- [x] **M0** Boot + print over UART + echo keys → `lessons/00-hello-kernel.md`
- [x] **M1** Kernel `printf`, panic, traps/exceptions → `lessons/01-printf-and-traps.md`
      *(the `break` command already freezes, reports and resumes: the seed of a consent fault)*
- [ ] **M2** Timer interrupts (the kernel's heartbeat)
- [ ] **M3** Physical memory allocator
- [ ] **M4** Paging (virtual memory) + **first consent fault**: touch an unmapped page → ask → map → resume
- [ ] **M5** Processes + scheduler (context switching)
- [ ] **M6** User mode + system calls + **consent faults for real programs**: a program that asks for something it has no key for is frozen until you answer
- [ ] **M7** Filesystem + shell  ← "impressive" checkpoint

### Phase 2 — Sigrun's own features
- [ ] **M8** Keys (capabilities): allow once / for this task / deny; revocable at any time
- [ ] **M9** Least-privilege report: "this program needed exactly these keys", and re-run with only those
- [ ] **M10** Agent bridge: an outside AI drives programs inside Sigrun, and gets frozen whenever it touches something new
- [ ] **M11** Showcase: boot it in the browser, CI, polished README, write-up

## What makes Sigrun different
1. **Consent faults:** the CPU freezes a program at the exact moment it touches something new, and Sigrun asks you. No cooperation from the program needed.
2. **Starts with zero access:** every program and agent begins with no keys at all; there is no all-powerful root user.
3. **Least privilege for free:** Sigrun records exactly which keys a program needed and can lock it to that list.
4. **Built to be readable:** every milestone has a beginner lesson in `lessons/`.
