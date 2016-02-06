#include "myst_bin.h"

#include "memory.h"
#include "wtextrange.h"

const char bin_magic[] = "ubi/b0-l";

enum BinType
{
  BIN_TYPE_COMMAND_BLOCK = 6,
  BIN_TYPE_STATE = 7,
  BIN_TYPE_GRAPHIC = 12,
  BIN_TYPE_INTERPOLATOR = 14,
  BIN_TYPE_LIGHT_SET = 20,
  BIN_TYPE_SUBTITLE = 36,
  BIN_TYPE_TEXT = 37,
  BIN_TYPE_TEXTURE = 39,
  BIN_TYPE_VIDEO = 41,
  BIN_TYPE_MENU = 46,
};

const char* bintype_to_string(int bin_type)
{
  switch (bin_type)
  {
  case BIN_TYPE_COMMAND_BLOCK:
    return "command block";
  case BIN_TYPE_STATE:
    return "state";
  case BIN_TYPE_GRAPHIC:
    return "graphic";
  case BIN_TYPE_INTERPOLATOR:
    return "interpolator";
  case BIN_TYPE_LIGHT_SET:
    return "light set";
  case BIN_TYPE_SUBTITLE:
    return "subtitle";
  case BIN_TYPE_TEXT:
    return "text";
  case BIN_TYPE_TEXTURE:
    return "texture";
  case BIN_TYPE_VIDEO:
    return "video";
  case BIN_TYPE_MENU:
    return "menu";
  default:
    return "<unknown type>";
  }
}

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

void subtitle_create(Subtitle** s)
{
  *s = (Subtitle*)mem_alloc(sizeof(Subtitle));

  (*s)->time_on = 0;
  (*s)->time_off = 0;
  (*s)->line = 0;
}

void subtitle_destroy(Subtitle** s)
{
  if ((*s)->line != 0) {
    wstring_destroy(&(*s)->line);
  }

  mem_free(*s);
  *s = 0;
}

void from_hex(int* result, short chr)
{
  switch (chr) {
    case '1': *result |= 1; break;
    case '2': *result |= 2; break;
    case '3': *result |= 3; break;
    case '4': *result |= 4; break;
    case '5': *result |= 5; break;
    case '6': *result |= 6; break;
    case '7': *result |= 7; break;
    case '8': *result |= 8; break;
    case '9': *result |= 9; break;
    case 'a': *result |= 10; break;
    case 'b': *result |= 11; break;
    case 'c': *result |= 12; break;
    case 'd': *result |= 13; break;
    case 'e': *result |= 14; break;
    case 'f': *result |= 15; break;
  }
}

__int64 convert_id_legacy(WRange* r)
{
  __int64 result;
  int len, i;

  result = 0;

  len = wrange_length(r);

  if (len == 16) {
    int tmp;
    for (i = 0; i < len; i+=2) {
      tmp = 0;
      from_hex(&tmp, r->begin[i]);
      tmp <<= 4;
      from_hex(&tmp, r->begin[i + 1]);
      
      result |= (__int64)(tmp) << (i*4);
    }
  }

  return result;
}

int convert_id(WRange* r)
{
  int result;
  int len, i, tmp;

  result = 0;

  len = wrange_length(r);

  for (i = 0; i < len; ++i)
  {
    tmp = 0;
    from_hex(&tmp, r->begin[i]);

    result *= 10;
    result += tmp;
  }

  return result;
}

void subtitle_set_id(Subtitle* s, WRange* r_on, WRange* r_off)
{
  s->time_on = convert_id(r_on);
  s->time_off = convert_id(r_off);
}

void subtitle_set_id_legacy(Subtitle* s, WRange* r)
{
  __int64 tmp;
  
  tmp = convert_id_legacy(r);

  s->time_on = (int)(tmp & 0xFFFFFFFF);
  s->time_off = (int)(tmp >> 32);
}

void locale_insert_subtitle(Locale* l, Subtitle *s)
{
  if (l->subt == 0) {
    l->subt = (Subtitle**)mem_alloc(sizeof(Subtitle*));

    l->subt[0] = s;
    l->subt_count = 1;
  }
  else {
    l->subt = (Subtitle**)mem_realloc(l->subt, sizeof(Subtitle*)* (l->subt_count + 1));

    l->subt[l->subt_count] = s;
    l->subt_count++;
  }
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
  (*l)->source = 0;
  (*l)->trans = 0;
  (*l)->trans_count = 0;
  (*l)->groups = 0;
  (*l)->group_count = 0;
  (*l)->subt = 0;
  (*l)->subt_count = 0;
}

