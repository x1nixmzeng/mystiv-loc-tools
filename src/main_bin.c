// Myst IV *.bin converter
// Written by WRS (XeNTaX.com)

#include "stream.h"
#include "myst_bin.h"
#include "memory.h"

#include "string.h"
#include "wstring.h"

const unsigned short xml_hdr[] = L"<?xml version=\"1.0\"?>\r\n";

const unsigned short trans_pre[] = L"<trans name=\"";
const unsigned short trans_post[] = L"\">";
const unsigned short trans_end[] = L"</trans>\r\n";

const unsigned short node_open[] = L"<";
const unsigned short node_closing[] = L"</";
const unsigned short node_close[] = L">\r\n";
const unsigned short group_open[] = L"<group>\r\n";
const unsigned short group_open_pre[] = L"<group name=\"";
const unsigned short group_open_post[] = L"\">\r\n";
const unsigned short group_close[] = L"</group>\r\n";

String* convert_wstring(WString* src)
{
  String* str;

  string_create(&str);

  str->length = src->length;
  if (str->length > 0) {
    int i;

    str->val = (char*)mem_alloc(str->length +1);
    
    for (i = 0; i < str->length; ++i) {
      str->val[i] = src->val[i] & 0x00FF;
    }

    str->val[str->length] = 0;
  }

  return str;
}

WString* convert_string(String* src)
{
  WString* str;

  wstring_create(&str);

  str->length = src->length;
  if (str->length > 0) {
    int i;

    str->val = (short* )mem_alloc((str->length + 1) * 2);

    for (i = 0; i < str->length; ++i) {
      str->val[i] = (unsigned short)src->val[i];
    }

    str->val[str->length] = 0;
  }

  return str;
}

void write_translation(FStream* xml, Translation* t)
{
  WString* conv;
  
  stream_write(xml, trans_pre, sizeof(trans_pre) -2);
  conv = convert_string(t->key);
  stream_write_wstring(xml, conv);
  stream_write(xml, trans_post, sizeof(trans_post) -2);
  stream_write_wstring(xml, t->trans);
  stream_write(xml, trans_end, sizeof(trans_end) -2);

  wstring_destroy(&conv);
}

FStream* create_xml_stream(const char* fn)
{
  FStream* res;

  stream_create(&res);

  if (stream_make(res, fn) != 0) {
    printf("Failed to create xml stream\n");
    return res;
  }

  {
    unsigned short BOM;

    BOM = 0xfeff;

    stream_write(res, &BOM, 2);
    stream_write(res, xml_hdr, sizeof(xml_hdr) -2);
  }

  return res;
}

int main(int argc, char** argv)
{
  printf("Myst IV *.bin converter\n");
  printf("WRS (XeNTaX.com)\n\n");

  if( argc != 3 ) {
    printf("Usage: bin t_<file>.bin <result>.xml\n");

    return 1;
  } else {
    FStream *fs;

    printf("Creating stream %s\n", argv[1]);

    stream_create(&fs);

    if( stream_open(fs, argv[1]) == 1 ) {
       printf("Failed to open file\n");
    } else {
      Locale *loc;

      loc = bin_read(fs);
      
      if (loc->name != 0) {
        FStream* out_xml;
        int i, j;
        WString* conv;

#define WRITE(xml) stream_write(out_xml, xml, sizeof(xml) -2);

        out_xml = create_xml_stream(argv[2]);

        if (out_xml->handle != 0) {

          WRITE(node_open);
          conv = convert_string(loc->name);
          stream_write_wstring(out_xml, conv);
          wstring_destroy(&conv);
          WRITE(node_close);

          if (loc->trans_count > 0) {
            WRITE(group_open);

            for (i = 0; i < loc->trans_count; ++i) {
              write_translation(out_xml, loc->trans[i]);
            }

            WRITE(group_close);
          }

          for (i = 0; i < loc->group_count; ++i) {
            WRITE(group_open_pre);
            conv = convert_string(loc->groups[i]->name);
            stream_write_wstring(out_xml, conv);
            wstring_destroy(&conv);
            WRITE(group_open_post);

            for (j = 0; j < loc->groups[i]->trans_count; ++j) {
              write_translation(out_xml, loc->groups[i]->trans[j]);
            }

            WRITE(group_close);
          }

          WRITE(node_closing);
          conv = convert_string(loc->name);
          stream_write_wstring(out_xml, conv);
          wstring_destroy(&conv);
          WRITE(node_close);
        }

        stream_destroy(&out_xml);
      }

      locale_destroy(&loc);
    }

    stream_destroy(&fs);
  }

  mem_dbg();

  return 0;
}