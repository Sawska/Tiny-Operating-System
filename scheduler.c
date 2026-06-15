#include "scheduler.h"
#include "memory.h"
#include <uart.h>

Task tasks[MAX_TASKS];
int current_task = -1;
int total_tasks = 0;

void scheduler_init() {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].active = 0;
        tasks[i].sp = 0;
        tasks[i].stack_base = 0;
        tasks[i].priority = 255;
    }
}

void task_create(void (*entry)(void), int priority, uint32_t caps) {
    if (total_tasks >= MAX_TASKS) return;

    uint8_t* stack = (uint8_t*)simple_malloc(STACK_SIZE);
    /* The exception frame must be 8-byte aligned, so round the top down. */
    uint32_t* sp = (uint32_t*)(((uintptr_t)(stack + STACK_SIZE)) & ~((uintptr_t)7));

    /* Build the frame the hardware/handler expects, top-down:
     *   [hardware-stacked] xPSR, PC(entry), LR, R12, R3, R2, R1, R0
     *   [software-saved]   R11..R4
     * On the first exception return the CPU pops the hardware frame and
     * jumps to `entry`. */
    *(--sp) = (uint32_t)0x01000000;        /* xPSR: Thumb bit set            */
    *(--sp) = (uint32_t)entry;             /* PC: task entry point           */
    *(--sp) = (uint32_t)scheduler_exit;    /* LR: where the task goes if it returns */
    for (int i = 0; i < 5; ++i)
        *(--sp) = 0;                       /* R12, R3, R2, R1, R0            */
    for (int i = 0; i < 8; ++i)
        *(--sp) = 0;                       /* R11..R4 (software-saved)       */

    tasks[total_tasks].sp = sp;
    tasks[total_tasks].stack_base = stack;
    tasks[total_tasks].active = 1;
    tasks[total_tasks].priority = priority;
    tasks[total_tasks].capabilities = caps; 
    total_tasks++;
}


/* Round-robin: pick the next active task after the current one. The old
 * strict-priority version always returned the single highest-priority task,
 * so nothing else ever ran. */
int get_next_task() {
    if (total_tasks == 0) return -1;
    for (int n = 1; n <= total_tasks; ++n) {
        int i = (current_task + n) % total_tasks;
        if (tasks[i].active) return i;
    }
    return tasks[current_task].active ? current_task : -1;
}

void scheduler_start() {
    /* SysTick drives preemptive scheduling. */
    *((volatile uint32_t*)0xE000E014) = 125000 - 1;  /* reload value          */
    *((volatile uint32_t*)0xE000E018) = 0;           /* current value         */
    *((volatile uint32_t*)0xE000E010) = 7;           /* enable|tickint|clksrc */

    current_task = 0;

    /* Launch the first task: aim PSP at its hardware-stacked frame, switch the
     * thread stack to PSP, then unstack the frame by hand and branch to the
     * entry point. From here on, SysTick_Handler performs all switches. This
     * sequence uses only ARMv6-M instructions, so it serves both the Cortex-M3
     * (QEMU) and Cortex-M0+ (RP2040) targets. */
    __asm volatile (
        ".syntax unified      \n"
        "ldr  r0, =tasks      \n"  /* r0 = &tasks[0]; sp is the first member */
        "ldr  r0, [r0]        \n"  /* r0 = tasks[0].sp (points at saved R4)  */
        "adds r0, #32         \n"  /* skip R4..R11 -> hardware frame (R0)    */
        "msr  psp, r0         \n"
        "movs r1, #2          \n"  /* CONTROL.SPSEL=1 -> use PSP, privileged */
        "msr  control, r1     \n"
        "isb                  \n"
        "pop  {r0-r3}         \n"  /* R0..R3                                 */
        "pop  {r4}            \n"  /* R12                                    */
        "mov  r12, r4         \n"
        "pop  {r4}            \n"  /* LR                                     */
        "mov  lr, r4          \n"
        "pop  {r4}            \n"  /* PC -> task entry                       */
        "pop  {r5}            \n"  /* xPSR (discarded)                       */
        "cpsie i              \n"
        "bx   r4              \n"
        ::: "memory"
    );
    while (1) { }  /* not reached */
}

