#ifndef __XENTAX_PC_DW8XL_H
#define __XENTAX_PC_DW8XL_H

#define X_SYSTEM PC
#define X_GAME   DW8XL

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
