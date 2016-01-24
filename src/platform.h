#ifndef PLATFORM_HEADER_H
#define PLATFORM_HEADER_H

#include "string.h"

typedef void(*PlatformFileCallback)(const char *name, int size, void* userdata);
typedef void(*PlatformDirCallback)(const char *name, void* userdata);

void platform_run_dirscan(String* path, PlatformFileCallback f_cb, PlatformDirCallback d_cb, void* userdata);

void platform_make_path(String* path);
void platform_make_full_path(String* path);

#endif // PLATFORM_HEADER_H
