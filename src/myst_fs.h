#ifndef MYST_FS_HEADER_H
#define MYST_FS_HEADER_H

#include "string.h"

typedef struct MystFileNode
{
  String* name;
  int size;
  int offset;
} MystFile;

typedef struct MystDirNode;

typedef struct MystDirNode
{
  String* name;

  struct MystFileNode** files;
  int file_count;
  struct MystDirNode** dirs;
  int dir_count;  
} MystDir;

void m4b_create_fsfile(MystFile** m);
void m4b_create_fsdir(MystDir** m);

void m4b_destroy_fsfile(MystFile** m);
void m4b_destroy_fsdir(MystDir** m);

typedef void(*MystFsCallback)(String* path, MystFile* file, void* userdata);

void m4v_iterate_fs(MystDir* root, String* root_path, MystFsCallback func, void* userdata);

#endif // MYST_FS_HEADER_H
