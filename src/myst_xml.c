#include "myst_xml.h"

#include "memory.h"

#include "xml.h"

#include "wstring.h"
#include "string.h"

#include "wtextrange.h"

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

const unsigned short subtitle_open_pre[] = L"<subtitle name=\"";
const unsigned short subtitle_open_post[] = L"\">\r\n";
const unsigned short subtitle_close[] = L"</subtitle>\r\n";

const unsigned short line_open_on[] = L"<line on=\"";
const unsigned short line_open_off[] = L"\" off=\"";
const unsigned short line_open_post[] = L"\">";
const unsigned short line_close[] = L"</line>\r\n";

#define WRITE(stream, xml) stream_write(stream, xml, sizeof(xml) -2);

String* convert_wstring(WString* src)
{
  String* str;

  string_create(&str);

  str->length = src->length;
  if (str->length > 0) {
    int i;

    str->val = (char*)mem_alloc(str->length + 1);

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

    str->val = (short*)mem_alloc((str->length + 1) * 2);

    for (i = 0; i < str->length; ++i) {
      str->val[i] = (unsigned short)src->val[i];
    }

    str->val[str->length] = 0;
  }

  return str;
}

FStream* create_xml_stream(const char* fn)
{
  FStream* res;
  unsigned short BOM;

  stream_create(&res);

  if (stream_make(res, fn) != 0) {
    printf("Failed to create xml stream\n");
    return res;
  }

  // write utf-16 byte order marker
  BOM = 0xfeff;
  stream_write(res, &BOM, 2);
  WRITE(res, xml_hdr);

  return res;
}

void write_translation(FStream* xml, Translation* t)
{
  WString* conv;

  WRITE(xml, trans_pre);
  conv = convert_string(t->key);
  stream_write_wstring(xml, conv);
  WRITE(xml, trans_post);
  stream_write_wstring(xml, t->trans);
  WRITE(xml, trans_end);

  wstring_destroy(&conv);
}

void write_subtitle(FStream* xml, Subtitle* s)
{
  WRITE(xml, line_open_on);
  stream_write_intstring(xml, s->time_on);
  WRITE(xml, line_open_off);
  stream_write_intstring(xml, s->time_off);
  WRITE(xml, line_open_post);
  stream_write_wstring(xml, s->line);
  WRITE(xml, line_close);
}

void myst_write_xml(FStream* out_xml, Locale* loc)
{
  int i, j;
  WString* conv;
  
  if (out_xml->handle != 0) {

    WRITE(out_xml, node_open);
    conv = convert_string(loc->name);
    stream_write_wstring(out_xml, conv);
    wstring_destroy(&conv);
    WRITE(out_xml, node_close);
    
    if (loc->subt_count > 0) {
      WRITE(out_xml, subtitle_open_pre);
      conv = convert_string(loc->source);
      stream_write_wstring(out_xml, conv);
      wstring_destroy(&conv);
      WRITE(out_xml, subtitle_open_post);
      
      for (i = 0; i < loc->subt_count; ++i) {
        write_subtitle(out_xml, loc->subt[i]);
      }

      WRITE(out_xml, subtitle_close);
    }

    if (loc->trans_count > 0) {
      WRITE(out_xml, group_open);

      for (i = 0; i < loc->trans_count; ++i) {
        write_translation(out_xml, loc->trans[i]);
      }

      WRITE(out_xml, group_close);
    }

    for (i = 0; i < loc->group_count; ++i) {
      WRITE(out_xml, group_open_pre);
      conv = convert_string(loc->groups[i]->name);
      stream_write_wstring(out_xml, conv);
      wstring_destroy(&conv);
      WRITE(out_xml, group_open_post);

      for (j = 0; j < loc->groups[i]->trans_count; ++j) {
        write_translation(out_xml, loc->groups[i]->trans[j]);
      }

      WRITE(out_xml, group_close);
    }

    WRITE(out_xml, node_closing);
    conv = convert_string(loc->name);
    stream_write_wstring(out_xml, conv);
    wstring_destroy(&conv);
    WRITE(out_xml, node_close);
  }
}

XmlHint xml_skip_declaration(Xml *xml)
{
  int loop;

  loop = 1;
  while (loop != 0) {
    switch (xml_parse(xml)) {
    case kXmlHintEnded:
    case kXmlHintEndDeclaration:
      loop = 0;
      break;
    default:
      break;
    }
  }

  return xml->context;
}

