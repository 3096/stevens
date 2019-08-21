#ifndef __XENTAX_PC_ARSLAN_H
#define __XENTAX_PC_ARSLAN_H

#define X_SYSTEM PC
#define X_GAME   ARSLAN

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
