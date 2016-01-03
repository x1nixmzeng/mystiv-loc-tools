// Myst IV *.bin converter
// Written by WRS (XeNTaX.com)

#include "stream.h"
#include "myst_bin.h"
#include "memory.h"

#include "string.h"
#include "wstring.h"

String* convert_wstring(WString* src)
{
  String* str;

  string_create(&str);

  str->length = src->length;
  if (str->length > 0) {
    int i;

    str->val = (char*)mem_alloc(str->length +1);
    
    for (i = 0; i < str->length; ++i) {
      str->val[i] = src->val[i] & 0x00FF;
    }

    str->val[str->length] = 0;
  }

  return str;
}

void write_translation(Translation* t)
{
  String* conv;

  conv = convert_wstring(t->trans);
  
  printf("<trans name=\"%s\">", string_get(t->key));
  printf("%s", string_get(conv));
  printf("</trans>\n");

  string_destroy(&conv);
}

int main(int argc, char** argv)
{
  printf("Myst IV *.bin converter\n");
  printf("WRS (XeNTaX.com)\n\n");

  if( argc != 2 ) {
    printf("Usage: bin t_<file>.bin\n");

    return 1;
  } else {
    FStream *fs;

    printf("Creating stream %s\n", argv[1]);

    stream_create(&fs);

    if( stream_open(fs, argv[1]) == 1 ) {
       printf("Failed to open file\n");
    } else {
      Locale *loc;
      int i, j;

      loc = bin_read(fs);
      
      printf("<?xml version=\"1.0\"?>\n");
      printf("<%s>\n", string_get(loc->name));

      if (loc->trans_count > 0) {
        printf("<group>\n");

        for (i = 0; i < loc->trans_count; ++i) {
          write_translation(loc->trans[i]);
        }

        printf("</group>\n");
      }

      for( i = 0; i < loc->group_count; ++i ) {
        printf("<group name=\"%s\">\n", string_get(loc->groups[i]->name) );

        for (j = 0; j < loc->groups[i]->trans_count; ++j) {
          write_translation(loc->groups[i]->trans[j]);
        }

        printf("</group>\n");
      }

      printf("</%s>\n\n", string_get(loc->name));

      locale_destroy(&loc);
    }

    stream_destroy(&fs);
  }

  mem_dbg();

  return 0;
}
