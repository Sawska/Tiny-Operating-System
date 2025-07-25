#include <stdint.h>

extern void _start();
extern void SysTick_Handler();

__attribute__((section(".vectors")))
void (*vectors[])() = {
    (void (*)())(0x10000000 + 0x1000),
    _start,                            
    0,0,0,0,0,0,0,0,0,0,0,0,
    SysTick_Handler                    
};

void irq_handler_io_bank0(void); 

__attribute__((section(".vector_table")))
void (* const vector_table[])(void) = {
    
    [16 + 13] = irq_handler_io_bank0, 
};



__attribute__((naked)) void SVC_Handler(void) {
    __asm__ volatile (
        "tst lr, #4\n"
        "ite eq\n"
        "mrseq r0, msp\n"
        "mrsne r0, psp\n"
        "b syscall_dispatch\n"
    );
}
