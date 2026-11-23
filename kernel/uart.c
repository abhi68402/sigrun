// uart.c — a tiny driver for the 16550 UART chip that QEMU emulates.
//
// KEY IDEA: "memory-mapped I/O" (MMIO).
// Hardware devices appear at special memory addresses. On QEMU's "virt"
// machine, the UART lives at address 0x10000000. Writing a byte to that
// address doesn't store it in RAM — it sends it out the serial port!

#include "uart.h"

#define UART_BASE 0x10000000UL

// The UART has several 1-byte "registers" at UART_BASE + offset:
#define THR 0   // Transmit Holding Register: write a byte here to send it
#define RBR 0   // Receive Buffer Register:   read a byte here to receive it
#define LSR 5   // Line Status Register:      tells us if the UART is ready

#define LSR_DATA_READY (1 << 0)   // bit 0: a key is waiting to be read
#define LSR_TX_EMPTY   (1 << 5)   // bit 5: ready to send another byte

// `volatile` tells the compiler: "this memory can change by itself (it's
// hardware), never skip or reorder reads/writes to it."
static inline volatile unsigned char *reg(int offset) {
    return (volatile unsigned char *)(UART_BASE + offset);
}

void uart_putc(char c) {
    while ((*reg(LSR) & LSR_TX_EMPTY) == 0) {
        // wait until the UART is ready ("busy waiting" / "polling")
    }
    *reg(THR) = c;
}

void uart_puts(const char *s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');  // terminals want \r\n for a new line
        uart_putc(*s++);
    }
}

void uart_puthex(unsigned long x) {
    const char *digits = "0123456789abcdef";
    uart_puts("0x");
    for (int shift = 60; shift >= 0; shift -= 4) {
        uart_putc(digits[(x >> shift) & 0xf]);  // take 4 bits at a time
    }
}

int uart_getc(void) {
    if (*reg(LSR) & LSR_DATA_READY) {
        return *reg(RBR);
    }
    return -1;  // no key pressed
}
