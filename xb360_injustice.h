#ifndef __XENTAX_XBOX360_INJUSTICE_H
#define __XENTAX_XBOX360_INJUSTICE_H

#define X_SYSTEM XBOX360
#define X_GAME   INJUSTICE

namespace X_SYSTEM { namespace X_GAME {

bool extract(void);
bool extract(const char* pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
