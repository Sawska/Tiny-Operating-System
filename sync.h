#pragma once

#include <stdint.h>

typedef struct {
    volatile int locked;
    int owner_task_id;
} Mutex;

typedef struct {
    volatile int count;
    int max_count;
} Semaphore;


void mutex_init(Mutex* mtx);
void mutex_lock(Mutex* mtx);
void mutex_unlock(Mutex* mtx);


void semaphore_init(Semaphore* sem, int initial, int max);
void semaphore_wait(Semaphore* sem);
void semaphore_post(Semaphore* sem);
