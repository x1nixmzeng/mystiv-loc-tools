#include "stream.h"

#include "memory.h"

void stream_create(FStream** fs)
{
  *fs = (FStream*)mem_alloc(sizeof(FStream));

  (*fs)->handle = 0;
  (*fs)->length = 0;
}

void stream_destroy(FStream** fs)
{
  if( (*fs)->handle != 0 ) {
    fclose((*fs)->handle);
  }

  mem_free(*fs);
  *fs = 0;
}

int stream_pos(FStream* fs)
{
  return ftell(fs->handle);
}

void stream_seek(FStream* fs, int pos)
{
  fseek(fs->handle, pos, SEEK_SET);
}

int stream_make(FStream* fs, const char *szFileName)
{
  fs->handle = fopen(szFileName, "wb");
  fs->length = 0;

  return (fs->handle == 0 ? 1 : 0);
}

int stream_open(FStream* fs, const char *szFileName)
{
  fs->handle = fopen(szFileName, "rb");
  fseek(fs->handle, 0, SEEK_END);
  fs->length = ftell(fs->handle);
  fseek(fs->handle, 0, SEEK_SET);

  return (fs->handle == 0 ? 1 : 0);
}

int stream_write(FStream *fs, const void *buffer, unsigned length)
{
  fwrite(buffer, length, 1, fs->handle);
  return 0;
}

int stream_write_intstring(FStream *fs, int value)
{
  int len;
  short buf[11]; // 2 147 483 647
  
  len = 0;

  do
  {
    buf[10-len] = (char)(value % 10) + '0';
    ++len;
    value /= 10;
  } while (value != 0);
  
  stream_write(fs, &buf[11-len], len * sizeof(short));

  return 0;
}

const char alphanum[] = "0123456789abcdef";

int stream_write_hex(FStream *fs, const void* buffer, unsigned int length)
{
  unsigned int i, val;
  const char *buf;

  buf = (const char*)buffer;
  
  for (i = 0; i < length; ++i, ++buf)
  {
    val = (int)alphanum[((*buf) >> 4) & 0xF] | (int)(alphanum[(*buf) & 0xF]) << 16;

    stream_write(fs, &val, 4);
  }

  return 0;
}

int stream_write_string(FStream *fs, String *str)
{
  if (str->val != 0) {
    return stream_write(fs, str->val, str->length);
  }

  return 1;
}

int stream_write_wstring(FStream *fs, WString *wstr)
{
  if (wstr->val != 0) {
    return stream_write(fs, wstr->val, wstr->length * 2);
  }

  return 1;
}

int stream_read(FStream *fs, void *buffer, unsigned int length)
{
  fread(buffer, length, 1, fs->handle);
  return 0;
}

int stream_read_int(FStream *fs)
{
  int value;

  stream_read(fs, &value, 4);

  return value;
}

char stream_read_char(FStream *fs)
{
  char value;

  stream_read(fs, &value, 1);

  return value;
}

String* stream_read_cstring(FStream *fs)
{
  String* str;

  string_create(&str);

  str->length = stream_read_int(fs);

  if( str->length > 0 ) {
    str->val = (char *)mem_alloc(str->length +1);

    stream_read(fs, str->val, str->length);

    str->val[str->length] = 0;
  }

  return str;
}

WString* stream_read_cwstring(FStream *fs)
{
  WString* wstr;

  wstring_create(&wstr);

  wstr->length = stream_read_int(fs);

  if( wstr->length > 0 ) {
    wstr->val = (short *)mem_alloc((wstr->length +1) * sizeof(short));

    stream_read(fs, wstr->val, wstr->length * 2);

    wstr->val[wstr->length] = 0;
  }

  return wstr;
}

int stream_insert_stream(FStream* fs, FStream* src, void* buffer, int buf_size)
{
  int i;

  i = 0;
  while (i < src->length)
  {
    int block;

    block = src->length - i;

    if (block > buf_size) {
      block = buf_size;
    }

    stream_read(src, buffer, block);
    stream_write(fs, buffer, block);

    i += block;
  }

  return 0;
}
