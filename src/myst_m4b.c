#include "myst_m4b.h"

#include "memory.h"
#include "string.h"
#include "myst_fs.h"

MystFile* m4b_read_file(FStream *fs)
{
  MystFile *node;

  m4b_create_fsfile(&node);

  node->name = stream_read_cstring(fs);
  node->size = stream_read_int(fs);
  node->offset = stream_read_int(fs);

  return node;
}

MystDir* m4b_read_dir(FStream *fs)
{
  MystDir* node;
  int num;

  m4b_create_fsdir(&node);

  node->name = stream_read_cstring(fs);

  num = (int)(unsigned char)stream_read_char(fs);

  if( num > 0 )
  {
    int i;

    node->dir_count = num;
    node->dirs = (MystDir** )mem_alloc(sizeof(MystDir*) * num);

    for( i = 0; i < num; ++i )
    {
      node->dirs[i] = m4b_read_dir(fs);
    }

    i = stream_read_int(fs);
  }
  else
  {
    int i;

    num = stream_read_int(fs);

    node->file_count = num;
    node->files = (MystFile** )mem_alloc(sizeof(MystFile*) * num);

    for( i = 0; i < num; ++i )
    {
      node->files[i] = m4b_read_file(fs);
    }
  } 

  return node;
}

int m4b_validate_header(FStream* fs)
{
  String* str;
  int i, len;
  const char magic[] = "UBI_BF_SIG";

  str = stream_read_cstring(fs);
  len = sizeof(magic);

  if( len != str->length ) {
    printf("Invalid signature length\n");
    string_destroy(&str);
    return 1;
  }

  for( i = 0; i < len; ++i ) {
    if( magic[i] != str->val[i] ) {
      printf("Invalid signature magic\n");
      string_destroy(&str);
      return 1;
    }
  }

  string_destroy(&str);
  return 0;
}

MystDir *m4b_read(FStream *fs)
{
  MystDir *myst;
  int unknown_0, unknown_1;
  int i, num_children;

  if( m4b_validate_header(fs) != 0 ) {
    return 0;
  }

  m4b_create_fsdir(&myst);

  string_create(&myst->name);

  unknown_0 = stream_read_int(fs);
  unknown_1 = stream_read_int(fs);

  num_children = (int)(unsigned char)stream_read_char(fs);

  myst->dir_count = num_children;
  myst->dirs = (MystDir** )mem_alloc(sizeof(MystDir*) * num_children);

  for( i = 0; i < num_children; ++i ) {
    myst->dirs[i] = m4b_read_dir(fs);
  }

  // another section is here (language indexes?)

  printf("Pos %u\n", stream_pos(fs));
  printf("Length %u\n", fs->length);

  return myst;
}
