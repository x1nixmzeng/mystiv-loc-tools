#include "myst_m4b.h"

#include "memory.h"
#include "string.h"
#include "myst_fs.h"

#include "platform.h"

const char m4b_magic[] = "UBI_BF_SIG";

MystFile* m4b_read_file(FStream *fs)
{
  MystFile *node;

  m4b_create_fsfile(&node);

  node->name = stream_read_cstring(fs);
  node->size = stream_read_int(fs);
  node->offset = stream_read_int(fs);

  return node;
}

MystFile* m4b_create_file(String* str, int size)
{
  MystFile *node;

  m4b_create_fsfile(&node);

  node->name = str;
  node->size = size;
  node->offset = -1;

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
  else // does this follow on from above?
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

  str = stream_read_cstring(fs);
  len = sizeof(m4b_magic);

  if( len != str->length ) {
    printf("Invalid signature length\n");
    string_destroy(&str);
    return 1;
  }

  for( i = 0; i < len; ++i ) {
    if (m4b_magic[i] != str->val[i]) {
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
  MystDir *root;
  int unknown_0, unknown_1;
  int i, num_children;

  if( m4b_validate_header(fs) != 0 ) {
    return 0;
  }

  m4b_create_fsdir(&root);

  string_create(&root->name);

  unknown_1 = stream_read_int(fs);
  unknown_0 = stream_read_int(fs);

  num_children = (int)(unsigned char)stream_read_char(fs);
  root->dir_count = num_children;

  if (root->dir_count > 0)
  {
    root->dirs = (MystDir**)mem_alloc(sizeof(MystDir*)* num_children);

    for (i = 0; i < num_children; ++i) {
      root->dirs[i] = m4b_read_dir(fs);
    }
  }

  num_children = stream_read_int(fs);
  root->file_count = num_children;

  if (root->file_count > 0)
  {
    root->files = (MystFile**)mem_alloc(sizeof(MystFile*)* num_children);

    for (i = 0; i < num_children; ++i) {
      root->files[i] = m4b_read_file(fs);
    }
  }

  return root;
}

int m4b_get_total_size(MystDir* root)
{
  int size, i;

  size = 0;

  for (i = 0; i < root->dir_count; ++i) {
    size += m4b_get_total_size(root->dirs[i]);
  }

  for (i = 0; i < root->file_count; ++i) {
    size += root->files[i]->size;
  }

  return size;
}

typedef struct CallbackUserdata
{
  MystDir* root;
  String* path;
} CbUserdata;

void userdata_create(CbUserdata** ud)
{
  (*ud) = (CbUserdata*)mem_alloc(sizeof(CbUserdata));

  (*ud)->path = 0;
  (*ud)->root = 0;
}

void userdata_destroy(CbUserdata** ud)
{
  // all members are references. do not destroy those.

  mem_free(*ud);
  *ud = 0;
}

void dir_callback(const char* name, void* ud);
void fs_callback(const char* name, int size, void* ud);

void dir_callback(const char* name, void* ud)
{
  CbUserdata* data;
  CbUserdata* ud_new;
  MystDir* dir;
  String *new_path;

  data = (CbUserdata*)ud;

  m4b_create_fsdir(&dir);
  
  dir->name = string_from_cstring(name);

  m4v_insert_fsdir(data->root, dir);

  userdata_create(&ud_new);

  new_path = string_concat_cstring(3, string_get(data->path), "\\", string_get(dir->name));

  ud_new->path = new_path;
  ud_new->root = dir;

  platform_run_dirscan(new_path, fs_callback, dir_callback, ud_new);

  string_destroy(&new_path);
  userdata_destroy(&ud_new);
}

void fs_callback(const char* name, int size, void* ud)
{
  CbUserdata* data;
  String* file_name;
  MystFile* file;

  data = (CbUserdata*)ud;

  file_name = string_from_cstring(name);

  file = m4b_create_file(file_name, size);
  m4v_insert_fsfile(data->root, file);
}

MystDir *m4b_create(String *path)
{
  MystDir* root;
  CbUserdata* ud;

  m4b_create_fsdir(&root);
  string_create(&root->name);

  userdata_create(&ud);

  ud->path = path;
  ud->root = root;
  
  platform_run_dirscan(path, fs_callback, dir_callback, ud);

  userdata_destroy(&ud);

  return root;
}

void myst_stream_write_string(FStream* out_bin, String* str)
{
  int tmp;

  tmp = str->length + 1;
  stream_write(out_bin, &tmp, sizeof(tmp));
  stream_write(out_bin, string_get(str), tmp);
}

void m4b_write_file(FStream* out_bin, MystFile* file, int* next_file_offset)
{
  myst_stream_write_string(out_bin, file->name);
  stream_write(out_bin, &file->size, sizeof(file->size));

  stream_write(out_bin, next_file_offset, sizeof(int));
  *next_file_offset += file->size;
}

void m4b_write_dir(FStream* out_bin, MystDir* dir, int* next_file_offset)
{
  int i;
  char dirs;

  myst_stream_write_string(out_bin, dir->name);
  
  // 1 byte
  dirs = (char)dir->dir_count & 0xFF;
  stream_write(out_bin, &dirs, sizeof(dirs));

  for (i = 0; i < dir->dir_count; ++i)
  {
    m4b_write_dir(out_bin, dir->dirs[i], next_file_offset);
  }
  
  // 4 bytes
  stream_write(out_bin, &dir->file_count, sizeof(dir->file_count));

  for (i = 0; i < dir->file_count; ++i)
  {
    m4b_write_file(out_bin, dir->files[i], next_file_offset);
  }
}

int m4v_get_file_size(MystFile* file)
{
  int len;

  len = 0;
  len += sizeof(file->name->length);
  len += file->name->length + 1;
  len += sizeof(file->size);
  len += sizeof(file->offset);

  return len;
}

int m4b_get_dir_size(MystDir* dir)
{
  int len, i;

  len = 0;

  len += sizeof(dir->name->length);
  len += dir->name->length + 1;

  len += sizeof(char);

  for (i = 0; i < dir->dir_count; ++i)
  {
    len += m4b_get_dir_size(dir->dirs[i]);
  }

  len += sizeof(int);
  
  for (i = 0; i < dir->file_count; ++i)
  {
    len += m4v_get_file_size(dir->files[i]);
  }

  return len;
}

int m4b_get_fssize(MystDir* root)
{
  int len, i;

  len = 0;
  len += sizeof(int);
  len += sizeof(m4b_magic);

  len += sizeof(int);
  len += sizeof(int);
  
  len += sizeof(char);

  for (i = 0; i < root->dir_count; ++i)
  {
    len += m4b_get_dir_size(root->dirs[i]);
  }

  len += sizeof(int);

  for (i = 0; i < root->file_count; ++i)
  {
    len += m4v_get_file_size(root->files[i]);
  }

  return len;
}

void m4b_write(FStream* out_bin, MystDir* root)
{
  int tmp;
  int unknown_0, unknown_1;
  char num_children;
  int file_offset;

  file_offset = m4b_get_fssize(root);

  tmp = sizeof(m4b_magic);

  stream_write(out_bin, &tmp, sizeof(int));
  stream_write(out_bin, m4b_magic, tmp);

  unknown_1 = 1;
  unknown_0 = 0;
  
  stream_write(out_bin, &unknown_1, sizeof(int));
  stream_write(out_bin, &unknown_0, sizeof(int));

  num_children = (char)root->dir_count & 0xFF;

  stream_write(out_bin, &num_children, sizeof(char));
 
  for (tmp = 0; tmp < root->dir_count; ++tmp)
  {
    m4b_write_dir(out_bin, root->dirs[tmp], &file_offset);
  }

  stream_write(out_bin, &root->file_count, sizeof(int));

  for (tmp = 0; tmp < root->file_count; ++tmp)
  {
    m4b_write_file(out_bin, root->files[tmp], &file_offset);
  }
}
