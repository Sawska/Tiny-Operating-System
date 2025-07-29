#include "fs.h"
#include "uart.h"
#include <stddef.h>



static size_t my_strlen(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}


static char* my_strncpy(char* dest, const char* src, size_t n) {
    size_t i = 0;
    for (; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    return dest;
}


static int my_strcmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        if (s1[i] == '\0') return 0;
    }
    return 0;
}


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
            my_strncpy(fs_table[i].name, name, FS_MAX_FILENAME - 1);
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
        if (fs_table[i].used && my_strcmp(fs_table[i].name, name, FS_MAX_FILENAME) == 0) {
            size_t len = my_strlen(content);
            if (len > FS_MAX_FILESIZE) len = FS_MAX_FILESIZE;
            my_strncpy(fs_table[i].data, content, len);
            fs_table[i].data[len] = '\0';
            fs_table[i].size = len;
            return 0;
        }
    }
    return -1; 
}

const char* fs_read(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && my_strcmp(fs_table[i].name, name, FS_MAX_FILENAME) == 0) {
            return fs_table[i].data;
        }
    }
    return NULL;
}

int fs_delete(const char* name) {
    for (int i = 0; i < FS_MAX_FILES; i++) {
        if (fs_table[i].used && my_strcmp(fs_table[i].name, name, FS_MAX_FILENAME) == 0) {
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
