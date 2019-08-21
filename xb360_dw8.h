#ifndef __XENTAX_XBOX360_DW8_H
#define __XENTAX_XBOX360_DW8_H

#define X_SYSTEM XBOX360
#define X_GAME   DW8

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
