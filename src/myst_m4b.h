#ifndef MYST_M4B_HEADER_H
#define MYST_M4B_HEADER_H

#include "stream.h"
#include "myst_fs.h"

MystDir *m4b_read(FStream *fs);
MystDir *m4b_create(String *path);

void m4b_write(FStream* out_bin, MystDir* root);

#endif // MYST_M4B_HEADER_H
