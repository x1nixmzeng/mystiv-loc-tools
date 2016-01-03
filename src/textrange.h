#ifndef TEXTRANGE_HEADER_H
#define TEXTRANGE_HEADER_H

#include "string.h"

typedef struct TextRange
{
  const char *begin;
  const char *end;
} Range;

void range_create(Range** r);
void range_destroy(Range** r);

Range* range_from_string(const char* str);

int range_length(Range* r);

void range_copy(Range* src, Range* dst);
int range_equal(Range* a, Range* b);

String* range_make_string(Range* r);

#endif // TEXTRANGE_HEADER_H
