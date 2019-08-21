#ifndef __XENTAX_PS3_RISEGUARD_H
#define __XENTAX_PS3_RISEGUARD_H

#define X_SYSTEM PS3
#define X_GAME   ROG

namespace X_SYSTEM { namespace X_GAME {

bool extract(void);
bool extract(const char* pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
