#include "myst_bin.h"
#include "memory.h"
#include "xml.h"
#include "wtextrange.h"

void translation_create(Translation** t)
{
  *t = (Translation* )mem_alloc(sizeof(Translation));

  (*t)->key = 0;
  (*t)->trans = 0;
}

void translation_destroy(Translation** t)
{
  if( (*t)->key != 0 ) {
    string_destroy( &(*t)->key );
  }

  if( (*t)->trans != 0 ) {
    wstring_destroy( &(*t)->trans );
  }

  mem_free(*t);
  *t = 0;
}

void group_create(Group** g)
{
  *g = (Group* )mem_alloc(sizeof(Group));

  (*g)->name = 0;
  (*g)->trans = 0;
  (*g)->trans_count = 0;
}

void group_destroy(Group** g)
{
  if( (*g)->name != 0 ) {
    string_destroy(&(*g)->name);
  }

  if( (*g)->trans != 0 ) {
    int i;

    for( i = 0; i < (*g)->trans_count; ++i ) {
      translation_destroy( &(*g)->trans[i] );
    }

    mem_free((*g)->trans);
  }

  mem_free(*g);
  *g = 0;
}

void locale_create(Locale** l)
{
  *l = (Locale*)mem_alloc(sizeof(Locale));

  (*l)->name = 0;
  (*l)->trans = 0;
  (*l)->trans_count = 0;
  (*l)->groups = 0;
  (*l)->group_count = 0;
}

void locale_destroy(Locale** l)
{
  if( (*l)->name != 0 ) {
    string_destroy(&(*l)->name);
  }

  if( (*l)->trans != 0 ) {
    int i;

    for( i = 0; i < (*l)->trans_count; ++i ) {
      translation_destroy( &(*l)->trans[i] );
    }

    mem_free((*l)->trans);
  }

  if( (*l)->groups != 0 ) {
    int i;

    for( i = 0; i < (*l)->group_count; ++i ) {
      group_destroy( &(*l)->groups[i] );
    }

    mem_free((*l)->groups);
  }

  mem_free(*l);
  *l = 0;
}

int bin_check_header(FStream *fs)
{
  char magic[9];
  int len, i;
  const char expected[] = "ubi/b0-l";

  len = sizeof(expected);

  stream_read(fs, &magic, len -1);
  magic[8] = 0;

  for( i = 0; i < len; ++i ) {
    if( magic[i] != expected[i] ) {
      printf("Bad magic for locale file\n");
      return 1;
    }
  }

  return 0;
}

void scramble_str(String* str)
{
  int i;
  char c;

  for( i=0; i<str->length; ++i )
  {
    c = str->val[i];

    // swap each bit position in pairs (abcd ==> badc)

    c = ((c & 0x40) << 1)
      + ((c & 0x80) >> 1)
      + ((c & 0x10) << 1)
      + ((c & 0x20) >> 1)
      + ((c & 0x4) << 1)
      + ((c & 0x8) >> 1)
      + ((c & 0x1) << 1)
      + ((c & 0x2) >> 1);

    str->val[i] = c;
  }
}

Translation* bin_read_translation(FStream *fs)
{
  Translation* trans;

  translation_create(&trans);

  trans->key = stream_read_cstring(fs);
  trans->trans = stream_read_cwstring(fs);

  return trans;
}

Group* bin_read_group(FStream *fs)
{
  Group* group;
  int i, unknown;

  group_create(&group);

  group->name = stream_read_cstring(fs);

  group->trans_count = stream_read_int(fs);
  unknown = stream_read_int(fs);

  if( group->trans_count > 0 ) {
    group->trans = (Translation** )mem_alloc(sizeof(Translation* ) * group->trans_count);

    for( i = 0; i < group->trans_count; ++i ) {
      group->trans[i] = bin_read_translation(fs);
    }
  }

  return group;
}

void bin_read_main(FStream *fs, Locale* loc)
{
  int i, unknown;

  unknown = stream_read_int(fs); // 1

  loc->trans_count = stream_read_int(fs);
  loc->group_count = stream_read_int(fs);

  if( loc->trans_count > 0 )
  {
    loc->trans = (Translation** )mem_alloc(sizeof(Translation *) * loc->trans_count);

    for( i = 0; i < loc->trans_count; ++i )
    {
      loc->trans[i] = bin_read_translation(fs);
    }
  }

  if( loc->group_count > 0 )
  {
    loc->groups = (Group** )mem_alloc(sizeof(Group *) * loc->group_count);

    for( i = 0; i < loc->group_count; ++i )
    {
      loc->groups[i] = bin_read_group(fs);
    }
  }
}

