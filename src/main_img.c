// Myst IV texture extractor
// Written by WRS (XeNTaX.com)

#include "myst_bin.h"

#include "stream.h"
#include "memory.h"

void extract_bin_texture(FStream* fs, const char* out_fn);

int main(int argc, char** argv)
{
  int show_usage;

  printf("Myst IV texture extractor\n");
  printf("WRS (XeNTaX.com)\n\n");

  show_usage = 1;

  if (argc > 1) {
    switch (argv[1][0])
    {
    case 'e':
    {
              if (argc == 4) {
                show_usage = 0;
                FStream *fs_in;

                stream_create(&fs_in);

                if (stream_open(fs_in, argv[2]) == 1) {
                  printf("Failed to open source file\n");
                }
                else {
                  extract_bin_texture(fs_in, argv[3]);
                }

                stream_destroy(&fs_in);
              }

              break;
    }
    }
  }

  if (show_usage != 0) {
    printf("Usage:\n");
    printf(" myst_extract_img e <file>.bin <result>\n");
    printf("\n");
  }

  mem_dbg();

  return 0;
}

void extract_bin_texture(FStream* fs, const char* out_fn)
{
  Texture *myst_img;

  myst_img = myst_read_bin_texture(fs);
  
  if (myst_img != 0){
    FStream* out;

    stream_create(&out);
    stream_make(out, out_fn);

    if (out->handle != 0) {
      myst_write_bin_texture(out, myst_img);
    }
    
    stream_destroy(&out);
  }

  texture_destroy(&myst_img);
}
