#ifndef __XENTAX_PC_RISE_OF_INCARNATES_H
#define __XENTAX_PC_RISE_OF_INCARNATES_H

#define X_SYSTEM PC
#define X_GAME   RISE_OF_INCARNATES

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