void locale_destroy(Locale** l)
{
  if( (*l)->name != 0 ) {
    string_destroy(&(*l)->name);
  }

  if ((*l)->source != 0) {
    string_destroy(&(*l)->source);
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

  if ((*l)->subt != 0) {
    int i;

    for (i = 0; i < (*l)->subt_count; ++i) {
      subtitle_destroy(&(*l)->subt[i]);
    }

    mem_free((*l)->subt);
  }

  mem_free(*l);
  *l = 0;
}

int locale_valid(Locale* l)
{
  int result;

  result = 0;

  if (l != 0) {
    if (l->name != 0) {
      result |= 1;
    }

    if (l->source != 0) {
      result |= 1;
    }

    if (l->group_count > 0 || l->trans_count > 0) {
      result |= 1;
    }

    if (l->subt_count > 0) {
      result |= 1;
    }
  }

  return result;
}

int bin_guess_type(Locale* l)
{
  if (l->source != 0) {
    return BIN_TYPE_SUBTITLE;
  }

  return BIN_TYPE_TEXT;
}

int bin_check_header(FStream *fs)
{
  char magic[9];
  int len, i;

  len = sizeof(bin_magic);

  stream_read(fs, &magic, len -1);
  magic[8] = 0;

  for( i = 0; i < len; ++i ) {
    if (magic[i] != bin_magic[i]) {
      printf("Unknown BIN file; magic string does not match\n");
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
  unknown = stream_read_int(fs); // 0 ?

  if( group->trans_count > 0 ) {
    group->trans = (Translation** )mem_alloc(sizeof(Translation* ) * group->trans_count);

    for( i = 0; i < group->trans_count; ++i ) {
      group->trans[i] = bin_read_translation(fs);
    }
  }

  return group;
}

Subtitle* bin_read_subtitle(FStream *fs)
{
  Subtitle* subtitle;

  subtitle_create(&subtitle);

  subtitle->time_on = stream_read_int(fs);
  subtitle->time_off = stream_read_int(fs);
  subtitle->line = stream_read_cwstring(fs);

  return subtitle;
}

void bin_read_text(FStream *fs, Locale* loc)
{
  int i, unknown_1;

  unknown_1 = stream_read_int(fs); // 1

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

void bin_read_subtitle_text(FStream *fs, Locale* loc)
{
  int unknown_1, i;

  unknown_1 = stream_read_int(fs); // 1

  loc->source = stream_read_cstring(fs);

  loc->subt_count = stream_read_int(fs);

  if (loc->subt_count > 0)
  {
    loc->subt = (Subtitle** )mem_alloc(sizeof(Subtitle* )* loc->subt_count);

    for (i = 0; i < loc->subt_count; ++i)
    {
      loc->subt[i] = bin_read_subtitle(fs);
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
  unknown_0 = stream_read_int(fs); // 0

  loc->name = stream_read_cstring(fs);
  scramble_str(loc->name);

  switch (bin_type)
  {
  case BIN_TYPE_SUBTITLE:
    bin_read_subtitle_text(fs, loc);
    break;
  case BIN_TYPE_TEXT:
    bin_read_text(fs, loc);
    break;
  default:
    printf("ERROR: Unknown BIN file; the \"%s\" type is not supported\n", bintype_to_string(bin_type));
  }
  
  if (fs->length != stream_pos(fs)) {
    printf("ERROR: Did not finished reading file\n");
  }

  return loc;
}

void bin_write_header(FStream* fs, Locale* l)
{
  int bin_type, unknown_1;
  
  bin_type = bin_guess_type(l);

  unknown_1 = 1; // prob. the count!

  stream_write(fs, bin_magic, sizeof(bin_magic)-1);
  stream_write(fs, &bin_type, sizeof(int));
  stream_write(fs, &unknown_1, sizeof(int));
}

void bin_write_name(FStream* fs, Locale *l)
{
  String* name;

  name = string_from_cstring(l->name->val);
  scramble_str(name);

  stream_write(fs, &name->length, sizeof(name->length));
  stream_write_string(fs, name);

  string_destroy(&name);
}

void bin_write_translation(FStream *fs, Translation *tr)
{
  // key
  stream_write(fs, &tr->key->length, sizeof(int));
  stream_write_string(fs, tr->key);
  
  // translation
  stream_write(fs, &tr->trans->length, sizeof(int));
  stream_write_wstring(fs, tr->trans);
}

void bin_write_group(FStream *fs, Group *g)
{
  int i, unknown;

  // name
  stream_write(fs, &g->name->length, sizeof(int));
  stream_write_string(fs, g->name);

  // translation count
  stream_write(fs, &g->trans_count, sizeof(int));

  unknown = 0;
  stream_write(fs, &unknown, sizeof(int));

  for (i = 0; i < g->trans_count; ++i)
  {
    bin_write_translation(fs, g->trans[i]);
  }
}

void bin_write_subtitle(FStream* fs, Subtitle* s)
{
  stream_write(fs, &s->time_on, sizeof(s->time_on));
  stream_write(fs, &s->time_off, sizeof(s->time_off));
  stream_write(fs, &s->line->length, sizeof(s->line->length));
  stream_write_wstring(fs, s->line);
}

void bin_write_text(FStream *fs, Locale *l)
{
  int i, unknown_1;

  unknown_1 = 1;

  stream_write(fs, &unknown_1, sizeof(int));
  
  stream_write(fs, &l->trans_count, sizeof(int));
  stream_write(fs, &l->group_count, sizeof(int));
  
  if (l->trans_count > 0)
  {
    for (i = 0; i < l->trans_count; ++i)
    {
      bin_write_translation(fs, l->trans[i]);
    }
  }

  if (l->group_count > 0)
  {
    for (i = 0; i < l->group_count; ++i)
    {
      bin_write_group(fs, l->groups[i]);
    }
  }
}

void bin_write_subtitle_text(FStream *fs, Locale *l)
{
  int i, unknown_1;

  unknown_1 = 1;

  stream_write(fs, &unknown_1, sizeof(unknown_1));

  stream_write(fs, &l->source->length, sizeof(l->source->length));
  stream_write_string(fs, l->source);

  stream_write(fs, &l->subt_count, sizeof(l->subt_count));

  if (l->subt_count > 0)
  {
    for (i = 0; i < l->subt_count; ++i)
    {
      bin_write_subtitle(fs, l->subt[i]);
    }
  }
}

void myst_write_bin(FStream* out_bin, Locale* loc)
{
  bin_write_header(out_bin, loc);
  bin_write_name(out_bin, loc);

  switch (bin_guess_type(loc))
  {
  case BIN_TYPE_TEXT:
    bin_write_text(out_bin, loc);
    break;
  case BIN_TYPE_SUBTITLE:
    bin_write_subtitle_text(out_bin, loc);
    break;
  }
}

