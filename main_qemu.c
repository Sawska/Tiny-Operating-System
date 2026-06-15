/* QEMU demo entry point for the 'lm3s6965evb' build.
 *
 * Unlike kernel.c (which drives RP2040 GPIO peripherals that don't exist under
 * QEMU and would bus-fault), this exercises the parts of the OS we can observe
 * over the serial console: preemptive multitasking, the mailbox, the IPC
 * queue, the bump allocator, and the logger. */

#include <stdint.h>
#include "uart.h"
#include "scheduler.h"
#include "mailbox.h"
#include "ipc.h"
#include "log.h"
#include "memory.h"

static Mailbox box;          /* producer -> consumer (single slot) */
static IPCQueue log_queue;   /* producer -> sink     (ring buffer) */

static inline void irq_disable(void) { __asm volatile("cpsid i" ::: "memory"); }
static inline void irq_enable(void)  { __asm volatile("cpsie i" ::: "memory"); }

/* Print a whole line with interrupts masked so a preemption can never split
 * one task's output across another's. */
static void emit(const char* s) {
    irq_disable();
    uart_puts(s);
    irq_enable();
}

static void emit_dec(const char* prefix, uint32_t n) {
    char tmp[11];
    int j = 0;
    if (n == 0) tmp[j++] = '0';
    while (n) { tmp[j++] = (char)('0' + (n % 10)); n /= 10; }
    irq_disable();
    uart_puts(prefix);
    while (j) uart_putc(tmp[--j]);
    uart_putc('\n');
    irq_enable();
}

static void emit_hex(const char* prefix, uint32_t n) {
    static const char d[] = "0123456789abcdef";
    char tmp[8];
    for (int i = 0; i < 8; i++) { tmp[i] = d[(n >> ((7 - i) * 4)) & 0xF]; }
    irq_disable();
    uart_puts(prefix);
    uart_puts("0x");
    for (int i = 0; i < 8; i++) uart_putc(tmp[i]);
    uart_putc('\n');
    irq_enable();
}

static void busy(volatile uint32_t n) { while (n--) { } }

/* Periodically produces a value: hands it to the consumer via the mailbox and
 * to the sink via the IPC queue. */
static void producer_task(void) {
    uint32_t v = 0;
    for (;;) {
        if (mailbox_send(&box, v)) {
            emit_dec("[producer] sent  value ", v);
            ipc_send(&log_queue, v);
            v++;
        }
        busy(400000);
    }
}

/* Drains the mailbox. */
static void consumer_task(void) {
    uint32_t msg;
    for (;;) {
        if (mailbox_receive(&box, &msg)) {
            emit_dec("[consumer] recv  value ", msg);
        }
        busy(250000);
    }
}

/* Drains the IPC queue. */
static void sink_task(void) {
    uint32_t msg;
    for (;;) {
        if (ipc_receive(&log_queue, &msg)) {
            emit_dec("[sink]     logged item ", msg);
        }
        busy(600000);
    }
}

/* Independent task, proves the round-robin scheduler keeps everyone alive. */
static void heartbeat_task(void) {
    uint32_t t = 0;
    for (;;) {
        emit_dec("[heartbeat] tick ", t++);
        busy(900000);
    }
}

void kernel_main(void) {
    uart_init();

    emit("\n=== Tiny-OS booting on QEMU (lm3s6965evb / Cortex-M3) ===\n");
    log_info("kernel initialised");

    /* Show the bump allocator actually hands out usable RAM. */
    void* a = simple_malloc(64);
    void* b = simple_malloc(128);
    emit_hex("[mem] simple_malloc(64)  -> ", (uint32_t)a);
    emit_hex("[mem] simple_malloc(128) -> ", (uint32_t)b);

    mailbox_init(&box);
    ipc_queue_init(&log_queue);
    scheduler_init();

    task_create(heartbeat_task, 1, CAP_UART);
    task_create(producer_task,  2, CAP_UART);
    task_create(consumer_task,  3, CAP_UART);
    task_create(sink_task,      4, CAP_UART);

    emit("=== starting scheduler with 4 tasks ===\n");
    scheduler_start();

    for (;;) { }  /* not reached */
}
