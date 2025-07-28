#include "fs.h"
#include "uart.h"
#include <string.h>

static FS_File fs_table[FS_MAX_FILES];

void fs_init() {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        fs_table[i].used = 0;
        fs_table[i].size = 0;
    }
}

int fs_create(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (!fs_table[i].used) {
            strncpy(fs_table[i].name, name, FS_MAX_FILENAME - 1);
            fs_table[i].name[FS_MAX_FILENAME - 1] = '\0';
            fs_table[i].used = 1;
            fs_table[i].size = 0;
            return 0;
        }
    }
    return -1; 
}

int fs_write(const char* name, const char* content) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && strncmp(fs_table[i].name, name, FS_MAX_FILENAME) == 0) {
            size_t len = strlen(content);
            if (len > FS_MAX_FILESIZE) len = FS_MAX_FILESIZE;
            strncpy(fs_table[i].data, content, len);
            fs_table[i].data[len] = '\0';
            fs_table[i].size = len;
            return 0;
        }
    }
    return -1; 
}

const char* fs_read(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && strncmp(fs_table[i].name, name, FS_MAX_FILENAME) == 0) {
            return fs_table[i].data;
        }
    }
    return NULL;
}

int fs_delete(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && strncmp(fs_table[i].name, name, FS_MAX_FILENAME) == 0) {
            fs_table[i].used = 0;
            fs_table[i].size = 0;
            return 0;
        }
    }
    return -1;
}

void fs_list() {
    uart_puts("Files:\n");
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used) {
            uart_puts(" - ");
            uart_puts(fs_table[i].name);
            uart_puts(" (");
            uart_putc('0' + fs_table[i].size);
            uart_puts(" bytes)\n");
        }
    }
}
