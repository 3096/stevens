#ifndef __XENTAX_BIOSHOCK_H
#define __XENTAX_BIOSHOCK_H

#define X_SYSTEM PS3
#define X_GAME   BIOSHOCK

namespace X_SYSTEM { namespace X_GAME {

bool extract(void);
bool extract(const char* pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
