#pragma once

#define SIO_BASE  0xd0000000
#define GPIO_OUT  *(volatile unsigned int*)(SIO_BASE + 0x10)
#define GPIO_OE   *(volatile unsigned int*)(SIO_BASE + 0x20)

static inline void gpio_init(int pin) {
    GPIO_OE |= (1u << pin);
}

static inline void gpio_set(int pin, int value) {
    if (value)
        GPIO_OUT |= (1u << pin);
    else
        GPIO_OUT &= ~(1u << pin);
}
