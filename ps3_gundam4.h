#ifndef __XENTAX_DWG4_H
#define __XENTAX_DWG4_H

#define X_SYSTEM PS3
#define X_GAME   DWG4

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
