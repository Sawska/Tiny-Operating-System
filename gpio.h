#pragma once

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

void delay_cycles(volatile int count) {
    while (count--);
}
