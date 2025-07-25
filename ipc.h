#pragma once
#include <stdint.h>

#define IPC_QUEUE_SIZE 16

typedef struct {
    uint32_t buffer[IPC_QUEUE_SIZE];
    volatile int head;
    volatile int tail;
    volatile int count;
} IPCQueue;

void ipc_queue_init(IPCQueue* q);
int ipc_send(IPCQueue* q, uint32_t msg);
int ipc_receive(IPCQueue* q, uint32_t* msg);
