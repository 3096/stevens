#ifndef __XENTAX_PS3_SENGOKU_MUSOU_4_H
#define __XENTAX_PS3_SENGOKU_MUSOU_4_H

#define X_SYSTEM PS3
#define X_GAME   SENGOKU_MUSOU_4

namespace X_SYSTEM { namespace X_GAME {

bool processBIN(LPCTSTR filename);
bool processIDXOUT(LPCTSTR filename);
bool processIDXZRC(LPCTSTR filename);

}};

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
