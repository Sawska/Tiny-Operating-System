#include <stdint.h>

/* RP2040 (Cortex-M0+) vector table, placed at 0x10000100 by the linker; the
 * second-stage bootloader points VTOR here before jumping to _start.
 *
 * One contiguous table: 16 architectural entries followed by the 32 external
 * IRQs. The previous version mis-placed SysTick (it landed in the PendSV slot)
 * and kept the IRQ entries in a separate section the linker never placed. */

extern void _start(void);
extern void SysTick_Handler(void);
extern void SVC_Handler(void);
extern void irq_handler_io_bank0(void);
extern uint32_t _stack_top;

static void default_handler(void) {
    while (1) { }
}

#define IO_IRQ_BANK0 13   /* RP2040 external interrupt number */

__attribute__((section(".vectors"), used))
void (* const vectors[16 + 32])(void) = {
    [0]  = (void (*)(void))(&_stack_top),  /* Initial stack pointer */
    [1]  = _start,                          /* Reset                 */
    [2]  = default_handler,                 /* NMI                   */
    [3]  = default_handler,                 /* HardFault             */
    [11] = SVC_Handler,                     /* SVCall                */
    [14] = default_handler,                 /* PendSV                */
    [15] = SysTick_Handler,                 /* SysTick               */
    [16 + IO_IRQ_BANK0] = irq_handler_io_bank0,
};

/* Selects MSP/PSP from the active stack and hands the exception frame to the
 * C syscall dispatcher (unchanged from the original design). */
__attribute__((naked)) void SVC_Handler(void) {
    __asm__ volatile (
        "movs   r0, #4              \n"
        "mov    r1, lr              \n"
        "tst    r0, r1              \n"
        "beq    1f                  \n"
        "mrs    r0, psp             \n"
        "b      2f                  \n"
    "1:                             \n"
        "mrs    r0, msp             \n"
    "2:                             \n"
        "ldr    r1, =syscall_dispatch \n"
        "bx     r1                  \n"
    );
}
