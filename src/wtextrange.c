#include "wtextrange.h"

#include "memory.h"

void wrange_create(WRange** r)
{
  *r = (WRange* )mem_alloc(sizeof(WRange));

  (*r)->begin = 0;
  (*r)->end = 0;
}

void wrange_destroy(WRange** r)
{
  mem_free(*r);
  *r = 0;
}

WRange* wrange_from_string(const short* wstr)
{
  WRange* r;

  wrange_create(&r);

  r->begin = wstr;
  r->end = wstr;

  while( *(r->end) != 0 )
  {
    r->end++;
  }

  return r;
}

int wrange_length(WRange* r)
{
  return (int)(r->end - r->begin);
}

void wrange_copy(WRange* src, WRange* dst)
{
  dst->begin = src->begin;
  dst->end = src->end;
}

int wrange_equal(WRange* a, WRange* b)
{
  int len_a, i;

  len_a = wrange_length( a );

  if( len_a != wrange_length( b ) ) {
    return 0;
  }

  for( i = 0; i < len_a; ++i ) {
    if( a->begin[i] != b->begin[i] ) {
      return 0;
    }
  }

  return 1;
}

WString* wrange_make_string(WRange* r)
{
  WString* wstr;
  int len, i;

  len = wrange_length(r);

  wstring_create(&wstr);

  wstr->val = (short *)mem_alloc( (len+1) *2 );
  wstr->length = len;

  for( i = 0; i < len; ++i ) {
    wstr->val[i] = r->begin[i];
  }

  wstr->val[len] = 0;

  return wstr;
}
