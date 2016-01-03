#include "stream.h"

#include "memory.h"

void stream_create(FStream** fs)
{
  *fs = (FStream*)mem_alloc(sizeof(FStream));
  (*fs)->handle = 0;
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

int stream_write(FStream *fs, void *buffer, unsigned length)
{
  fwrite(buffer, length, 1, fs->handle);
  return 0;
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

