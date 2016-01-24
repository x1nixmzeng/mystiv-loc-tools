#ifndef MYST_M4B_HEADER_H
#define MYST_M4B_HEADER_H

#include "string.h"
#include "stream.h"
#include "myst_fs.h"

MystDir *m4b_read(FStream *fs);
MystDir *m4b_create(String *path);

void m4b_write_header(FStream* out_bin, MystDir* root);
void m4b_write_data(FStream* out_bin, MystDir* root, String* base_path);

#endif // MYST_M4B_HEADER_H
