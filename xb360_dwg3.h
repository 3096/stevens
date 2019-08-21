#ifndef __XENTAX_XBOX360_DWG3_H
#define __XENTAX_XBOX360_DWG3_H

#define X_SYSTEM XBOX360
#define X_GAME   DWG3

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
