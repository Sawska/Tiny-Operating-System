#include "ipc.h"

void ipc_queue_init(IPCQueue* q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
}

int ipc_send(IPCQueue* q, uint32_t msg) {
    if (q->count >= IPC_QUEUE_SIZE)
        return 0;

    q->buffer[q->tail] = msg;
    q->tail = (q->tail + 1) % IPC_QUEUE_SIZE;
    q->count++;
    return 1;
}

int ipc_receive(IPCQueue* q, uint32_t* msg) {
    if (q->count == 0)
        return 0;

    *msg = q->buffer[q->head];
    q->head = (q->head + 1) % IPC_QUEUE_SIZE;
    q->count--;
    return 1;
}
