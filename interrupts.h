#pragma once
#include <stdint.h>

#define IO_BANK0_BASE      0x40014000
#define PROC0_INTS         0xE000E100
#define NVIC_ISER          ((volatile uint32_t*)(PROC0_INTS + 0x000))
#define NVIC_ICPR          ((volatile uint32_t*)(PROC0_INTS + 0x080))
#define IO_BANK0_IRQ0      13


#define IO_BANK0_INTR(pin)   (*(volatile uint32_t*)(IO_BANK0_BASE + 0x108 + (pin) * 8))
#define IO_BANK0_INTE(pin)   (*(volatile uint32_t*)(IO_BANK0_BASE + 0x128 + (pin) * 8))


#define IO_IRQ_EDGE_HIGH (1 << 3)
#define IO_IRQ_EDGE_LOW  (1 << 2)
