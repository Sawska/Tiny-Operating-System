#include "scheduler.h"
#include "memory.h"
#include <uart.h>

Task tasks[MAX_TASKS];
int current_task = -1;
int total_tasks = 0;

extern void switch_context(uint32_t** old_sp, uint32_t* new_sp);

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
    uint32_t* sp = (uint32_t*)(stack + STACK_SIZE);

    *(--sp) = (uint32_t)0x01000000;
    *(--sp) = (uint32_t)entry;     
    *(--sp) = 0xFFFFFFFD;          
    for (int i = 0; i < 5; ++i)
        *(--sp) = 0;               
    for (int i = 0; i < 8; ++i)
        *(--sp) = 0;               

    tasks[total_tasks].sp = sp;
    tasks[total_tasks].stack_base = stack;
    tasks[total_tasks].active = 1;
    tasks[total_tasks].priority = priority;
    tasks[total_tasks].capabilities = caps; 
    total_tasks++;
}


void scheduler_start() {
    *((volatile uint32_t*)0xE000E014) = 125000 - 1; 
    *((volatile uint32_t*)0xE000E018) = 0;          
    *((volatile uint32_t*)0xE000E010) = 7;          

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

    void (*task_entry)() = (void (*)())(((uint32_t*)sp)[15]); 
    task_entry(); 
}


int get_next_task() {
    int best = -1;
    for (int i = 0; i < total_tasks; ++i) {
        if (!tasks[i].active) continue;
        if (best == -1 || tasks[i].priority < tasks[best].priority) {
            best = i;
        }
    }
    return best;
}

void SysTick_Handler() {
    int prev_task = current_task;
    current_task = get_next_task();

    if (current_task != -1 && current_task != prev_task) {
        switch_context(&tasks[prev_task].sp, tasks[current_task].sp);
    }
}

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

 
    yield();
}


