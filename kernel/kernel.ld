/* kernel.ld — the linker script.
 *
 * The compiler turns each .c/.S file into machine code. The LINKER glues
 * those pieces into one file and decides WHERE in memory each piece lives.
 * Normal programs don't need this (the OS decides for them), but we ARE
 * the OS, so we must tell the linker exactly where to put things.
 */

OUTPUT_ARCH(riscv)
ENTRY(_start)                 /* first instruction to run = _start in entry.S */

SECTIONS
{
    . = 0x80200000;           /* OpenSBI jumps here, so our code must start here */

    .text : {                 /* .text = machine code */
        *(.text.boot)         /* entry.S first, so _start is exactly at 0x80200000 */
        *(.text .text.*)
    }

    .rodata : {               /* read-only data, e.g. string constants "hello" */
        *(.rodata .rodata.*)
    }

    .data : {                 /* global variables that start with a value */
        *(.data .data.*)
    }

    .bss : {                  /* global variables that start as zero */
        bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        bss_end = .;
    }

    kernel_end = .;           /* marks where our kernel ends in memory */
}
