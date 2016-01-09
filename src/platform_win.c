#include "platform.h"
#include "textrange.h"

#include <windows.h>

void create_dir_internal(String *path)
{
  CreateDirectory(string_get(path), 0);
}

void platform_make_path(String* path)
{
  create_dir_internal(path);
}

void platform_make_full_path(String* path)
{
  Range *span_path;
  Range *span_subpath;
  
  range_create(&span_path);
  range_create(&span_subpath);

  span_path->begin = string_get(path);
  span_path->end = span_path->begin + path->length;

  span_subpath->begin = span_path->begin;
  span_subpath->end = span_path->begin; // Not a typo

  while (span_subpath->end < span_path->end) {
    if (*span_subpath->end == '\\' || *span_subpath->end == '/') {
      String *tmp;
      tmp = range_make_string(span_subpath);

      platform_make_path(tmp);

      string_destroy(&tmp);
    }

    ++span_subpath->end;
  }

  platform_make_path(path);
  
  range_destroy(&span_subpath);
  range_destroy(&span_path);
}