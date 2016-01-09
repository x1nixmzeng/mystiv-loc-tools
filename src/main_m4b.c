// Myst IV *.m4b packer
// Written by WRS (XeNTaX.com)
// Note: Does not work with the demo

#include "stream.h"
#include "myst_bin.h"
#include "myst_m4b.h"
#include "myst_fs.h"

#include "memory.h"
#include "platform.h"

void list_files(const char* fn);
void extract_files(const char* fn, const char* dst_folder);
void pack_files(const char* folder, const char* fn);

int main(int argc, char** argv)
{
  int show_usage;

  printf("Myst IV *.m4b packer\n");
  printf("  Note: Does not work with the demo\n");
  printf("WRS (XeNTaX.com)\n\n");

  show_usage = 1;

  if( argc > 1 ) {
    switch (argv[1][0])
    {
    case 'l':
    {
              if (argc == 3) {
                show_usage = 0;
                list_files(argv[2]);
              }

              break;
    }
    case 'e':
    {
              if (argc == 4) {
                show_usage = 0;
                extract_files(argv[2], argv[3]);
              }

              break;
    }

    case 'p':
    {
              if (argc == 4) {
                show_usage = 0;
                pack_files(argv[3], argv[2]);
              }

              break;
    }
    default:
    {
             printf("Unknown option '%c'\n", argv[1][0]);
             break; 
    }
    }
  }

  if (show_usage != 0) {
    printf("Usage:\n");
    printf(" List files    m4b l file.m4b\n");
    printf(" Extract files m4b e file.m4b folder\n");
    printf(" Pack folder   m4b p folder file.m4b\n");
    printf("\n");
  }
  
  mem_dbg();

  return 0;
}

void cb_list_file(String* path, MystFile* file, void* userdata)
{
  printf("%s%s %u %u\n",
    string_get(path),
    string_get(file->name),
    file->size,
    file->offset);
}

void list_files(const char* fn)
{
  FStream *fs;

  printf("Open %s\n", fn);
  stream_create(&fs);

  if (stream_open(fs, fn) == 1) {
    printf("Failed to open file\n");
  }
  else {
    MystDir *root;

    root = m4b_read(fs);

    if (root != 0) {
      String* root_dir;

      root_dir = string_from_cstring("");

      m4v_iterate_fs(root, root_dir, cb_list_file, 0);

      string_destroy(&root_dir);
      m4b_destroy_fsdir(&root);
    }
    else {
      printf("Failed to read the file system\n");
    }
  }

  stream_destroy(&fs);
}

void cb_extract_file(String* path, MystFile* file, void* userdata)
{
  FStream *m4b, *fs;
  int i;
  unsigned char* buf;

  m4b = (FStream*)userdata;

  buf = (unsigned char*)mem_alloc(1024);
  
  sprintf(buf, "%s%s", string_get(path), string_get(file->name));

  platform_make_full_path(path);

  stream_create(&fs);
  if (stream_make(fs, buf) != 0)
  {
    printf("Failed to extract file %s\n", buf);

    mem_free(buf);
    stream_destroy(&fs);
    return;
  }
  
  printf("Extracting %s\n", buf);

  stream_seek(m4b, file->offset);

  i = 0;
  while (i < file->size)
  {
    int block;

    block = file->size - i;

    if (block > 1024) {
      block = 1024;
    }

    stream_read(m4b, buf, block);
    stream_write(fs, buf, block);

    i += block;
  }

  mem_free(buf);

  stream_destroy(&fs);
}

void extract_files(const char* fn, const char* dst_folder)
{
  FStream *fs;

  printf("Open %s\n", fn);
  stream_create(&fs);

  if (stream_open(fs, fn) == 1) {
    printf("Failed to open file\n");
  }
  else {
    MystDir *root;

    root = m4b_read(fs);

    if (root != 0) {
      String* root_dir;

      root_dir = string_from_cstring(dst_folder);

      m4v_iterate_fs(root, root_dir, cb_extract_file, fs);

      string_destroy(&root_dir);
      m4b_destroy_fsdir(&root);
    }
    else {
      printf("Failed to read the file system\n");
    }
  }

  stream_destroy(&fs);
}

void pack_files(const char* folder, const char* fn)
{

}
