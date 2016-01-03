#include "wstring.h"

#include "memory.h"

void wstring_create(WString** wstr)
{
  *wstr = (WString *)mem_alloc(sizeof(WString));
  (*wstr)->val = 0;
  (*wstr)->length = 0;
}

void wstring_destroy(WString** wstr)
{
  if ((*wstr)->val != 0) {
    mem_free((*wstr)->val);
  }

  mem_free(*wstr);
  *wstr = 0;
}

const short *wstring_get(WString* wstr)
{
  if (wstr->length > 0) {
    return wstr->val;
  }

  return L"";
}
