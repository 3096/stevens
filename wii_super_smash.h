#ifndef __XENTAX_WIIU_SUPER_SMASH_BROS_H
#define __XENTAX_WIIU_SUPER_SMASH_BROS_H

#define X_SYSTEM WIIU
#define X_GAME   SUPER_SMASH_BROS

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
