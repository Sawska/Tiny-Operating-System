#include <stdint.h>

extern void _start(void);
extern void SysTick_Handler(void);
extern void syscall_dispatch(uint32_t *stack_frame);
extern void irq_handler_io_bank0(void);
extern uint32_t _stack_top;

__attribute__((section(".vectors")))
void (* const vectors[])(void) = {
    (void (*)(void))(&_stack_top),
    _start,                       
    0,                            
    0,                            
    0, 0, 0, 0, 0, 0, 0,          
    0,                            
    0,                            
    0,                            
    SysTick_Handler               
    
};

__attribute__((section(".vector_table")))
void (* const vector_table[])(void) = {
    [16 + 13] = irq_handler_io_bank0, 
};

__attribute__((naked)) void SVC_Handler(void) {
    __asm__ volatile (
        "mov    r2, lr              \n"
        "movs   r3, #4              \n"
        "tst    r2, r3              \n"
        "beq    1f                   \n"
        "mrs    r0, psp              \n"
        "b      2f                   \n"
    "1:                             \n"
        "mrs    r0, msp              \n"
    "2:                             \n"
        "b      syscall_dispatch     \n"
    );
}