XmlHint myst_xml_read_node(Xml *xml, WRange** ele_name, int max_attribs, WRange** attrib_val)
{
  XmlHint hint;

  hint = xml->context;

  if (xml->context != kXmlHintStartElementOpen) {
    hint = xml_parse(xml);
  }

  if (hint == kXmlHintStartElementOpen) {
    int loop;
    int attribs;

    if (ele_name != 0) {
      (*ele_name)->begin = xml->cursor;
      (*ele_name)->end = (*ele_name)->begin;
    }

    loop = 1;
    attribs = 0;

    while (loop) {
      switch (hint) {
      case kXmlHintEnded:
        loop = 0;
        break;
      case kXmlHintEndElementOpen:
        // bug: element name should be returned separately from elementopen/close
        if (ele_name != 0){
          if ((*ele_name)->end == (*ele_name)->begin) {
            (*ele_name)->end = xml->cursor - 1;
          }
        }
        loop = 0;
        break;
      case kXmlHintStartAttributeName:
        if (ele_name != 0 ){
          if ((*ele_name)->end == (*ele_name)->begin) {
            (*ele_name)->end = xml->cursor - 1;
          }
        }
        break;
      case kXmlHintStartAttributeValue:
        if (attrib_val != 0 && attribs < max_attribs){
          attrib_val[attribs]->begin = xml->cursor;
          attrib_val[attribs]->end = attrib_val[attribs]->begin;
        }
        break;
      case kXmlHintEndAttributeValue:
        if (attrib_val != 0 && attribs < max_attribs) {
          attrib_val[attribs]->end = xml->cursor;
          ++attribs;
        }
        break;
      }

      hint = xml_parse(xml);
    }
  }

  return hint;
}

XmlHint myst_xml_close_node(Xml *xml, WRange** ele_name)
{
  XmlHint hint;

  hint = xml->context;

  if (xml->context != kXmlHintStartElementClose) {
    hint = xml_parse(xml);
  }

  if (hint == kXmlHintStartElementClose) {
    int loop;

    if (ele_name != 0) {
      (*ele_name)->begin = xml->cursor;
      (*ele_name)->end = (*ele_name)->begin;
    }

    loop = 1;

    while (loop) {
      switch (hint) {
      case kXmlHintEnded:
        loop = 0;
        break;
      case kXmlHintEndElementClose:
        if (ele_name != 0) {
          (*ele_name)->end = xml->cursor - 1;
        }
        loop = 0;
        break;
      }

      hint = xml_parse(xml);
    }
  }

  return hint;
}


XmlHint myst_xml_read_inner_text(Xml *xml, WRange** text, WRange** ele_name)
{
  XmlHint hint;

  hint = xml->context;

  if (xml->context != kXmlHintStartInnerText) {
    hint = xml_parse(xml);
  }

  if (hint == kXmlHintStartInnerText) {
    int loop;

    (*text)->begin = xml->cursor;
    (*text)->end = (*text)->begin;

    loop = 1;

    while (loop) {
      switch (hint) {
      case kXmlHintEnded:
        loop = 0;
        break;
      case kXmlHintEndInnerText:
        (*text)->end = xml->cursor;
        break;
      case kXmlHintStartElementClose:
        if (ele_name != 0) {
          (*ele_name)->begin = xml->cursor;
          (*ele_name)->end = (*ele_name)->begin;
        }
        break;
      case kXmlHintEndElementClose:
        if (ele_name != 0) {
          (*ele_name)->end = xml->cursor - 1;
        }
        loop = 0;
        break;
      }

      hint = xml_parse(xml);
    }
  }

  return hint;
}

