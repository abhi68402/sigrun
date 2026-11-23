# Sigrun OS — Progress

Stack: **C + RISC-V 64 + QEMU (virt machine, OpenSBI firmware)**
Pitch (end goal): *A capability-secure OS that runs sandboxed WebAssembly apps — bootable in your browser.*

## Roadmap
- [x] **M0** Boot + print over UART + echo keys → `lessons/00-hello-kernel.md`
- [x] **M1** Kernel `printf`, panic, traps/exceptions → `lessons/01-printf-and-traps.md`
- [ ] **M2** Timer interrupts
- [ ] **M3** Physical memory allocator
- [ ] **M4** Paging (virtual memory)
- [ ] **M5** Processes + scheduler (context switching)
- [ ] **M6** User mode + system calls
- [ ] **M7** Filesystem + shell  ← "impressive" checkpoint
- [ ] **M8** Standout: WebAssembly userspace + capabilities
- [ ] **M9** Standout: boot it in the browser, CI, polished README

## What makes this OS different
1. **Apps are WebAssembly**, not native programs: each app runs sandboxed, and the same app file runs on any CPU.
2. **Capability security:** an app can only touch what it was explicitly handed (no "root user" that can do everything).
3. **Boots in a browser tab:** anyone can try it from a link, with nothing to install.
4. **Built to be readable:** every milestone has a beginner lesson in `lessons/`.
