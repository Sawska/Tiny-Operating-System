#include "scheduler.h"
#include "memory.h"

Task tasks[MAX_TASKS];
int current_task = -1;
int total_tasks = 0;

extern void switch_context(uint32_t** old_sp, uint32_t* new_sp);

void scheduler_init() {
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].active = 0;
        tasks[i].sp = 0;
        tasks[i].stack_base = 0;
    }
}

void task_create(void (*entry)(void)) {
    if (total_tasks >= MAX_TASKS) return;

    uint8_t* stack = (uint8_t*)simple_malloc(STACK_SIZE);
    uint32_t* sp = (uint32_t*)(stack + STACK_SIZE);


    *(--sp) = (uint32_t)0x01000000; // xPSR
    *(--sp) = (uint32_t)entry;      // PC
    *(--sp) = 0xFFFFFFFD;           // LR 
    for (int i = 0; i < 5; ++i)
        *(--sp) = 0;                // R12, R3, R2, R1, R0

    for (int i = 0; i < 8; ++i)
        *(--sp) = 0;                // R4-R11

    tasks[total_tasks].sp = sp;
    tasks[total_tasks].stack_base = stack;
    tasks[total_tasks].active = 1;
    total_tasks++;
}

void scheduler_start() {
    // Setup SysTick (preemptive scheduling, e.g., every 1ms)
    *((volatile uint32_t*)0xE000E014) = 125000 - 1; // reload value (assumes 125MHz)
    *((volatile uint32_t*)0xE000E018) = 0;          // current value
    *((volatile uint32_t*)0xE000E010) = 7;          // enable SysTick with interrupt

    current_task = 0;
    uint32_t* sp = tasks[current_task].sp;
    asm volatile(
        "msr psp, %0\n"
        "mov r0, #3\n"
        "msr CONTROL, r0\n"
        "isb\n"
        :
        : "r" (sp)
        : "memory"
    );

    void (*task_entry)() = (void (*)())(((uint32_t*)sp)[15]); // retrieve original PC
    task_entry(); // jump to task
}

void SysTick_Handler() {
    int prev_task = current_task;
    current_task = (current_task + 1) % total_tasks;
    switch_context(&tasks[prev_task].sp, tasks[current_task].sp);
}