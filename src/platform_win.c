#include "platform.h"
#include "textrange.h"
#include "memory.h"
#include "string.h"

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

int special_dir(const char *path)
{
  int result;

  result = 0;

  // current dir
  if (path[0] == '.' && path[1] == 0) {
    result = 1;
  }

  // parent link
  if (path[0] == '.' && path[1] == '.' && path[2] == 0) {
    result = 1;
  }

  return result;
}

void platform_run_dirscan(String* path, PlatformFileCallback f_cb, PlatformDirCallback d_cb, void* userdata)
{
  String *search_path;
  HANDLE handle;
  WIN32_FIND_DATA data;

  search_path = string_concat_cstring(2, string_get(path), "\\*");

  handle = FindFirstFileA(string_get(search_path), &data);

  if (handle != INVALID_HANDLE_VALUE) {
    do {
      if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
        if (d_cb != 0) {
          if (special_dir(data.cFileName) == 0) {
            d_cb(data.cFileName, userdata);
          }
        }
      }
      else {
        if (f_cb != 0) {
          f_cb(data.cFileName, data.nFileSizeLow, userdata);
        }
      }
    } while (FindNextFileA(handle, &data));

    FindClose(handle);
  }

  string_destroy(&search_path);
}
