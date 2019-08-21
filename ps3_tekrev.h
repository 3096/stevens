#ifndef __XENTAX_PS3_TEKKEN_REVOLUTION_H
#define __XENTAX_PS3_TEKKEN_REVOLUTION_H

#define X_SYSTEM PS3
#define X_GAME   TEKKEN_REVOLUTION

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

// OTHER FUNCTIONS
// These functions are exposed to help when other games by
// Namco use the same format as Tekken Revolusion.
namespace X_SYSTEM { namespace X_GAME {

}};

#undef X_SYSTEM
#undef X_GAME

#endif
