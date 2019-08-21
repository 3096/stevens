#ifndef __XENTAX_RON_H
#define __XENTAX_RON_H

#define X_SYSTEM XBOX360
#define X_GAME   RON

namespace X_SYSTEM { namespace X_GAME {

bool extract(void);
bool extract(const char* pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
