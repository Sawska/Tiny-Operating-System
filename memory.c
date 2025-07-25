#include "memory.h"

#define HEAP_START 0x20004000
#define HEAP_SIZE  (32 * 1024)

static unsigned char* heap_ptr = (unsigned char*)HEAP_START;

void* simple_malloc(unsigned int size) {
    void* alloc = (void*)heap_ptr;
    heap_ptr += size;

    if ((unsigned int)heap_ptr >= (HEAP_START + HEAP_SIZE)) {
        return 0;
    }

    return alloc;
}
