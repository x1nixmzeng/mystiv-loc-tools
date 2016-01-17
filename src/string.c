#include "string.h"

#include <stdarg.h>
#include "memory.h"

void string_create(String** str)
{
  *str = (String *)mem_alloc(sizeof(String));
  (*str)->val = 0;
  (*str)->length = 0;
}

void string_destroy(String** str)
{
  if( (*str)->val != 0 ) {
    mem_free((*str)->val);
  }

  mem_free(*str);
  *str = 0;
}

const char *string_get(String* str)
{
  if (str->length > 0) {
    return str->val;
  }

  return "";
}

int cstring_length(const char* cstr)
{
  const char* pStr;

  pStr = cstr;

  while (*pStr) {
    ++pStr;
  }

  return pStr - cstr;
}

String *string_from_cstring(const char* cstr)
{
  String* str;
  int i;

  string_create(&str);
  
  str->length = cstring_length(cstr);

  if (str->length > 0) {
    str->val = (char *)mem_alloc(str->length + 1);

    for (i = 0; i < str->length; ++i){
      str->val[i] = cstr[i];
    }

    str->val[str->length] = 0;
  }

  return str;
}

String *string_concat_cstring(int num, ...)
{
  va_list valist;
  String *str;
  int i, length;
  const char *arg;
  char *buf;
  
  va_start(valist, num);
  length = 0;

  for (i = 0; i < num; ++i) {
    arg = va_arg(valist, const char *);

    length += cstring_length(arg);
  }

  va_end(valist);

  string_create(&str);

  str->length = length;
  str->val = (char*)mem_alloc(str->length + 1);
  str->val[str->length] = 0;

  buf = str->val;

  va_start(valist, num);

  for (i = 0; i < num; ++i) {
    arg = va_arg(valist, const char *);
    length = cstring_length(arg);

    while (*arg) {
      *buf++ = *arg++;
    }
  }

  va_end(valist);

  return str;
}
