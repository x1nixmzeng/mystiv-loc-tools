#include "stream.h"
#include "myst_bin.h"
#include "memory.h"

int main(int argc, char** argv)
{
  printf("Myst IV *.bin locale converter\n");
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
      int i;

      loc = bin_read(fs);

      printf("Internal name is %s\n", loc->name->val);

      printf("Translations: %u\n", loc->trans_count);

      for( i = 0; i < loc->trans_count; ++i ) {
        printf("%s\n", loc->trans[i]->key->val);
      }

      printf("Groups: %u\n", loc->group_count);

      for( i = 0; i < loc->group_count; ++i ) {
        printf("Group: %s (%u translations)\n", loc->groups[i]->name->val, loc->groups[i]->trans_count);
      }

      locale_destroy(&loc);
    }

    stream_destroy(&fs);
  }

  mem_dbg();

  return 0;
}
