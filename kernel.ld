/* kernel.ld — tells the build tool WHERE in memory to put our code.
 *
 * The firmware jumps to memory box 0x80200000. So our first instruction
 * must be exactly there, with nothing else in front of it.
 */
ENTRY(_start)                  /* the first instruction is called _start */

SECTIONS
{
    . = 0x80200000;            /* start placing things at box 0x80200000 */
    .text   : { *(.text) }     /* 1st: our instructions */
    .rodata : { *(.rodata) }   /* 2nd: read-only data, like our message */
}
