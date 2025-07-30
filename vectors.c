#include <stdint.h>

extern void _start(void);
extern void SysTick_Handler(void);
extern void syscall_dispatch(uint32_t *stack_frame);
extern void irq_handler_io_bank0(void);
extern uint32_t _stack_top;

/* Cortex-M0+ requires at least 16 entries before IRQs start */
__attribute__((section(".vectors")))
void (* const vectors[])(void) = {
    (void (*)(void))(&_stack_top), // Initial stack pointer
    _start,                        // Reset Handler
    0,                             // NMI Handler
    0,                             // HardFault Handler
    0, 0, 0, 0, 0, 0, 0,           // Reserved
    0,                             // SVC Handler (optional)
    0,                             // Reserved
    0,                             // PendSV Handler
    SysTick_Handler                // SysTick Handler
    // IRQ handlers would follow here
};

/* Example: mapping a specific IRQ */
__attribute__((section(".vector_table")))
void (* const vector_table[])(void) = {
    [16 + 13] = irq_handler_io_bank0, // Example: IRQ number 13
};

/* Cortex-M0+ safe SVC handler */
__attribute__((naked)) void SVC_Handler(void) {
    __asm__ volatile (
        "mov    r2, lr              \n" // Move LR into low register
        "movs   r3, #4              \n" // Constant #4
        "tst    r2, r3              \n" // Test EXC_RETURN bit
        "beq    1f                   \n" // If EQ: MSP
        "mrs    r0, psp              \n" // Else: PSP
        "b      2f                   \n"
    "1:                             \n"
        "mrs    r0, msp              \n"
    "2:                             \n"
        "b      syscall_dispatch     \n"
    );
}
