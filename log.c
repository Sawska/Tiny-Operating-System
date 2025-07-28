#include "log.h"
#include "uart.h"
#include <stdarg.h>

static LogLevel current_level = LOG_LEVEL_DEBUG;

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
