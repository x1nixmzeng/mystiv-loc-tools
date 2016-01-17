#ifndef WSTRING_HEADER_H
#define WSTRING_HEADER_H

typedef struct WString
{
 short *val;
 int length;
} WString;

void wstring_create(WString** wstr);

void wstring_destroy(WString** wstr);

const short *wstring_get(WString* wstr);

int cwstring_length(const short* cwstr);

WString *wstring_from_cwstring(const short* cwstr);

#endif // WSTRING_HEADER_H
