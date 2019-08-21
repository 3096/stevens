#ifndef __XENTAX_PS3_DREAMCLUB_GOGO_H
#define __XENTAX_PS3_DREAMCLUB_GOGO_H

#define X_SYSTEM PS3
#define X_GAME   DREAMCLUB_GOGO

namespace X_SYSTEM { namespace X_GAME {

bool processCAT(LPCTSTR filename);
bool processCAN(LPCTSTR filename);
bool processGTF(LPCTSTR filename);
bool processTMD(LPCTSTR filename);

}};

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
