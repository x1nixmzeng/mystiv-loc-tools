#include "textrange.h"

#include "memory.h"

void range_create(Range** r)
{
  *r = (Range* )mem_alloc(sizeof(Range));

  (*r)->begin = 0;
  (*r)->end = 0;
}

void range_destroy(Range** r)
{
  mem_free(*r);
  *r = 0;
}

Range* range_from_string(const char* str)
{
  Range* r;

  range_create(&r);

  r->begin = str;
  r->end = str;

  while( *(r->end) != 0 )
  {
    r->end++;
  }

  return r;
}

int range_length(Range* r)
{
  return (int)(r->end - r->begin);
}

void range_copy(Range* src, Range* dst)
{
  dst->begin = src->begin;
  dst->end = src->end;
}

int range_equal(Range* a, Range* b)
{
  int len_a, i;

  len_a = range_length( a );

  if( len_a != range_length( b ) ) {
    return 0;
  }

  for( i = 0; i < len_a; ++i ) {
    if( a->begin[i] != b->begin[i] ) {
      return 0;
    }
  }

  return 1;
}

String* range_make_string(Range* r)
{
  String* str;
  int len, i;

  len = range_length(r);

  string_create(&str);

  str->val = (char *)mem_alloc(len +1);
  str->length = len;

  for( i = 0; i < len; ++i ) {
    str->val[i] = r->begin[i];
  }

  str->val[len] = 0;

  return str;
}
