#ifndef MYST_XML_HEADER_H
#define MYST_XML_HEADER_H

#include "myst_bin.h"
#include "stream.h"

FStream* create_xml_stream(const char* fn);

Locale* myst_read_xml(FStream *fs);
void myst_write_xml(FStream* out_xml, Locale* loc);

#endif // MYST_XML_HEADER_H
