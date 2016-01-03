#ifndef XML_HEADER_H
#define XML_HEADER_H

#include "textrange.h"

typedef struct XmlDocument
{
  Range* data;
  Range* node;
  const char* cursor;
} XmlDoc;

void xmldoc_create(XmlDoc** xd);
void xmldoc_destroy(XmlDoc** xd);

XmlDoc* xmldoc_init(Range* r);
XmlDoc* xmldoc_getnodeinner(XmlDoc* xd);

int xmldoc_next(XmlDoc* xd);
Range* xmldoc_getnodename(XmlDoc* xd);

#endif
