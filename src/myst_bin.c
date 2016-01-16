#include "myst_bin.h"

#include "memory.h"
#include "wtextrange.h"

void translation_create(Translation** t)
{
  *t = (Translation* )mem_alloc(sizeof(Translation));

  (*t)->key = 0;
  (*t)->trans = 0;
}

void translation_destroy(Translation** t)
{
  if( (*t)->key != 0 ) {
    string_destroy( &(*t)->key );
  }

  if( (*t)->trans != 0 ) {
    wstring_destroy( &(*t)->trans );
  }

  mem_free(*t);
  *t = 0;
}

void group_insert_translation(Group* g, Translation *t)
{
  if (g->trans == 0) {
    g->trans = (Translation**)mem_alloc(sizeof(Translation*));

    g->trans[0] = t;
    g->trans_count = 1;
  }
  else {
    g->trans = (Translation**)mem_realloc(g->trans, sizeof(Translation*)* (g->trans_count + 1));

    g->trans[g->trans_count] = t;
    g->trans_count++;
  }
}

void group_create(Group** g)
{
  *g = (Group* )mem_alloc(sizeof(Group));

  (*g)->name = 0;
  (*g)->trans = 0;
  (*g)->trans_count = 0;
}

void group_destroy(Group** g)
{
  if( (*g)->name != 0 ) {
    string_destroy(&(*g)->name);
  }

  if( (*g)->trans != 0 ) {
    int i;

    for( i = 0; i < (*g)->trans_count; ++i ) {
      translation_destroy( &(*g)->trans[i] );
    }

    mem_free((*g)->trans);
  }

  mem_free(*g);
  *g = 0;
}

void locale_insert_translation(Locale* l, Translation *t)
{
  if (l->trans == 0) {
    l->trans = (Translation**)mem_alloc(sizeof(Translation*));

    l->trans[0] = t;
    l->trans_count = 1;
  }
  else {
    l->trans = (Translation**)mem_realloc(l->trans, sizeof(Translation*)* (l->trans_count + 1));

    l->trans[l->trans_count] = t;
    l->trans_count++;
  }
}

void locale_insert_group(Locale *l, Group *g)
{
  if (l->groups == 0) {
    l->groups = (Group**)mem_alloc(sizeof(Group*));

    l->groups[0] = g;
    l->group_count = 1;
  } else {
    l->groups = (Group**)mem_realloc(l->groups, sizeof(Group*)* (l->group_count + 1));

    l->groups[l->group_count] = g;
    l->group_count++;
  }
}

void locale_create(Locale** l)
{
  *l = (Locale*)mem_alloc(sizeof(Locale));

  (*l)->name = 0;
  (*l)->trans = 0;
  (*l)->trans_count = 0;
  (*l)->groups = 0;
  (*l)->group_count = 0;
}

void locale_destroy(Locale** l)
{
  if( (*l)->name != 0 ) {
    string_destroy(&(*l)->name);
  }

  if( (*l)->trans != 0 ) {
    int i;

    for( i = 0; i < (*l)->trans_count; ++i ) {
      translation_destroy( &(*l)->trans[i] );
    }

    mem_free((*l)->trans);
  }

  if( (*l)->groups != 0 ) {
    int i;

    for( i = 0; i < (*l)->group_count; ++i ) {
      group_destroy( &(*l)->groups[i] );
    }

    mem_free((*l)->groups);
  }

  mem_free(*l);
  *l = 0;
}

int bin_check_header(FStream *fs)
{
  char magic[9];
  int len, i;
  const char expected[] = "ubi/b0-l";

  len = sizeof(expected);

  stream_read(fs, &magic, len -1);
  magic[8] = 0;

  for( i = 0; i < len; ++i ) {
    if( magic[i] != expected[i] ) {
      printf("Bad magic for locale file\n");
      return 1;
    }
  }

  return 0;
}

void scramble_str(String* str)
{
  int i;
  char c;

  for( i=0; i<str->length; ++i )
  {
    c = str->val[i];

    // swap each bit position in pairs (abcd ==> badc)

    c = ((c & 0x40) << 1)
      + ((c & 0x80) >> 1)
      + ((c & 0x10) << 1)
      + ((c & 0x20) >> 1)
      + ((c & 0x4) << 1)
      + ((c & 0x8) >> 1)
      + ((c & 0x1) << 1)
      + ((c & 0x2) >> 1);

    str->val[i] = c;
  }
}

Translation* bin_read_translation(FStream *fs)
{
  Translation* trans;

  translation_create(&trans);

  trans->key = stream_read_cstring(fs);
  trans->trans = stream_read_cwstring(fs);

  return trans;
}

Group* bin_read_group(FStream *fs)
{
  Group* group;
  int i, unknown;

  group_create(&group);

  group->name = stream_read_cstring(fs);

  group->trans_count = stream_read_int(fs);
  unknown = stream_read_int(fs);

  if( group->trans_count > 0 ) {
    group->trans = (Translation** )mem_alloc(sizeof(Translation* ) * group->trans_count);

    for( i = 0; i < group->trans_count; ++i ) {
      group->trans[i] = bin_read_translation(fs);
    }
  }

  return group;
}

void bin_read_main(FStream *fs, Locale* loc)
{
  int i, unknown;

  unknown = stream_read_int(fs); // 1

  loc->trans_count = stream_read_int(fs);
  loc->group_count = stream_read_int(fs);

  if( loc->trans_count > 0 )
  {
    loc->trans = (Translation** )mem_alloc(sizeof(Translation *) * loc->trans_count);

    for( i = 0; i < loc->trans_count; ++i )
    {
      loc->trans[i] = bin_read_translation(fs);
    }
  }

  if( loc->group_count > 0 )
  {
    loc->groups = (Group** )mem_alloc(sizeof(Group *) * loc->group_count);

    for( i = 0; i < loc->group_count; ++i )
    {
      loc->groups[i] = bin_read_group(fs);
    }
  }
}

Locale* myst_read_bin(FStream *fs)
{
  Locale* loc;
  int bin_type;
  int unknown_0;

  locale_create(&loc);

  if( bin_check_header(fs) != 0 ) {
    return loc;
  }
 
  bin_type = stream_read_int(fs);

  // Types
  // Subtitle == 36 (subtitle_?.bin)
  // Text = 37 (t_?.bin)

  if( bin_type != 37) {
    printf("This BIN file is not a text resource\n");
    return loc;
  }

  unknown_0 = stream_read_int(fs);

  loc->name = stream_read_cstring(fs);
  scramble_str(loc->name);

  bin_read_main(fs, loc);

  return loc;
}

