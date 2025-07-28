#include "memory.h"
#include "uart.h"

#define HEAP_START 0x20004000
#define HEAP_SIZE  (32 * 1024)

#define MAGIC_ALLOCATED 0xBEEF
#define GUARD_VALUE     0xDEAD

typedef struct alloc_header {
    uint16_t magic;
    uint16_t size;
    struct alloc_header* next;
} alloc_header_t;

static unsigned char* heap_ptr = (unsigned char*)HEAP_START;
static alloc_header_t* alloc_list = 0;

void* simple_malloc(unsigned int size) {
    size = (size + 3) & ~3;

    unsigned int total_size = sizeof(alloc_header_t) + size + sizeof(uint16_t);

    if ((unsigned int)(heap_ptr + total_size) >= (HEAP_START + HEAP_SIZE)) {
        return 0;
    }

    alloc_header_t* header = (alloc_header_t*)heap_ptr;
    header->magic = MAGIC_ALLOCATED;
    header->size = size;
    header->next = alloc_list;

    alloc_list = header;

    void* user_ptr = (void*)(header + 1);
    uint16_t* guard = (uint16_t*)((unsigned char*)user_ptr + size);
    *guard = GUARD_VALUE;

    heap_ptr += total_size;

    return user_ptr;
}

void memory_check_leaks(void) {
    uart_puts("Checking for leaks:\n");
    alloc_header_t* curr = alloc_list;
    int count = 0;
    while (curr) {
        uart_puts(" Leaked block: size = ");
        uart_putint(curr->size);
        uart_puts(" bytes\n");
        curr = curr->next;
        count++;
    }
    if (count == 0)
        uart_puts(" No leaks found.\n");
}

void memory_check_corruption(void) {
    uart_puts("Checking for corruption:\n");
    alloc_header_t* curr = alloc_list;
    int count = 0;
    while (curr) {
        void* user_ptr = (void*)(curr + 1);
        uint16_t* guard = (uint16_t*)((unsigned char*)user_ptr + curr->size);
        if (*guard != GUARD_VALUE) {
            uart_puts(" Corruption detected! Block size = ");
            uart_putint(curr->size);
            uart_puts(" bytes\n");
            count++;
        }
        curr = curr->next;
    }
    if (count == 0)
        uart_puts(" No corruption detected.\n");
}
