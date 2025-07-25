#include "sync.h"
#include "scheduler.h"

extern int get_current_task_id();
extern void task_yield();


void mutex_init(Mutex* mtx) {
    mtx->locked = 0;
    mtx->owner_task_id = -1;
}

void mutex_lock(Mutex* mtx) {
    int id = get_current_task_id();
    while (__sync_lock_test_and_set(&mtx->locked, 1)) {
        task_yield(); 
    }
    mtx->owner_task_id = id;
}

void mutex_unlock(Mutex* mtx) {
    if (mtx->owner_task_id == get_current_task_id()) {
        mtx->owner_task_id = -1;
        __sync_lock_release(&mtx->locked);
    }
}


void semaphore_init(Semaphore* sem, int initial, int max) {
    sem->count = initial;
    sem->max_count = max;
}

void semaphore_wait(Semaphore* sem) {
    while (1) {
        if (sem->count > 0) {
            __sync_fetch_and_sub(&sem->count, 1);
            break;
        }
        task_yield();
    }
}

void semaphore_post(Semaphore* sem) {
    if (sem->count < sem->max_count) {
        __sync_fetch_and_add(&sem->count, 1);
    }
}
