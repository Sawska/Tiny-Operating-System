#include "cmd_parser.h"
#include "uart.h"
#include "syscall.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CMD_BUFFER_SIZE 128

static char cmd_buffer[CMD_BUFFER_SIZE];
static uint8_t cmd_pos = 0;

static int tokenize(char* str, char* argv[], int max_args) {
    int argc = 0;
    char* token = strtok(str, " ");
    while(token && argc < max_args) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    return argc;
}

void cmd_parser_init(void) {
    cmd_pos = 0;
}

void cmd_parser_poll(void) {
    while (uart_available()) {
        char c = uart_getc();

        if (c == '\r' || c == '\n') {
            uart_puts("\r\n");
            cmd_buffer[cmd_pos] = 0; 

            if (cmd_pos > 0) {
                char* argv[10];
                int argc = tokenize(cmd_buffer, argv, 10);

                if (argc > 0) {
                    syscall_handle_command(argc, argv);
                }
            }

            cmd_pos = 0;
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
