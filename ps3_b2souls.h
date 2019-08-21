#ifndef __XENTAX_PS3_BEYOND_TWO_SOULS_H
#define __XENTAX_PS3_BEYOND_TWO_SOULS_H

#define X_SYSTEM PS3
#define X_GAME   BEYOND_TWO_SOULS

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
