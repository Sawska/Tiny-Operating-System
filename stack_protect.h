#ifndef STACK_PROTECT_H
#define STACK_PROTECT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define STACK_GUARD_SIZE_BYTES 16
#define STACK_GUARD_PATTERN    0xDEADBEEF


void stack_init_guard(uint32_t* stack_base, size_t stack_size_words);


bool stack_check_guard(uint32_t* stack_base);

#endif // STACK_PROTECT_H
