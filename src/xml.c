#include "xml.h"
#include "memory.h"

void xmldoc_create(XmlDoc** xd)
{
  *xd = (XmlDoc* )mem_alloc(sizeof(XmlDoc));

  (*xd)->data = 0;
  (*xd)->cursor = 0;

  range_create( &(*xd)->node );
}

void xmldoc_destroy(XmlDoc** xd)
{
  if( (*xd)->data != 0 ) {
    range_destroy(&(*xd)->data);
  }

  range_destroy(&(*xd)->node);

  mem_free(*xd);
  *xd = 0;
}

XmlDoc* xmldoc_init(Range* r)
{
  XmlDoc* xml;

  xmldoc_create(&xml);

  range_create(&(xml->data));
  range_copy(r, xml->data);

  range_copy(r, xml->node);
  xml->cursor = xml->data->begin;

  return xml;
}

const char* xmldoc_eat_whitespace(XmlDoc* xd)
{
  const char* str;

  str = xd->cursor;

  while( str < xd->data->end )
  {
    switch( *str )
    {
      case ' ':
      case '\t':
      case '\n':
        str++;
        break;
      case 0:
        return str;
      default:
        return str;
    }
  }

  return str;
}

void eat_text(Range* r)
{
  const char* str;

  str = r->begin;

  while( str < r->end )
  {
    if( ( *str >= 'a' && *str <= 'z' )
     || ( *str >= 'A' && *str <= 'Z' )
     || ( *str >= '0' && *str <= '9' )
     || ( *str == '_' ) ) {
        str++;
    } else {
      break;
    }
  }

  r->begin = str;
}

Range* xmldoc_getnode(Range* r)
{
  Range* node;
  const char* p;

  range_create(&node);
  range_copy(r, node);

  p = node->begin;

  while( p < node->end ) {
    if( *p == '<' ) {
      p++;

//      if( *p == '/' ) {
//        p++;
//      }

      node->begin = p;

      while( p < node->end ) {
        if( *p == '>' ) {
          node->end = p;
          return node;
        } else {
          p++;
        }
      }

    } else {
      p++;
    }
  }

  range_destroy(&node);
  return 0;
}

int xmldoc_next(XmlDoc* xd)
{
  const char* pos;
  Range* tag;

  pos = xmldoc_eat_whitespace(xd);

   

  tag = xmldoc_getnode(xd->node);

  if( tag != 0 ) {
    String* str;
    int end;

    if( *tag->begin == '/' ) {
      printf("This is the END node. Exiting\n");
      range_destroy(&tag);
      return 0;
    }

    str = range_make_string(tag);
    printf("Node name is '%s'\n", str->val);
    string_destroy(&str);

    // parse next node until the end

    {
      Range* cursor;
      Range* tag_end;

      range_create(&cursor);

      range_copy(xd->node, cursor);
      cursor->begin = tag->end +1;

      tag_end = xmldoc_getnode(cursor);

      if( tag_end != 0 ) {
        String* str_end;

        // ah. this is the exiting node. horrible!

        if( *tag_end->begin == '/' ) {
          tag_end->begin++;
        }


        str_end = range_make_string(tag_end);

        printf("Next node is '%s'\n", str_end->val);

        if( range_equal( tag, tag_end ) == 1 ) {
          printf("Matching tags!");
        }

        string_destroy(&str_end);
        range_destroy(&tag_end);
      }

      range_destroy(&cursor);     
    }


    range_destroy(&tag);
  }

  return 0;
}

Range* xmldoc_getnodename(XmlDoc* xd)
{
  Range* r;
  
  range_create(&r);
  
  // todo: from xd->node
  
  return r;
}

XmlDoc* xmldoc_getnodeinner(XmlDoc* xd)
{
  XmlDoc* xml;

  xml = xmldoc_init(xd->node);

  return xml;
}
