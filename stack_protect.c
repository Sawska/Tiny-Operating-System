#include "stack_protect.h"

void stack_init_guard(uint32_t* stack_base, size_t stack_size_words) {
    size_t guard_words = STACK_GUARD_SIZE_BYTES / sizeof(uint32_t);
    if (guard_words > stack_size_words) {

        return;
    }

    for (size_t i = 0; i < guard_words; i++) {
        stack_base[i] = STACK_GUARD_PATTERN;
    }
}

bool stack_check_guard(uint32_t* stack_base) {
    size_t guard_words = STACK_GUARD_SIZE_BYTES / sizeof(uint32_t);

    for (size_t i = 0; i < guard_words; i++) {
        if (stack_base[i] != STACK_GUARD_PATTERN) {
            return false; 
        }
    }
    return true; 
}
