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

typedef struct LocaleNode
{
  String* name;
  Translation **trans;
  int trans_count;
  Group **groups;
  int group_count;
} Locale;

void translation_create(Translation** t);
void translation_destroy(Translation** t);

void group_create(Group** g);
void group_destroy(Group** g);

void locale_create(Locale** l);
void locale_destroy(Locale** l);

Locale* bin_read(FStream *fs);

#endif // MYST_BIN_HEADER_H
