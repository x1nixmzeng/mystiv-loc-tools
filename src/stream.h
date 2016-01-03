#ifndef STREAM_HEADER_H
#define STREAM_HEADER_H

#include <stdio.h>

#include "string.h"
#include "wstring.h"

typedef struct FileStream
{
 FILE *handle;
 unsigned int length;
} FStream;

void stream_create(FStream** fs);

void stream_destroy(FStream** fs);

int stream_pos(FStream* fs);

void stream_seek(FStream* fs, int pos);

int stream_make(FStream* fs, const char *szFileName);

int stream_open(FStream* fs, const char *szFileName);

int stream_read(FStream *fs, void *buffer, unsigned int length);

int stream_write(FStream *fs, void *buffer, unsigned int length);

int stream_read_int(FStream *fs);

char stream_read_char(FStream *fs);

String* stream_read_cstring(FStream *fs);

WString* stream_read_cwstring(FStream *fs);

#endif // STREAM_HEADER_H
