#pragma once
#include <stdint.h>

#define PWM_BASE 0x40050000
#define PWM_CHAN_CSR(slice)   (*(volatile uint32_t*)(PWM_BASE + 0x000 + (slice) * 0x14))
#define PWM_CHAN_DIV(slice)   (*(volatile uint32_t*)(PWM_BASE + 0x004 + (slice) * 0x14))
#define PWM_CHAN_TOP(slice)   (*(volatile uint32_t*)(PWM_BASE + 0x008 + (slice) * 0x14))
#define PWM_CHAN_CC(slice)    (*(volatile uint32_t*)(PWM_BASE + 0x00C + (slice) * 0x14))
#define PWM_CHAN_ENABLE(slice)(*(volatile uint32_t*)(PWM_BASE + 0x0A0))

#define PWM_CLK_SYS 125000000 

static inline void pwm_set_gpio(int gpio, int slice, int channel) {

    volatile uint32_t* IO_BANK0_CTRL = (volatile uint32_t*)(0x40014000 + 0x04 + gpio * 8);
    *IO_BANK0_CTRL = (4 << 0);
    
    
    PWM_CHAN_CSR(slice) = (1 << 0);
}

static inline void pwm_configure(int slice, uint32_t freq, uint8_t duty_percent) {
    uint32_t divider16 = PWM_CLK_SYS / freq / 4096;
    if (divider16 < 16) divider16 = 16;
    if (divider16 > 255 * 16) divider16 = 255 * 16;

    uint32_t div = divider16 / 16;
    uint32_t top = PWM_CLK_SYS / (freq * div);

    PWM_CHAN_DIV(slice) = (div << 4);
    PWM_CHAN_TOP(slice) = top;
    PWM_CHAN_CC(slice) = (top * duty_percent) / 100;
}

static inline void pwm_set_duty(int slice, uint8_t duty_percent) {
    uint32_t top = PWM_CHAN_TOP(slice);
    PWM_CHAN_CC(slice) = (top * duty_percent) / 100;
}
