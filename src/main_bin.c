// Myst IV *.bin converter
// Written by WRS (XeNTaX.com)

#include "myst_bin.h"
#include "myst_xml.h"

#include "stream.h"
#include "memory.h"

void convert_bin(FStream* fs, const char* out_fn);
void convert_xml(FStream* fs, const char* out_fn);

int main(int argc, char** argv)
{
  int show_usage;

  printf("Myst IV *.bin converter\n");
  printf("WRS (XeNTaX.com)\n\n");

  show_usage = 1;

  if (argc > 1) {
    switch (argv[1][0])
    {
    case 't':
    {
              if (argc == 4) {
                show_usage = 0;
                FStream *fs_in;

                stream_create(&fs_in);

                if (stream_open(fs_in, argv[2]) == 1) {
                  printf("Failed to open source file\n");
                }
                else {
                  convert_xml(fs_in, argv[3]);
                }

                stream_destroy(&fs_in);
              }

              break;
    }
    case 'f':
    {
              if (argc == 4) {
                show_usage = 0;
                FStream *fs_in;

                stream_create(&fs_in);

                if (stream_open(fs_in, argv[2]) == 1) {
                  printf("Failed to open source file\n");
                }
                else {
                  convert_bin(fs_in, argv[3]);
                }

                stream_destroy(&fs_in);
              }

              break;
    }
    }
  }

  if (show_usage != 0) {
    printf("Usage:\n");
    printf(" myst_loc f <file>.bin <result>.xml\n");
    printf(" myst_loc t <file>.xml <result>.bin\n");
    printf("\n");
  }

  mem_dbg();

  return 0;
}

void convert_bin(FStream* fs, const char* out_fn)
{
  Locale *myst_loc;

  myst_loc = myst_read_bin(fs);
  
  if (myst_loc != 0){
    FStream* out;

    out = create_xml_stream(out_fn);

    if (out->handle != 0) {
      myst_write_xml(out, myst_loc);
    }
    
    stream_destroy(&out);
    locale_destroy(&myst_loc);
  }
}

void convert_xml(FStream* fs, const char* out_fn)
{
  Locale *myst_loc;

  myst_loc = myst_read_xml(fs);

  if ( myst_loc != 0) {
    FStream* out;

    stream_create(&out);
    stream_make(out, out_fn);

    if (out->handle != 0) {
      myst_write_bin(out, myst_loc);
    }

    stream_destroy(&out);
    locale_destroy(&myst_loc);
  }
}
