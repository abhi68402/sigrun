# trap.S — the "emergency entrance" of the kernel.
#
# When something goes wrong (or special) the CPU stops what it's doing and
# jumps to the address stored in the `stvec` register. That's this code.
#
# Problem: the code that was interrupted was using ALL the registers. If
# our C handler overwrites them, the interrupted code would be ruined.
# Solution: save every register to memory, call the C handler, then put
# every register back exactly as it was, and return with `sret`.
#
# The saved registers form a "trap frame" (see struct trap_frame in trap.h).
# Register xN is saved at offset N*8 bytes.

.section .text
.global trap_vector
.align 4                     # stvec requires a 4-byte-aligned address
trap_vector:
    addi sp, sp, -256        # make room for 32 registers x 8 bytes

    sd x1,   1*8(sp)         # sd = "store doubleword" (8 bytes) to memory
    # x2 is sp itself; we store its ORIGINAL value below
    sd x3,   3*8(sp)
    sd x4,   4*8(sp)
    sd x5,   5*8(sp)
    sd x6,   6*8(sp)
    sd x7,   7*8(sp)
    sd x8,   8*8(sp)
    sd x9,   9*8(sp)
    sd x10, 10*8(sp)
    sd x11, 11*8(sp)
    sd x12, 12*8(sp)
    sd x13, 13*8(sp)
    sd x14, 14*8(sp)
    sd x15, 15*8(sp)
    sd x16, 16*8(sp)
    sd x17, 17*8(sp)
    sd x18, 18*8(sp)
    sd x19, 19*8(sp)
    sd x20, 20*8(sp)
    sd x21, 21*8(sp)
    sd x22, 22*8(sp)
    sd x23, 23*8(sp)
    sd x24, 24*8(sp)
    sd x25, 25*8(sp)
    sd x26, 26*8(sp)
    sd x27, 27*8(sp)
    sd x28, 28*8(sp)
    sd x29, 29*8(sp)
    sd x30, 30*8(sp)
    sd x31, 31*8(sp)
    addi t0, sp, 256         # original sp (before we subtracted 256)
    sd t0,   2*8(sp)

    mv a0, sp                # first C argument = pointer to the trap frame
    call kernel_trap         # handle it in C (trap.c)

    ld x1,   1*8(sp)         # ld = "load doubleword": restore everything
    ld x3,   3*8(sp)
    ld x4,   4*8(sp)
    ld x5,   5*8(sp)
    ld x6,   6*8(sp)
    ld x7,   7*8(sp)
    ld x8,   8*8(sp)
    ld x9,   9*8(sp)
    ld x10, 10*8(sp)
    ld x11, 11*8(sp)
    ld x12, 12*8(sp)
    ld x13, 13*8(sp)
    ld x14, 14*8(sp)
    ld x15, 15*8(sp)
    ld x16, 16*8(sp)
    ld x17, 17*8(sp)
    ld x18, 18*8(sp)
    ld x19, 19*8(sp)
    ld x20, 20*8(sp)
    ld x21, 21*8(sp)
    ld x22, 22*8(sp)
    ld x23, 23*8(sp)
    ld x24, 24*8(sp)
    ld x25, 25*8(sp)
    ld x26, 26*8(sp)
    ld x27, 27*8(sp)
    ld x28, 28*8(sp)
    ld x29, 29*8(sp)
    ld x30, 30*8(sp)
    ld x31, 31*8(sp)
    addi sp, sp, 256         # give the stack space back

    sret                     # "supervisor return": jump back to sepc
