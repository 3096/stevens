#ifndef __XENTAX_PC_RE7_H
#define __XENTAX_PC_RE7_H

#define X_SYSTEM PC
#define X_GAME   RE7

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
