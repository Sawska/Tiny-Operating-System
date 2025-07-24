#include "uart.h"

// UART0 base and register offsets
#define UART0_BASE       0x40034000
#define UART0_DR         *(volatile unsigned int*)(UART0_BASE + 0x00)
#define UART0_FR         *(volatile unsigned int*)(UART0_BASE + 0x18)
#define UART0_IBRD       *(volatile unsigned int*)(UART0_BASE + 0x24)
#define UART0_FBRD       *(volatile unsigned int*)(UART0_BASE + 0x28)
#define UART0_LCRH       *(volatile unsigned int*)(UART0_BASE + 0x2C)
#define UART0_CR         *(volatile unsigned int*)(UART0_BASE + 0x30)
#define UART0_IMSC       *(volatile unsigned int*)(UART0_BASE + 0x38)
#define UART0_ICR        *(volatile unsigned int*)(UART0_BASE + 0x44)

// UART flags
#define UART_TXFF        (1 << 5)
#define UART_RXFE        (1 << 4)

// IO Bank base and GPIO control
#define IO_BANK0_BASE    0x40014000
#define GPIO0_CTRL       *(volatile unsigned int*)(IO_BANK0_BASE + 0x004)
#define GPIO1_CTRL       *(volatile unsigned int*)(IO_BANK0_BASE + 0x008)

// Pad control
#define PADS_BANK0_BASE  0x4001C000
#define PADS_GPIO0       *(volatile unsigned int*)(PADS_BANK0_BASE + 0x000)
#define PADS_GPIO1       *(volatile unsigned int*)(PADS_BANK0_BASE + 0x004)

// Reset control
#define RESETS_BASE         0x4000C000
#define RESETS_RESET        *(volatile unsigned int*)(RESETS_BASE + 0x0)
#define RESETS_RESET_DONE   *(volatile unsigned int*)(RESETS_BASE + 0x8)
#define RESET_UART0         (1 << 22)

// Function select
#define FUNC_UART        0

void uart_init() {
    // Bring UART0 out of reset
    RESETS_RESET &= ~RESET_UART0;
    while (!(RESETS_RESET_DONE & RESET_UART0));

    // Set GPIO0 (TX) and GPIO1 (RX) to function 0 (UART)
    PADS_GPIO0 = 0x50;
    PADS_GPIO1 = 0x50;
    GPIO0_CTRL = FUNC_UART;
    GPIO1_CTRL = FUNC_UART;

    // Disable UART0 before configuration
    UART0_CR = 0x00000000;

    // Baud rate for 115200 @ 48 MHz UART clock
    UART0_IBRD = 26;  // Integer part: 48,000,000 / (16 * 115200) = ~26.041
    UART0_FBRD = 3;   // Fractional part: .041 * 64 + 0.5 ≈ 3

    // Configure UART: 8-bit, no parity, 1 stop bit, FIFO enabled
    UART0_LCRH = (1 << 4) | (1 << 5) | (1 << 6);

    // Enable UART, TX, RX
    UART0_CR = (1 << 0) | (1 << 8) | (1 << 9);
}

void uart_putc(char c) {
    while (UART0_FR & UART_TXFF);  // Wait if TX FIFO is full
    UART0_DR = c;
}

void uart_puts(const char* s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r');  // Add carriage return before newline
        uart_putc(*s++);
    }
}

char uart_getc() {
    while (UART0_FR & UART_RXFE);  // Wait if RX FIFO is empty
    return (char)(UART0_DR & 0xFF);
}
