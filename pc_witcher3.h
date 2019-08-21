#ifndef __XENTAX_PC_WITCHER_3_H
#define __XENTAX_PC_WITCHER_3_H

#define X_SYSTEM PC
#define X_GAME   WITCHER_3

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);
bool test(void);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
