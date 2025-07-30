#pragma once
#include <stdint.h>

#define MAX_TASKS 13
#define STACK_SIZE 1024

#define CAP_UART        (1 << 0)
#define CAP_GPIO        (1 << 1)
#define CAP_ADC         (1 << 2)
#define CAP_NETWORK     (1 << 3)
#define CAP_FILESYSTEM  (1 << 4)


typedef struct {
    uint32_t* sp;  
    uint8_t* stack_base;
    int active;
    int priority; 
    uint32_t capabilities; 
} Task;


void scheduler_init();
void task_create(void (*entry)(void), int priority, uint32_t caps);
void scheduler_start();
void SysTick_Handler();  

void yield(void);
void scheduler_exit(void);

int task_has_capability(uint32_t cap);

extern Task tasks[MAX_TASKS];
extern int total_tasks;