#ifndef __XENTAX_PS3_UNLIMITED_WORLD_RED_H
#define __XENTAX_PS3_UNLIMITED_WORLD_RED_H

#define X_SYSTEM PS3
#define X_GAME   UNLIMITED_WORLD_RED

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
