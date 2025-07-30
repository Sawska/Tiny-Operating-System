#include "log.h"
#include "uart.h"
#include <stdarg.h>

static LogLevel current_level = LOG_LEVEL_DEBUG;

typedef unsigned int size_t; 
typedef unsigned int uint32_t;
typedef int int32_t;




static void reverse(char* str, size_t len) {
    size_t i = 0, j = len - 1;
    while (i < j) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
        i++; j--;
    }
}

static size_t itoa(int32_t value, char* buf, size_t buf_size, int base, int is_unsigned) {
    static const char digits[] = "0123456789abcdef";
    size_t i = 0;
    uint32_t uvalue = (uint32_t)value;

    if (!is_unsigned && value < 0) {
        uvalue = (uint32_t)(-value);
    }

    if (uvalue == 0) {
        if (buf_size > 1) {
            buf[i++] = '0';
            buf[i] = '\0';
            return 1;
        }
        return 0;
    }

    while (uvalue && i < buf_size - 1) {
        buf[i++] = digits[uvalue % base];
        uvalue /= base;
    }

    if (!is_unsigned && value < 0) {
        if (i < buf_size - 1) {
            buf[i++] = '-';
        }
    }

    buf[i] = '\0';
    reverse(buf, i);
    return i;
}



int vsnprintf(char* buffer, size_t buffer_size, const char* fmt, va_list args) {
    size_t pos = 0;
    char numbuf[32];

    for (; *fmt && pos < buffer_size; fmt++) {
        if (*fmt != '%') {
            buffer[pos++] = *fmt;
            continue;
        }

        fmt++;  // Skip '%'

        int long_flag = 0;
        if (*fmt == 'l') {  // support %ld or %lu, ignore for simplicity
            long_flag = 1;
            fmt++;
        }

        switch (*fmt) {
            case 'd': {
                int val = long_flag ? va_arg(args, long) : va_arg(args, int);
                size_t len = itoa(val, numbuf, sizeof(numbuf), 10, 0);
                for (size_t i = 0; i < len && pos < buffer_size; i++)
                    buffer[pos++] = numbuf[i];
                break;
            }
            case 'u': {
                unsigned int val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                size_t len = itoa(val, numbuf, sizeof(numbuf), 10, 1);
                for (size_t i = 0; i < len && pos < buffer_size; i++)
                    buffer[pos++] = numbuf[i];
                break;
            }
            case 'x': {
                unsigned int val = long_flag ? va_arg(args, unsigned long) : va_arg(args, unsigned int);
                size_t len = itoa(val, numbuf, sizeof(numbuf), 16, 1);
                for (size_t i = 0; i < len && pos < buffer_size; i++)
                    buffer[pos++] = numbuf[i];
                break;
            }
            case 'c': {
                char c = (char)va_arg(args, int);
                if (pos < buffer_size)
                    buffer[pos++] = c;
                break;
            }
            case 's': {
                const char* s = va_arg(args, const char*);
                while (*s && pos < buffer_size)
                    buffer[pos++] = *s++;
                break;
            }
            case '%': {
                if (pos < buffer_size)
                    buffer[pos++] = '%';
                break;
            }
            default:
                // Unsupported specifier, print it literally
                if (pos < buffer_size)
                    buffer[pos++] = '%';
                if (pos < buffer_size)
                    buffer[pos++] = *fmt;
                break;
        }
    }

    if (pos < buffer_size)
        buffer[pos] = '\0';
    else if (buffer_size > 0)
        buffer[buffer_size - 1] = '\0';

    return (int)pos;
}

void log_set_level(LogLevel level) {
    current_level = level;
}

static const char* level_to_str(LogLevel level) {
    switch (level) {
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_WARN:  return "WARN";
        case LOG_LEVEL_ERROR: return "ERROR";
        default:              return "UNK";
    }
}

static void log_printf(LogLevel level, const char* fmt, va_list args) {
    if (level < current_level) return;

    uart_puts("[");
    uart_puts(level_to_str(level));
    uart_puts("] ");

    char buffer[128]; 
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    uart_puts(buffer);
    uart_puts("\n");
}

void log_debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_printf(LOG_LEVEL_DEBUG, fmt, args);
    va_end(args);
}

void log_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_printf(LOG_LEVEL_INFO, fmt, args);
    va_end(args);
}

void log_warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_printf(LOG_LEVEL_WARN, fmt, args);
    va_end(args);
}

void log_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    log_printf(LOG_LEVEL_ERROR, fmt, args);
    va_end(args);
}
