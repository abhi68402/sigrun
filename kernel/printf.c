// printf.c — the kernel's own printf.
//
// How does printf take "any number" of arguments? C has a feature called
// "variadic functions" (the `...`). <stdarg.h> is one of the few headers
// that comes from the COMPILER, not the OS, so we're allowed to use it.

#include <stdarg.h>
#include "printf.h"
#include "uart.h"
#include "types.h"

// Print an unsigned number in any base (10 = decimal, 16 = hex).
static void print_unsigned(uint64 n, int base) {
    const char *digits = "0123456789abcdef";
    char buf[32];
    int i = 0;

    // Pull off digits from the right: 1234 -> '4','3','2','1'
    do {
        buf[i++] = digits[n % base];
        n /= base;
    } while (n > 0);

    // ...then print them in reverse so they come out the right way round.
    while (i > 0) uart_putc(buf[--i]);
}

static void print_signed(int64 n) {
    if (n < 0) {
        uart_putc('-');
        n = -n;
    }
    print_unsigned((uint64)n, 10);
}

// The real worker. Walks through the format string character by character.
static void vkprintf(const char *fmt, va_list args) {
    for (; *fmt; fmt++) {
        if (*fmt != '%') {                     // ordinary character
            if (*fmt == '\n') uart_putc('\r');
            uart_putc(*fmt);
            continue;
        }

        fmt++;                                 // skip the '%'
        int is_long = 0;
        if (*fmt == 'l') {                     // "%ld" or "%lx"
            is_long = 1;
            fmt++;
        }

        switch (*fmt) {
        case 'd':
            print_signed(is_long ? va_arg(args, int64) : va_arg(args, int));
            break;
        case 'u':
            print_unsigned(is_long ? va_arg(args, uint64) : va_arg(args, uint32), 10);
            break;
        case 'x':
            print_unsigned(is_long ? va_arg(args, uint64) : va_arg(args, uint32), 16);
            break;
        case 'p':
            uart_puts("0x");
            print_unsigned((uint64)va_arg(args, void *), 16);
            break;
        case 's': {
            const char *s = va_arg(args, const char *);
            uart_puts(s ? s : "(null)");
            break;
        }
        case 'c':
            uart_putc((char)va_arg(args, int));
            break;
        case '%':
            uart_putc('%');
            break;
        case '\0':                             // string ended right after '%'
            return;
        default:                               // unknown: print it as-is
            uart_putc('%');
            uart_putc(*fmt);
        }
    }
}

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vkprintf(fmt, args);
    va_end(args);
}

void panic(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    kprintf("\n!!! KERNEL PANIC: ");
    vkprintf(fmt, args);
    kprintf("\nSystem halted. (Quit QEMU: Ctrl-A then X)\n");
    va_end(args);

    for (;;) {
        asm volatile("wfi");                   // sleep forever
    }
}
