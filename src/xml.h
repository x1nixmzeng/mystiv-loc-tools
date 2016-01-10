#ifndef XML_HEADER_H
#define XML_HEADER_H

#include "wtextrange.h"

// Heavily inspired by yxml library by Yoran Heling
// to support parsing utf-16 text ranges

typedef enum
{
  kXmlHintUnknown,
  kXmlHintEnded,

  kXmlHintStartDeclaration,
  kXmlHintStartElementOpen,
  kXmlHintStartElementClose,
  kXmlHintStartAttributeName,
  kXmlHintStartAttributeValue,
  kXmlHintStartInnerText,

  kXmlHintDeclaration,
  kXmlHintElementOpen,
  kXmlHintElementClose,
  kXmlHintAttributeName,
  kXmlHintAttributeValue,
  kXmlHintInnerText,

  kXmlHintEndDeclaration,
  kXmlHintEndElementOpen,
  kXmlHintEndElementClose,
  kXmlHintEndAttributeName,
  kXmlHintEndAttributeValue,
  kXmlHintEndInnerText,

} XmlHint;

typedef struct XmlRange
{
  WRange *range;
  const short* cursor;
  XmlHint context;
} Xml;

void xml_create(Xml** xml);
void xml_destroy(Xml** xml);

XmlHint xml_parse(Xml* xml);

#endif
