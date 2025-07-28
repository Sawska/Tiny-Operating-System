#include "syscall.h"
#include "uart.h"  
#include "scheduler.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>

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


static void syscall_led(int argc, char* argv[]) {
    if (argc < 2) {
        uart_puts("Usage: led on|off\r\n");
        return;
    }
    if (strcmp(argv[1], "on") == 0) {
        uart_puts("LED turned ON\r\n");
    }
    else if (strcmp(argv[1], "off") == 0) {
        uart_puts("LED turned OFF\r\n");
    }
    else {
        uart_puts("Invalid argument, use on or off\r\n");
    }
}

static void syscall_adc(int argc, char* argv[]) {
    if (argc < 3 || strcmp(argv[1], "read") != 0) {
        uart_puts("Usage: adc read <channel>\r\n");
        return;
    }
    int ch = atoi(argv[2]);
    if (ch < 0 || ch > 4) {
        uart_puts("Invalid ADC channel, use 0-4\r\n");
        return;
    }

    uint16_t val = 1234; 
    char buf[32];
    snprintf(buf, sizeof(buf), "ADC[%d] = %d\r\n", ch, val);
    uart_puts(buf);
}

void syscall_handle_command(int argc, char* argv[]) {
    if (strcmp(argv[0], "led") == 0) {
        syscall_led(argc, argv);
    }
    else if (strcmp(argv[0], "adc") == 0) {
        syscall_adc(argc, argv);
    }
    else if (strcmp(argv[0], "help") == 0) {
        uart_puts("Available commands:\r\n");
        uart_puts("  led on|off\r\n");
        uart_puts("  adc read <channel>\r\n");
    }
    else {
        uart_puts("Unknown command\r\n");
    }
}

#include "scheduler.h"
#include "uart.h"
#include "gpio.h"

void syscall_gpio_set(int pin, int value) {
    if (!task_has_capability(CAP_GPIO)) {
        uart_puts("Error: Permission denied\r\n");
        return;
    }
    gpio_set(pin, value);
    uart_puts("GPIO set\r\n");
}