Locale* bin_read(FStream *fs)
{
  Locale* loc;
  int bin_type;
  int unknown_0;

  locale_create(&loc);

  if( bin_check_header(fs) != 0 ) {
    return loc;
  }
 
  bin_type = stream_read_int(fs);

  // Types
  // Subtitle == 36 (subtitle_?.bin)
  // Text = 37 (t_?.bin)

  if( bin_type != 37) {
    printf("This BIN file is not a text resource\n");
    return loc;
  }

  unknown_0 = stream_read_int(fs);

  loc->name = stream_read_cstring(fs);
  scramble_str(loc->name);

  bin_read_main(fs, loc);

  return loc;
}

WRange* xml_get_element_open(Xml* xml)
{
  WRange* range;
  wrange_create(&range);



  return range;
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

XmlHint myst_xml_read_node(Xml *xml, WRange** ele_name, WRange** attrib_val)
{
  XmlHint hint;

  hint = xml->context;

  if (xml->context != kXmlHintStartElementOpen) {
    hint = xml_parse(xml);
  }
  
  if (hint == kXmlHintStartElementOpen) {
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
        if (ele_name != 0){
          (*ele_name)->end = xml->cursor - 1;
        }
        break;
      case kXmlHintStartAttributeValue:
        if (attrib_val != 0){
          (*attrib_val)->begin = xml->cursor;
          (*attrib_val)->end = (*attrib_val)->begin;
        }
        break;
      case kXmlHintEndAttributeValue:
        if (attrib_val != 0) {
          (*attrib_val)->end = xml->cursor;
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
          (*ele_name)->end = xml->cursor -1;
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
  Xml* xml;
  XmlHint hint;
  
  xml_create(&xml);

  wrange_copy(src, xml->range);
  xml->cursor = xml->range->begin;

  hint = xml_skip_declaration(xml);

  if (hint != kXmlHintEnded) {
    WRange* root_name;
    wrange_create(&root_name);

    hint = myst_xml_read_node(xml, &root_name, 0);

    if (hint != kXmlHintEnded) {
      WString *root;

      root = wrange_make_string(root_name);

      printf("Got root %ls\n", wstring_get(root));

      {
        WRange *ele_name;
        WRange *ele_attr_val;
        WRange *ele_inner;
        
        WRange *group_range;
        WRange *trans_range;

        int loop; 

        group_range = wrange_from_string(L"group");
        trans_range = wrange_from_string(L"trans");
        
        wrange_create(&ele_name);
        wrange_create(&ele_attr_val);

        wrange_create(&ele_inner);
        
        loop = 1;

        while (loop != 0)
        {
          hint = myst_xml_read_node(xml, &ele_name, &ele_attr_val);

          if (hint == kXmlHintEnded)
          {
            loop = 0;
          }
          else if (wrange_equal(ele_name, group_range) == 1)
          {
            WString * val;
            
            val = wrange_make_string(ele_attr_val);
            printf("Parsing group %ls\n", wstring_get(val));

            wstring_destroy(&val);
          }
          else if (wrange_equal(ele_name, trans_range) == 1)
          {
            WString* name;
            WString* val;

            hint = myst_xml_read_inner_text(xml, &ele_inner, &ele_name);

            name = wrange_make_string(ele_attr_val);
            val = wrange_make_string(ele_inner);

            printf("%ls == \"%ls\"\n", wstring_get(name), wstring_get(val));

            wstring_destroy(&name);
            wstring_destroy(&val);
          }
          else
          {
            loop = 0;
          }

          if (loop != 0)
          {
            while(hint == kXmlHintStartElementClose)
            {
              hint = myst_xml_close_node(xml, &ele_name);

              if (wrange_equal(ele_name, group_range) == 1)
              {
                printf("Closing group\n\n");
              }
              else if (wrange_equal(ele_name, root_name) == 1)
              {
                printf("Hit closing root node\n");
                loop = 0;
              }
            }
          }
        }

        wrange_destroy(&ele_inner);

        wrange_destroy(&group_range);
        wrange_destroy(&trans_range);

        wrange_destroy(&ele_attr_val);
        wrange_destroy(&ele_name);
      }

      wstring_destroy(&root);
    }

    wrange_destroy(&root_name);
  }

  xml_destroy(&xml);

  return 0;
}

Locale* xml_read(FStream *fs)
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

    range->begin = (const short*)((const char *)(buffer + 2));
    range->end = (const short*)((const char *)(range->begin + fs->length));

    loc = loc_from_xml(range);

    wrange_destroy(&range);
  }

  mem_free(buffer);
  
  return loc;
}
