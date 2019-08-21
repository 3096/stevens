#ifndef __XENTAX_PS3_DRAGONS_DOGMA_DARK_ARISEN_H
#define __XENTAX_PS3_DRAGONS_DOGMA_DARK_ARISEN_H

#define X_SYSTEM PS3
#define X_GAME   DRAGONS_DOGMA_DARK_ARISEN

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
