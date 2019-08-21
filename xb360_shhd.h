#ifndef __XENTAX_XBOX360_SILENTHILL_H
#define __XENTAX_XBOX360_SILENTHILL_H

#define X_SYSTEM XBOX360
#define X_GAME   SILENTHILL

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
