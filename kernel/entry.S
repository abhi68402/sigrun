# entry.S — the very first instructions of our kernel.
#
# When QEMU starts, a small piece of firmware called OpenSBI runs first
# (think of it as the "BIOS"). When it's done, it jumps to address
# 0x80200000, which is where our linker script puts this code.
#
# At this point there is NO stack, so we can't call C functions yet.
# This file's only job: set up a stack, then jump into C (kmain).

.section .text.boot        # special section so the linker puts this FIRST
.global _start
_start:
    la   sp, stack_top     # sp = stack pointer register. Point it at the top of our stack.
                           # (Stacks grow DOWN, so we start at the top.)
    call kmain             # jump into C code in main.c

    # If kmain ever returns (it shouldn't), sleep forever.
hang:
    wfi                    # "wait for interrupt": pauses the CPU to save power
    j    hang

# Reserve 16 KB of memory for the stack.
.section .bss
.align 16
stack_bottom:
    .space 4096 * 4
stack_top:
