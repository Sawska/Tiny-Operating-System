#include "shell.h"
#include "uart.h"
#include "gpio.h"
#include "adc.h"




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

// Forward declarations
static void cmd_help(int argc, char* argv[]);
static void cmd_led(int argc, char* argv[]);
static void cmd_adc(int argc, char* argv[]);

static command_t commands[] = {
    { "help", cmd_help, "Show this help" },
    { "led",  cmd_led,  "led on|off" },
    { "adc",  cmd_adc,  "adc read <channel>" },
    { NULL, NULL, NULL }
};

static int my_strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2) return *s1 - *s2;
        s1++; s2++;
    }
    return *s1 - *s2;
}

static int my_tokenize(char* str, char* argv[], int max_args) {
    int argc = 0;
    char* p = str;

    while (*p && argc < max_args) {
        while (*p == ' ') p++;
        if (*p == 0) break;

        argv[argc++] = p;

        while (*p && *p != ' ') p++;

        if (*p) {
            *p = 0;
            p++;
        }
    }
    return argc;
}


static int my_atoi(const char* str) {
    int result = 0;
    int sign = 1;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return sign * result;
}


static int my_itoa(char* buf, unsigned int val) {
    int i = 0, j, start;
    if (val == 0) {
        buf[0] = '0';
        buf[1] = 0;
        return 1;
    }

    while (val > 0 && i < 31) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    start = 0; 
    for (j = i - 1; j > start; j--, start++) {
        char t = buf[start];
        buf[start] = buf[j];
        buf[j] = t;
    }
    buf[i] = 0;
    return i;
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
                int argc = my_tokenize(cmd_buffer, argv, MAX_ARGS);

                if (argc > 0) {
                    int found = 0;
                    for (int i = 0; commands[i].name != NULL; i++) {
                        if (my_strcmp(argv[0], commands[i].name) == 0) {
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
    if (my_strcmp(argv[1], "on") == 0) {
        gpio_set(25, 1);
        uart_puts("LED turned ON\r\n");
    }
    else if (my_strcmp(argv[1], "off") == 0) {
        gpio_set(25, 0);
        uart_puts("LED turned OFF\r\n");
    }
    else {
        uart_puts("Invalid argument. Use 'on' or 'off'\r\n");
    }
}

static void cmd_adc(int argc, char* argv[]) {
    if (argc < 3 || my_strcmp(argv[1], "read") != 0) {
        uart_puts("Usage: adc read <channel>\r\n");
        return;
    }
    int ch = my_atoi(argv[2]);
    if (ch < 0 || ch > 4) {
        uart_puts("Invalid ADC channel, use 0-4\r\n");
        return;
    }
    uint16_t val = adc_read(ch);

    char buf[32];
    int pos = 0;
    const char prefix[] = "ADC[";
    for (int i = 0; prefix[i] != 0; i++)
        buf[pos++] = prefix[i];

    pos += my_itoa(buf + pos, ch);

    const char mid[] = "] = ";
    for (int i = 0; mid[i] != 0; i++)
        buf[pos++] = mid[i];

    pos += my_itoa(buf + pos, val);

    const char suffix[] = "\r\n";
    for (int i = 0; suffix[i] != 0; i++)
        buf[pos++] = suffix[i];

    buf[pos] = 0;

    uart_puts(buf);
}
