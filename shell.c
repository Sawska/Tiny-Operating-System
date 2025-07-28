#include "shell.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CMD_BUFFER_SIZE 128
#define MAX_ARGS 8

static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_pos = 0;

typedef void (*command_func_t)(int argc, char* argv[]);

typedef struct {
    const char* name;
    command_func_t func;
    const char* help;
} command_t;

static void cmd_help(int argc, char* argv[]);
static void cmd_led(int argc, char* argv[]);
static void cmd_adc(int argc, char* argv[]);

static command_t commands[] = {
    { "help", cmd_help, "Show this help" },
    { "led",  cmd_led,  "led on|off" },
    { "adc",  cmd_adc,  "adc read <channel>" },
    { NULL, NULL, NULL }
};

static int tokenize(char* str, char* argv[], int max_args) {
    int argc = 0;
    char* token = strtok(str, " ");
    while (token && argc < max_args) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    return argc;
}

void shell_init(void) {
    cmd_pos = 0;
    uart_puts("Welcome to TinyOS Shell!\r\n> ");
}

void shell_poll(void) {
    while (uart_available()) {
        char c = uart_getc();

        if (c == '\r' || c == '\n') {
            uart_puts("\r\n");
            if (cmd_pos > 0) {
                cmd_buffer[cmd_pos] = 0;

                char* argv[MAX_ARGS];
                int argc = tokenize(cmd_buffer, argv, MAX_ARGS);

                if (argc > 0) {
                    int found = 0;
                    for (int i = 0; commands[i].name != NULL; i++) {
                        if (strcmp(argv[0], commands[i].name) == 0) {
                            commands[i].func(argc, argv);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) {
                        uart_puts("Unknown command. Type 'help' for commands.\r\n");
                    }
                }
            }
            cmd_pos = 0;
            uart_puts("> ");
        }
        else if (c == '\b' || c == 127) {
            if (cmd_pos > 0) {
                cmd_pos--;
                uart_puts("\b \b");
            }
        }
        else if (cmd_pos < CMD_BUFFER_SIZE - 1) {
            cmd_buffer[cmd_pos++] = c;
            uart_putc(c);
        }
    }
}



static void cmd_help(int argc, char* argv[]) {
    uart_puts("Available commands:\r\n");
    for (int i = 0; commands[i].name != NULL; i++) {
        uart_puts("  ");
        uart_puts(commands[i].name);
        uart_puts(" - ");
        uart_puts(commands[i].help);
        uart_puts("\r\n");
    }
}

static void cmd_led(int argc, char* argv[]) {
    if (argc < 2) {
        uart_puts("Usage: led on|off\r\n");
        return;
    }
    if (strcmp(argv[1], "on") == 0) {
        gpio_set(25, 1); 
        uart_puts("LED turned ON\r\n");
    }
    else if (strcmp(argv[1], "off") == 0) {
        gpio_set(25, 0);
        uart_puts("LED turned OFF\r\n");
    }
    else {
        uart_puts("Invalid argument. Use 'on' or 'off'\r\n");
    }
}

static void cmd_adc(int argc, char* argv[]) {
    if (argc < 3 || strcmp(argv[1], "read") != 0) {
        uart_puts("Usage: adc read <channel>\r\n");
        return;
    }
    int ch = atoi(argv[2]);
    if (ch < 0 || ch > 4) {
        uart_puts("Invalid ADC channel, use 0-4\r\n");
        return;
    }
    uint16_t val = adc_read(ch);
    char buf[32];
    snprintf(buf, sizeof(buf), "ADC[%d] = %d\r\n", ch, val);
    uart_puts(buf);
}
