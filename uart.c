#include "uart.h"

#define UART0_BASE       0x40034000
#define UART0_DR         *(volatile unsigned int*)(UART0_BASE + 0x00)
#define UART0_FR         *(volatile unsigned int*)(UART0_BASE + 0x18)
#define UART0_IBRD       *(volatile unsigned int*)(UART0_BASE + 0x24)
#define UART0_FBRD       *(volatile unsigned int*)(UART0_BASE + 0x28)
#define UART0_LCRH       *(volatile unsigned int*)(UART0_BASE + 0x2C)
#define UART0_CR         *(volatile unsigned int*)(UART0_BASE + 0x30)
#define UART0_IMSC       *(volatile unsigned int*)(UART0_BASE + 0x38)
#define UART0_ICR        *(volatile unsigned int*)(UART0_BASE + 0x44)

#define UART_TXFF        (1 << 5)
#define UART_RXFE        (1 << 4)


#define IO_BANK0_BASE    0x40014000
#define GPIO0_CTRL       *(volatile unsigned int*)(IO_BANK0_BASE + 0x004)
#define GPIO1_CTRL       *(volatile unsigned int*)(IO_BANK0_BASE + 0x008)


#define PADS_BANK0_BASE  0x4001C000
#define PADS_GPIO0       *(volatile unsigned int*)(PADS_BANK0_BASE + 0x000)
#define PADS_GPIO1       *(volatile unsigned int*)(PADS_BANK0_BASE + 0x004)


#define RESETS_BASE         0x4000C000
#define RESETS_RESET        *(volatile unsigned int*)(RESETS_BASE + 0x0)
#define RESETS_RESET_DONE   *(volatile unsigned int*)(RESETS_BASE + 0x8)
#define RESET_UART0         (1 << 22)


#define FUNC_UART        0

void uart_init() {
    
    RESETS_RESET &= ~RESET_UART0;
    while (!(RESETS_RESET_DONE & RESET_UART0));

    
    PADS_GPIO0 = 0x50;
    PADS_GPIO1 = 0x50;
    GPIO0_CTRL = FUNC_UART;
    GPIO1_CTRL = FUNC_UART;

    
    UART0_CR = 0x00000000;

    
    UART0_IBRD = 26;
    UART0_FBRD = 3;

    
    UART0_LCRH = (1 << 4) | (1 << 5) | (1 << 6);

    
    UART0_CR = (1 << 0) | (1 << 8) | (1 << 9);
}

void uart_putc(char c) {
    while (UART0_FR & UART_TXFF);  
    UART0_DR = c;
}

void uart_puts(const char* s) {
    while (*s) {
        if (*s == '\n') uart_putc('\r'); 
        uart_putc(*s++);
    }
}

char uart_getc() {
    while (UART0_FR & UART_RXFE);  
    return (char)(UART0_DR & 0xFF);
}
