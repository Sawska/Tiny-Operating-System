#pragma once

#include <stdint.h>

#define FS_MAX_FILES 10
#define FS_MAX_FILENAME 16
#define FS_MAX_FILESIZE 256

typedef struct {
    char name[FS_MAX_FILENAME];
    uint32_t size;
    char data[FS_MAX_FILESIZE];
    int used;
} FS_File;

void fs_init();
int fs_create(const char* name);
int fs_write(const char* name, const char* content);
const char* fs_read(const char* name);
int fs_delete(const char* name);
void fs_list();
