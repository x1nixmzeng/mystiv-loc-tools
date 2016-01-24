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

void m4v_insert_fsfile(MystDir* root, MystFile *f)
{
  if (root->file_count == 0)
  {
    root->files = (MystFile**)mem_alloc(sizeof(MystFile*));

    root->files[0] = f;
    root->file_count++;
  } else {
    root->files = (MystFile**)mem_realloc(root->files, sizeof(MystFile*)* (root->file_count + 1));

    root->files[root->file_count] = f;
    root->file_count++;
  }
}

void m4v_insert_fsdir(MystDir* root, MystDir *d)
{
  if (root->dir_count == 0)
  {
    root->dirs = (MystDir**)mem_alloc(sizeof(MystDir*));

    root->dirs[0] = d;
    root->dir_count++;
  }
  else {
    root->dirs = (MystDir**)mem_realloc(root->dirs, sizeof(MystDir*)* (root->dir_count + 1));

    root->dirs[root->dir_count] = d;
    root->dir_count++;
  }
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

void m4v_iterate_fs(MystDir* root, String* root_path, MystFsCallback func, void* userdata)
{
  String* current_path;
  int i;

  current_path = string_concat_cstring(3, string_get(root_path), string_get(root->name), "/");

  if (root->dir_count > 0) {
    for (i = 0; i < root->dir_count; ++i) {
      m4v_iterate_fs(root->dirs[i], current_path, func, userdata);
    }
  }

  for (i = 0; i < root->file_count; ++i) {
    func(current_path, root->files[i], userdata);
  }

  string_destroy(&current_path);
}
