#include "xml.h"
#include "memory.h"

void xml_create(Xml** xml)
{
  *xml = (Xml*)mem_alloc(sizeof(Xml));
  
  (*xml)->range = 0;
  (*xml)->cursor = 0;
  (*xml)->context = kXmlHintUnknown;
  
  wrange_create(&(*xml)->range);
}

void xml_destroy(Xml** xml)
{
  wrange_destroy(&(*xml)->range);

  mem_free(*xml);
}

void xml_skip_whitespace(Xml* xml)
{
  while (xml->cursor < xml->range->end) {
    switch (*xml->cursor) {
      case 0:
      case ' ':
      case '\r':
      case '\t':
      case '\n':
        ++xml->cursor;
        break;
      default:
        return;
    }
  }
}

int xml_char(const short c)
{
  if ((c >= 'a' && c <= 'z')
    || (c >= 'A' && c <= 'Z')
    || (c >= '0' && c <= '9')
    || c == '_' || c == '.' ) {
    return 0;
  } else {
    return 1;
  }
}

XmlHint xml_parse(Xml* xml)
{
  switch (xml->context)
  {
    case kXmlHintUnknown:
    case kXmlHintEndDeclaration:
    case kXmlHintEndElementClose:
    case kXmlHintEndInnerText:
    {
      xml_skip_whitespace(xml);

      if (*xml->cursor == '<') {
        xml->cursor++;

        switch (*xml->cursor) {
        case '/':
          ++xml->cursor;
          xml->context = kXmlHintStartElementClose;
          break;
        case '?':
          ++xml->cursor;
          xml->context = kXmlHintStartDeclaration;
          break;
        default:
          xml->context = kXmlHintStartElementOpen;
        }
      } else {
        xml->context = kXmlHintEnded;
      }
    }
    break;

    case kXmlHintEndElementOpen:
    {
      xml_skip_whitespace(xml);

      if(*xml->cursor == '<') {
        xml->cursor++;
        if (*xml->cursor == '/') {
          ++xml->cursor;
          xml->context = kXmlHintStartElementClose;
        } else {
          xml->context = kXmlHintStartElementOpen;
        }
      } else {
        xml->context = kXmlHintStartInnerText;
      }
    }
    break;
    
    case kXmlHintStartInnerText:
    case kXmlHintInnerText:
    {
      if (*xml->cursor == '<' ) {
        xml->context = kXmlHintEndInnerText;
      } else {
        ++xml->cursor;

        if (*xml->cursor == '<') {
          xml->context = kXmlHintEndInnerText;
        } else {
          xml->context = kXmlHintInnerText;
        }
      }
    }
    break;

    case kXmlHintStartDeclaration:
    case kXmlHintStartElementOpen:
    case kXmlHintStartElementClose:
    {
      ++xml->cursor;
      unsigned short chr = *xml->cursor;
  
      if (xml_char(chr) == 0) {
        switch (xml->context) {
        case kXmlHintStartElementOpen:
          xml->context = kXmlHintElementOpen;
          break;
        case kXmlHintStartElementClose:
          xml->context = kXmlHintElementClose;
          break;
        case kXmlHintStartDeclaration:
          xml->context = kXmlHintDeclaration;
          break;
        default:
          xml->context = kXmlHintEnded;
        }
      } else {
        xml->context = kXmlHintEnded;
      }
    }
    break;

    case kXmlHintDeclaration:
    {
      ++xml->cursor;
      unsigned short chr = *xml->cursor;
  
      if (xml_char(chr) == 0) {
        //xml->context = kXmlHintDeclaration;
      }
      else if (chr == ' ') {
        ++xml->cursor;
        xml->context = kXmlHintStartAttributeName;
      } else if (chr == '?') {
        ++xml->cursor;
        if (*xml->cursor == '>') {
          ++xml->cursor;
          xml->context = kXmlHintEndDeclaration;
        } else {
          xml->context = kXmlHintEnded;
        }
      }
    }
    break;

    case kXmlHintElementOpen:
    {
      ++xml->cursor;
      unsigned short chr = *xml->cursor;
  
      if (xml_char(chr) == 0) {
        //xml->context = kXmlHintElementOpen;
      }
      else if (chr == ' ') {
        ++xml->cursor;
        xml->context = kXmlHintStartAttributeName;
      } else if (chr == '>') {
        ++xml->cursor;
        xml->context = kXmlHintEndElementOpen;
      }
    }
    break;

    case kXmlHintElementClose:
    {
      ++xml->cursor;
      unsigned short chr = *xml->cursor;
  
      if (xml_char(chr) == 0) {
        //xml->context = kXmlHintElementClose;
      } else if (chr == '>') {
        ++xml->cursor;
        xml->context = kXmlHintEndElementClose;

        if (xml->cursor == xml->range->end) {
          xml->context = kXmlHintEnded;
        }
      }
    }
    break;

    case kXmlHintAttributeName:
    case kXmlHintStartAttributeName:
    {
      ++xml->cursor;
      unsigned short chr = *xml->cursor;

      if (xml_char(chr) == 0) {
        xml->context = kXmlHintAttributeName;
      } else if (chr == '=') {
        ++xml->cursor;
        xml->context = kXmlHintEndAttributeName;
      } else {
        xml->context = kXmlHintEnded;
      }
    }
    break;

    case kXmlHintEndAttributeName:
    {
      if (*xml->cursor == '"') {
        ++xml->cursor;
        xml->context = kXmlHintStartAttributeValue;
      } else if (*xml->cursor == '>') {
        ++xml->cursor;
        xml->context = kXmlHintEndElementOpen;
      }
    }
    break;

    case kXmlHintStartAttributeValue:
    {
      ++xml->cursor;
      unsigned short chr = *xml->cursor;

      if (xml_char(chr) == 0) {
        xml->context = kXmlHintAttributeValue;
      } else if (chr == '"') {
        xml->context = kXmlHintEndAttributeValue;
      } else if (chr == '>') {
        xml->context = kXmlHintEnded;
      }
    }
    break;

    case kXmlHintAttributeValue:
    {
      ++xml->cursor;
      unsigned short chr = *xml->cursor;

      if (xml_char(chr) == 0) {
        xml->context = kXmlHintAttributeValue;
      } else if (chr == '"') {
        xml->context = kXmlHintEndAttributeValue;
      } else if (chr == '>') {
        xml->context = kXmlHintEnded;
      }
    }
    break;

    case kXmlHintEndAttributeValue:
    {
      ++xml->cursor;

      switch (*xml->cursor) {
      case '>':
        ++xml->cursor;
        xml->context = kXmlHintEndElementOpen;
        break;
      case '?':
        ++xml->cursor;
        if (*xml->cursor == '>') {
          ++xml->cursor;
          xml->context = kXmlHintEndDeclaration;
        } else {
          xml->context = kXmlHintEnded;
        }
        break;
      case ' ':
        // todo: check this isn't just whitespace
        ++xml->cursor;
        xml->context = kXmlHintStartAttributeName;
        break;
      default:
        xml->context = kXmlHintEnded;
      }
    }
    break;

    default:
    {
      xml->context = kXmlHintEnded;
    }
    break;
  }

  return xml->context;
}
