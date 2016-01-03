#include "string.h"

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
