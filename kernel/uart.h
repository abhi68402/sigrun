// uart.h — talk to the serial port (the "UART").
// The UART is how our kernel prints text and reads keys.
#pragma once

void uart_putc(char c);           // print one character
void uart_puts(const char *s);    // print a string
void uart_puthex(unsigned long x);// print a number in hex, e.g. 0x80200000
int  uart_getc(void);             // read one key, or -1 if none was pressed
