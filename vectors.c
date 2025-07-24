#include <stdint.h>

extern void _start();
extern void SysTick_Handler();

__attribute__((section(".vectors")))
void (*vectors[])() = {
    (void (*)())(0x10000000 + 0x1000), // initial SP
    _start,                            // reset handler
    0,0,0,0,0,0,0,0,0,0,0,0,
    SysTick_Handler                    // SysTick
};
