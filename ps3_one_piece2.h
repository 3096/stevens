#ifndef __XENTAX_PS3_ONE_PIECE_TWO_H
#define __XENTAX_PS3_ONE_PIECE_TWO_H

#define X_SYSTEM PS3
#define X_GAME   ONE_PIECE_X2

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
