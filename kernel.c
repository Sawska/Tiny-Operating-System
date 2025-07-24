#include "gpio.h"
#include "memory.h"
#include "scheduler.h"
#include "mailbox.h"
#include "uart.h"

Mailbox box;

void task1() {
    int count = 0;
    while (1) {
        mailbox_send(&box, count++);
        uart_puts("Task1: sent value\n");
        for (volatile int i = 0; i < 100000; i++);
    }
}

void task2() {
    uint32_t msg = 0;
    while (1) {
        if (mailbox_receive(&box, &msg)) {
            uart_puts("Task2: received value = ");
            uart_putc('0' + (msg % 10));  // crude print
            uart_putc('\n');
        }
        for (volatile int i = 0; i < 100000; i++);
    }
}

void kernel_main(void) {
    gpio_init(25);
    uart_init();
    mailbox_init(&box);
    scheduler_init();
    task_create(task1);
    task_create(task2);
    scheduler_start();
}
