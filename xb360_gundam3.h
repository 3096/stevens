#ifndef __XB360_GUNDAM3_H
#define __XB360_GUNDAM3_H

#include "xentax.h"

namespace Xbox360 { namespace DynastyWarriorsGundam3 {

 bool ExtractArchiveANS(void);
 bool ExtractArchiveBNS(void);
 bool ExtractArchiveCNS(void);
 bool ExtractDLC(const char* filename);

 // temporary
 bool Extract002(const binary_stream& bs, const char* path);
 bool Extract003(const binary_stream& bs, const char* path);
 bool ExtractModels(const binary_stream& bs, const char* path, size_t item);
 bool ExtractG1TG0060(const binary_stream& bs, const char* path, size_t item);

};};

#endif
