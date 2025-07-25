#pragma once

#include <stdint.h>

typedef enum {
    SYS_YIELD,
    SYS_PRINT,
    SYS_EXIT,
    SYS_USER_DEFINED_1,
    SYS_MAX
} SyscallNumber;

uint32_t syscall(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3);
