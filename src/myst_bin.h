#ifndef MYST_BIN_HEADER_H
#define MYST_BIN_HEADER_H

#include "stream.h"
#include "string.h"
#include "wstring.h"

typedef struct TranslationNode
{
  String* key;
  WString* trans;
} Translation;

typedef struct TranslationGroupNode
{
  String* name;
  Translation **trans;
  int trans_count;
} Group;

typedef struct SubtitleNode
{
  __int64 id;
  WString* line;
} Subtitle;

typedef struct LocaleNode
{
  String* name;
  String* source;
  Translation **trans;
  int trans_count;
  Group **groups;
  int group_count;
  Subtitle **subt;
  int subt_count;
} Locale;

void translation_create(Translation** t);
void translation_destroy(Translation** t);

void group_insert_translation(Group* g, Translation *t);

void group_create(Group** g);
void group_destroy(Group** g);

void subtitle_create(Subtitle** s);
void subtitle_destroy(Subtitle** s);

void locale_insert_subtitle(Locale* l, Subtitle *s);
void locale_insert_translation(Locale* l, Translation *t);
void locale_insert_group(Locale* l, Group *g);

void locale_create(Locale** l);
void locale_destroy(Locale** l);

int locale_valid(Locale* l);

Locale* myst_read_bin(FStream *fs);
void myst_write_bin(FStream* out_bin, Locale* loc);

#endif // MYST_BIN_HEADER_H
