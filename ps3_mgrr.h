#ifndef __XENTAX_PS3_MGRR_H
#define __XENTAX_PS3_MGRR_H

#define X_SYSTEM PS3
#define X_GAME   MGRR

namespace X_SYSTEM { namespace X_GAME {

bool extract(void);
bool extract(const char* pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
