#ifndef WSTRING_HEADER_H
#define WSTRING_HEADER_H

typedef struct WString
{
 short *val;
 int length;
} WString;

void wstring_create(WString** str);

void wstring_destroy(WString** str);

#endif // WSTRING_HEADER_H
