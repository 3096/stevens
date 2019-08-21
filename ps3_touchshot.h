#ifndef __XENTAX__H
#define __XENTAX__H

#define X_SYSTEM PS3
#define X_GAME   TOUCHSHOT

namespace X_SYSTEM { namespace X_GAME {

bool extract(void);
bool extract(const char* pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
