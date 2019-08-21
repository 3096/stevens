#ifndef __XENTAX_PS3_MACROSS30_H
#define __XENTAX_PS3_MACROSS30_H

#define X_SYSTEM PS3
#define X_GAME   MACROSS30

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
