#ifndef __XENTAX_PC_TOMB_RAIDER_H
#define __XENTAX_PC_TOMB_RAIDER_H

#define X_SYSTEM PC
#define X_GAME   TOMB_RAIDER

namespace X_SYSTEM { namespace X_GAME {

bool processTR2(LPCTSTR filename);
bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
