#include <stdint.h>

/* ARMv7-M vector table for the QEMU 'lm3s6965evb' build.
 *
 * The previous (RP2040) table placed SysTick at index 14 (the PendSV slot)
 * and never installed an SVCall handler, so SysTick interrupts vectored to a
 * zero entry and faulted. Here every architectural slot is at its correct
 * index: SP at 0, Reset at 1, SVCall at 11, PendSV at 14, SysTick at 15. */

extern void _start(void);
extern void SysTick_Handler(void);
extern void SVC_Handler(void);
extern uint32_t _stack_top;

extern void syscall_dispatch(uint32_t *stack_frame);

static void default_handler(void) {
    while (1) { }
}

/* On SVC, pick whichever stack (MSP/PSP) was active and hand its exception
 * frame to the C dispatcher. EXC_RETURN bit 2 tells us which stack was used. */
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

__attribute__((section(".vectors"), used))
void (* const vectors[16 + 1])(void) = {
    [0]  = (void (*)(void))(&_stack_top),  /* Initial stack pointer        */
    [1]  = _start,                          /* Reset                        */
    [2]  = default_handler,                 /* NMI                          */
    [3]  = default_handler,                 /* HardFault                    */
    [4]  = default_handler,                 /* MemManage                    */
    [5]  = default_handler,                 /* BusFault                     */
    [6]  = default_handler,                 /* UsageFault                   */
    [11] = SVC_Handler,                     /* SVCall                       */
    [14] = default_handler,                 /* PendSV                       */
    [15] = SysTick_Handler,                 /* SysTick                      */
};