Locale* loc_from_xml(WRange* src)
{
  Locale* loc;
  Xml* xml;
  XmlHint hint;

  locale_create(&loc);
  xml_create(&xml);

  wrange_copy(src, xml->range);
  xml->cursor = xml->range->begin;

  hint = xml_skip_declaration(xml);

  if (hint != kXmlHintEnded) {
    WRange* root_name;
    wrange_create(&root_name);

    hint = myst_xml_read_node(xml, &root_name, 0, 0);

    if (hint != kXmlHintEnded) {
      WString *root;

      root = wrange_make_string(root_name);

      // Set final name
      loc->name = convert_wstring(root);

      {
        WRange *ele_name;
        WRange *ele_attr_val[2];
        WRange *ele_inner;

        WRange *group_range;
        WRange *trans_range;
        WRange *subtitle_range;
        WRange *line_range;

        Group *last_group;

        int loop;

        group_range = wrange_from_string(L"group");
        trans_range = wrange_from_string(L"trans");
        subtitle_range = wrange_from_string(L"subtitle");
        line_range = wrange_from_string(L"line");

        wrange_create(&ele_name);
        wrange_create(&ele_attr_val[0]);
        wrange_create(&ele_attr_val[1]);

        wrange_create(&ele_inner);

        last_group = 0;

        loop = 1;

        while (loop != 0)
        {
          hint = myst_xml_read_node(xml, &ele_name, 2, ele_attr_val);

          if (hint == kXmlHintEnded)
          {
            loop = 0;
          }
          else if (wrange_equal(ele_name, group_range) == 1)
          {
            WString * val;

            val = wrange_make_string(ele_attr_val[0]);
            group_create(&last_group);
            
            last_group->name = convert_wstring(val);
            locale_insert_group(loc, last_group);

            wstring_destroy(&val);
          }
          else if (wrange_equal(ele_name, trans_range) == 1)
          {
            Translation* tr;
            WString* name;

            hint = myst_xml_read_inner_text(xml, &ele_inner, &ele_name);

            name = wrange_make_string(ele_attr_val[0]);

            translation_create(&tr);

            tr->key = convert_wstring(name);
            tr->trans = wrange_make_string(ele_inner);

            if (last_group == 0) {
              locale_insert_translation(loc, tr);
            } else {
              group_insert_translation(last_group, tr);
            }

            wstring_destroy(&name);
          }
          else if (wrange_equal(ele_name, subtitle_range) == 1)
          {
            WString* name;

            name = wrange_make_string(ele_attr_val[0]);
            loc->source = convert_wstring(name);

            wstring_destroy(&name);
          }
          else if (wrange_equal(ele_name, line_range) == 1)
          {
            Subtitle* sub;
            int len;
            subtitle_create(&sub);

            hint = myst_xml_read_inner_text(xml, &ele_inner, 0);

            // for compatability with old <line id="x"> format
            // new format is <line on="0" off="1">
            len = wrange_length(ele_attr_val[0]);

            if (len == 16) {
              subtitle_set_id_legacy(sub, ele_attr_val[0]);
            } else {
              subtitle_set_id(sub, ele_attr_val[0], ele_attr_val[1]);
            }

            sub->line = wrange_make_string(ele_inner);

            locale_insert_subtitle(loc, sub);
          }
          else
          {
            loop = 0;
          }

          if (loop != 0)
          {
            while (hint == kXmlHintStartElementClose)
            {
              hint = myst_xml_close_node(xml, &ele_name);

              // close group
              if (wrange_equal(ele_name, group_range) == 1)
              {
                last_group = 0;
              }

              // end parsing (hit a node name the same as the root one)
              else if (wrange_equal(ele_name, root_name) == 1)
              {
                loop = 0;
              }
            }
          }
        }

        wrange_destroy(&ele_inner);

        wrange_destroy(&line_range);
        wrange_destroy(&subtitle_range);
        wrange_destroy(&group_range);
        wrange_destroy(&trans_range);

        wrange_destroy(&ele_attr_val[1]);
        wrange_destroy(&ele_attr_val[0]);
        wrange_destroy(&ele_name);
      }

      wstring_destroy(&root);
    }

    wrange_destroy(&root_name);
  }

  xml_destroy(&xml);

  return loc;
}

Locale* myst_read_xml(FStream *fs)
{
  Locale* loc;
  void* buffer;

  stream_seek(fs, 0);
  buffer = mem_alloc(fs->length);

  stream_read(fs, buffer, fs->length);

  // check utf-16 export
  if (*(unsigned short*)buffer == 0xFEFF) {
    WRange *range;

    wrange_create(&range);

    range->begin = (const short*)((const char *)buffer + 2);
    range->end = (const short*)((const char *)buffer + fs->length);

    loc = loc_from_xml(range);

    wrange_destroy(&range);
  }

  mem_free(buffer);

  return loc;
}
