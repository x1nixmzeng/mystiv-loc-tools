#ifndef WTEXTRANGE_HEADER_H
#define WTEXTRANGE_HEADER_H

#include "wstring.h"

typedef struct WTextRange
{
  const short *begin;
  const short *end;
} WRange;

void wrange_create(WRange** r);
void wrange_destroy(WRange** r);

WRange* wrange_from_string(const short* str);

int wrange_length(WRange* r);

void wrange_copy(WRange* src, WRange* dst);
int wrange_equal(WRange* a, WRange* b);

WString* wrange_make_string(WRange* r);

#endif // WTEXTRANGE_HEADER_H
