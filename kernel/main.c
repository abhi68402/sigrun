// main.c — the kernel's main function. entry.S jumps here.
//
// Note: there is NO standard library here. No printf, no malloc, no files.
// We're the OS, so anything we want, we build ourselves.

#include "uart.h"
#include "printf.h"
#include "string.h"
#include "trap.h"

// These come from kernel.ld (the linker script).
extern char bss_start[], bss_end[], kernel_end[];

static void clear_bss(void) {
    // C promises that global variables start at 0. Normally the OS
    // does that for a program... but we ARE the OS, so we do it ourselves.
    for (char *p = bss_start; p < bss_end; p++) {
        *p = 0;
    }
}

// ---------------------------------------------------------------------
// A tiny command console. (Not a real shell yet — that's Milestone 7.)
// ---------------------------------------------------------------------

static void cmd_help(void) {
    kprintf("Commands:\n");
    kprintf("  help     - show this list\n");
    kprintf("  info     - where the kernel lives in memory\n");
    kprintf("  break    - hit a breakpoint (the kernel recovers!)\n");
    kprintf("  crash    - read from a bad address (crash report)\n");
    kprintf("  illegal  - run a garbage instruction (crash report)\n");
}

static void cmd_info(void) {
    kprintf("Kernel starts at %p, ends at %p (%d KB)\n",
            (void *)0x80200000UL, (void *)kernel_end,
            (int)(((unsigned long)kernel_end - 0x80200000UL) / 1024));
}

static void run_command(const char *line) {
    if (strcmp(line, "") == 0) {
        // empty line: do nothing
    } else if (strcmp(line, "help") == 0) {
        cmd_help();
    } else if (strcmp(line, "info") == 0) {
        cmd_info();
    } else if (strcmp(line, "break") == 0) {
        asm volatile("ebreak");          // the breakpoint instruction
        kprintf("...and we're back after the breakpoint.\n");
    } else if (strcmp(line, "crash") == 0) {
        volatile unsigned long *bad = (unsigned long *)0xdead0000UL;
        kprintf("Reading from %p, where no memory exists...\n", (void *)bad);
        kprintf("value = %lx\n", *bad);  // traps before this prints
    } else if (strcmp(line, "illegal") == 0) {
        kprintf("Running instruction 0x00000000 (not a valid instruction)...\n");
        asm volatile(".word 0");         // raw bytes that aren't an instruction
    } else {
        kprintf("unknown command '%s' (try 'help')\n", line);
    }
}

void kmain(void) {
    clear_bss();
    trap_init();   // from now on, crashes give a report instead of freezing

    kprintf("\n");
    kprintf("=================================\n");
    kprintf("  Sigrun OS -- hello from my own kernel!\n");
    kprintf("=================================\n");
    kprintf("Type 'help' for commands. Quit QEMU: Ctrl-A then X\n\n");

    char line[64];
    int len = 0;
    kprintf("> ");

    for (;;) {
        int c = uart_getc();
        if (c == -1) continue;                       // no key yet

        if (c == '\r' || c == '\n') {                // Enter: run the line
            kprintf("\n");
            line[len] = '\0';                        // end the string
            run_command(line);
            len = 0;
            kprintf("> ");
        } else if (c == 127 || c == 8) {             // Backspace
            if (len > 0) {
                len--;
                kprintf("\b \b");
            }
        } else if (len < (int)sizeof(line) - 1) {    // normal key: store + echo
            line[len++] = (char)c;
            uart_putc((char)c);
        }
    }
}
