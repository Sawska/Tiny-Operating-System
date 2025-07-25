#include "mailbox.h"

void mailbox_init(Mailbox* box) {
    box->full = 0;
    box->message = 0;
}

int mailbox_send(Mailbox* box, uint32_t msg) {
    if (box->full) return 0; 
    box->message = msg;
    box->full = 1;
    return 1; 
}

int mailbox_receive(Mailbox* box, uint32_t* out_msg) {
    if (!box->full) return 0;
    *out_msg = box->message;
    box->full = 0;
    return 1; 
}
