#include <stdint.h>
#include "uart.h"
#include "scheduler.h"
#include "syscall.h"


void syscall_dispatch(uint32_t *stack_frame) {
    uint32_t syscall_num = stack_frame[0];
    uint32_t arg1 = stack_frame[1];
    uint32_t arg2 = stack_frame[2];
    uint32_t arg3 = stack_frame[3];

    uint32_t result = 0;

    switch (syscall_num) {
        case SYS_YIELD:
            yield();  
            break;

        case SYS_PRINT:
            uart_putc((char)arg1);
            break;

        case SYS_EXIT:
            uart_puts("Task exiting\n");
            scheduler_exit();
            break;

        case SYS_USER_DEFINED_1:
            
            break;

        default:
            uart_puts("Unknown syscall\n");
            break;
    }

    stack_frame[0] = result;
}
