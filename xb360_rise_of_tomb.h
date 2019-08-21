#ifndef __XENTAX_XB360_RISE_OF_TOMB_RAIDER_H
#define __XENTAX_XB360_RISE_OF_TOMB_RAIDER_H

#define X_SYSTEM XBOX360
#define X_GAME   RISE_OF_TOMB_RAIDER

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
