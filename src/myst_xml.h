#ifndef MYST_XML_HEADER_H
#define MYST_XML_HEADER_H

#include "myst_bin.h"
#include "stream.h"

FStream* create_xml_stream(const char* fn);

void myst_write_xml_locale(FStream* out_xml, Locale* loc);

Locale* xml_read(FStream *fs);

#endif // MYST_XML_HEADER_H
