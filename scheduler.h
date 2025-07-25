#pragma once

#include <stdint.h>

#define MAX_TASKS 13
#define STACK_SIZE 1024

typedef struct {
    uint32_t* sp;  
    uint8_t* stack_base;
    int active;
    int priority; 
} Task;

void scheduler_init();
void task_create(void (*entry)(void), int priority);
void scheduler_start();
void SysTick_Handler();  
