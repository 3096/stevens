#ifndef __XENTAX_PS3_HEAVYRAIN_H
#define __XENTAX_PS3_HEAVYRAIN_H

#define X_SYSTEM PS3
#define X_GAME   HEAVYRAIN

namespace X_SYSTEM { namespace X_GAME {

bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
