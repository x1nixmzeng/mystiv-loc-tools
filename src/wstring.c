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

int cwstring_length(const short* cwstr)
{
  const short* pStr;

  pStr = cwstr;

  while (*pStr) {
    ++pStr;
  }

  return pStr - cwstr;
}

WString *wstring_from_cwstring(const short* cwstr)
{
  WString* wstr;
  int i;

  wstring_create(&wstr);

  wstr->length = cwstring_length(cwstr);

  if (wstr->length > 0) {
    wstr->val = (short *)mem_alloc((wstr->length + 1) * sizeof(short));

    for (i = 0; i < wstr->length; ++i){
      wstr->val[i] = cwstr[i];
    }

    wstr->val[wstr->length] = 0;
  }

  return wstr;
}
