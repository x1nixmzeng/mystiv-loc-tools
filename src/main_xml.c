#include <stdio.h>

#include "xml.h"
#include "textrange.h"
#include "string.h"

int main(int argc, char** argv)
{
  XmlDoc* xml;
  Range* xml_range;
  String* str;

  if( argc != 2 ) {
    printf("Usage: xml <string data>\n");
    return 0;
  }

  xml_range = range_from_string(argv[1]);

  xml = xmldoc_init(xml_range);
  range_destroy(&xml_range);

  printf("XML demonstration\n");

  while( xmldoc_next(xml) != 0 )
  {
    Range* r;

    r = xmldoc_getnodename(xml);

    str = range_make_string(r);

    printf("Node '%s'\n", str->val);

    {
      XmlDoc* inner;
      inner = xmldoc_getnodeinner(xml);

      

      xmldoc_destroy(&inner);
    }

    string_destroy(&str);
    range_destroy(&r);
  }

//  string_destroy(&str);
  xmldoc_destroy(&xml);

  mem_dbg();

  return 0;
}
