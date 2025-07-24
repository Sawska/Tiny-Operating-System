#pragma once

#include <stdint.h>

typedef struct {
    volatile uint32_t message;
    volatile int full;
} Mailbox;

void mailbox_init(Mailbox* box);
int mailbox_send(Mailbox* box, uint32_t msg);
int mailbox_receive(Mailbox* box, uint32_t* out_msg);
