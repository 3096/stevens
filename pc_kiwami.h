#ifndef __XENTAX_PC_KIWAMI_H
#define __XENTAX_PC_KIWAMI_H

#define X_SYSTEM PC
#define X_GAME   KIWAMI

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
