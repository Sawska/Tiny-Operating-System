#pragma once
#include "interrupts.h"

#define SIO_BASE  0xd0000000
#define GPIO_OUT  *(volatile unsigned int*)(SIO_BASE + 0x10)
#define GPIO_OE   *(volatile unsigned int*)(SIO_BASE + 0x20)
#define GPIO_IN   *(volatile unsigned int*)(SIO_BASE + 0x04)

static inline int gpio_get(int pin) {
    return (GPIO_IN >> pin) & 1u;
}


static inline void gpio_init(int pin) {
    GPIO_OE |= (1u << pin);
}

static inline void gpio_set(int pin, int value) {
    if (value)
        GPIO_OUT |= (1u << pin);
    else
        GPIO_OUT &= ~(1u << pin);
}

static inline void gpio_set_output(int pin) {
    GPIO_OE |= (1u << pin);
}

static inline void gpio_set_input(int pin) {
    GPIO_OE &= ~(1u << pin);
}

static inline void gpio_toggle(int pin) {
    GPIO_OUT ^= (1u << pin);
}

static inline  void delay_cycles(volatile int count) {
    while (count--);
}


static inline void gpio_set_function(uint32_t pin, uint32_t function) {
    volatile uint32_t *ctrl_reg = (uint32_t *)(IO_BANK0_BASE + 0x04 + (pin * 8));
    *ctrl_reg = function & 0x1F;
}

static inline  void gpio_enable_irq(int pin, int rising, int falling) {
    
    uint32_t irq_mask = 0;
    if (rising) irq_mask |= IO_IRQ_EDGE_HIGH;
    if (falling) irq_mask |= IO_IRQ_EDGE_LOW;

    IO_BANK0_INTR(pin) = irq_mask;
    IO_BANK0_INTE(pin) = irq_mask;

    
    NVIC_ISER[IO_BANK0_IRQ0 / 32] = (1 << (IO_BANK0_IRQ0 % 32));
}
