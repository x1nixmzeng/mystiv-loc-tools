#include "myst_fs.h"

#include "memory.h"

void m4b_create_fsfile(MystFile** m)
{
  *m = (MystFile* )mem_alloc(sizeof(MystFile));

  (*m)->name = 0;
  (*m)->size = 0;
  (*m)->offset = 0;
}

void m4b_create_fsdir(MystDir** m)
{
  *m = (MystDir* )mem_alloc(sizeof(MystDir));

  (*m)->name = 0;
  (*m)->files = 0;
  (*m)->file_count = 0;
  (*m)->dirs = 0;
  (*m)->dir_count = 0;
}

void m4b_destroy_fsfile(MystFile** m)
{
  if( (*m)->name != 0 ) {
    string_destroy( &(*m)->name );
  }

  mem_free(*m);
  *m = 0;
}

void m4b_destroy_fsdir(MystDir** m)
{
  int i;

  if( (*m)->dirs != 0 ) {
    for( i = 0; i < (*m)->dir_count; ++i ) {
      m4b_destroy_fsdir( &(*m)->dirs[i] );
    }

    mem_free( (*m)->dirs );
  }

  if( (*m)->files != 0 ) {
    for( i = 0; i < (*m)->file_count; ++i ) {
      m4b_destroy_fsfile( &(*m)->files[i] );
    }

    mem_free( (*m)->files );
  }

  if( (*m)->name != 0 ) {
    string_destroy( &(*m)->name );
  }

  mem_free(*m);
  *m = 0;
}

void m4b_dump_dir(MystDir* root, String* path)
{
  int i;

  if (root->dir_count > 0) {
    String* next_path;

    string_create(&next_path);

    next_path->length = path->length + root->name->length + 1;
    next_path->val = (char*)mem_alloc(next_path->length + 1);

    sprintf(next_path->val, "%s%s/", string_get(path), string_get(root->name));

    for (i = 0; i < root->dir_count; ++i) {
      m4b_dump_dir(root->dirs[i], next_path);
    }

    string_destroy(&next_path);
  }

  for( i = 0; i < root->file_count; ++i ) {
    printf("%s%s %u %u\n",
      string_get(path),
      string_get(root->files[i]->name),
      root->files[i]->size,
      root->files[i]->offset);
  }
}
