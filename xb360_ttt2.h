#ifndef __XENTAX_XB360_TTT2_H
#define __XENTAX_XB360_TTT2_H

#define X_SYSTEM XBOX360
#define X_GAME   TTT2

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

// OTHER FUNCTIONS
// These functions are exposed to help when other games by
// Namco use the same format as Tekken Tag Tournament 2.
namespace X_SYSTEM { namespace X_GAME {

bool processTTT(LPCTSTR filename);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
