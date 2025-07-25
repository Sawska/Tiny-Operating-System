#include "syscall.h"
#include "uart.h"  
#include "scheduler.h"

extern void yield();

uint32_t syscall(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    uint32_t ret;

    __asm__ volatile (
        "mov r0, %1\n"
        "mov r1, %2\n"
        "mov r2, %3\n"
        "mov r3, %4\n"
        "svc #0\n"
        "mov %0, r0\n"
        : "=r" (ret)
        : "r" (syscall_num), "r" (arg1), "r" (arg2), "r" (arg3)
        : "r0", "r1", "r2", "r3"
    );

    return ret;
}