/* Invoked from SysTick_Handler with the outgoing task's stack pointer (already
 * adjusted below its saved R4..R11). Records it, advances the schedule, and
 * returns the incoming task's stack pointer. */
uint32_t* context_switch_pick(uint32_t* old_sp) {
    tasks[current_task].sp = old_sp;
    int next = get_next_task();
    if (next != -1) current_task = next;
    return tasks[current_task].sp;
}

/* Preemptive context switch, done entirely on exception entry/exit:
 *   save R4-R11 below PSP, swap stacks via context_switch_pick, restore
 *   R4-R11, and let the exception return pop the rest and resume the task. */
#if defined(BOARD_LM3S)
/* Cortex-M3: STMDB/LDMIA can move R4-R11 directly. */
__attribute__((naked)) void SysTick_Handler(void) {
    __asm volatile (
        ".syntax unified           \n"
        "mrs   r0, psp             \n"
        "stmdb r0!, {r4-r11}       \n"
        "push  {lr}                \n"  /* preserve EXC_RETURN across the call */
        "bl    context_switch_pick \n"
        "pop   {lr}                \n"
        "ldmia r0!, {r4-r11}       \n"
        "msr   psp, r0             \n"
        "bx    lr                  \n"
    );
}
#else
/* Cortex-M0+: no STMDB and no multi-store of the high registers, so save and
 * restore R4-R11 by hand through the low registers. Layout below PSP, lowest
 * address first: R4,R5,R6,R7,R8,R9,R10,R11. */
__attribute__((naked)) void SysTick_Handler(void) {
    __asm volatile (
        ".syntax unified           \n"
        "mrs   r0, psp             \n"
        "subs  r0, #32             \n"  /* room for 8 saved registers        */
        "mov   r2, r0              \n"  /* r2 = save base (becomes old_sp)    */
        "stmia r0!, {r4-r7}        \n"  /* R4-R7  -> base..base+12           */
        "mov   r4, r8              \n"
        "mov   r5, r9              \n"
        "mov   r6, r10             \n"
        "mov   r7, r11             \n"
        "stmia r0!, {r4-r7}        \n"  /* R8-R11 -> base+16..base+28        */
        "mov   r0, r2              \n"  /* r0 = old_sp                       */
        "push  {lr}                \n"  /* preserve EXC_RETURN across call    */
        "bl    context_switch_pick \n"  /* r0 = new save base                */
        "pop   {r1}                \n"  /* r1 = EXC_RETURN                   */
        "mov   r2, r0              \n"
        "adds  r2, #16             \n"  /* -> saved R8-R11                   */
        "ldmia r2!, {r4-r7}        \n"
        "mov   r8, r4              \n"
        "mov   r9, r5              \n"
        "mov   r10, r6             \n"
        "mov   r11, r7             \n"
        "ldmia r0!, {r4-r7}        \n"  /* restore R4-R7 (r0 -> base+16)     */
        "adds  r0, #16             \n"  /* r0 = base+32 = task PSP           */
        "msr   psp, r0             \n"
        "bx    r1                  \n"
    );
}
#endif

int task_has_capability(uint32_t cap) {
    if (current_task < 0 || current_task >= total_tasks) return 0;
    return (tasks[current_task].capabilities & cap) != 0;
}

void yield(void) {
 
    *(volatile uint32_t*)0xE000ED04 = (1UL << 28);
}

void scheduler_exit(void) {
    if (current_task >= 0 && current_task < total_tasks) {
        tasks[current_task].active = 0;
        uart_puts("Task exited\r\n");
    }

    /* Mark inactive and spin until the next SysTick preempts us; the scheduler
     * skips inactive tasks, so this one is never resumed. */
    for (;;) { }
}


