#pragma once

#include <stdint.h>

void* simple_malloc(unsigned int size);
void memory_check_leaks(void);
void memory_check_corruption(void);
