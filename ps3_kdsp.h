#ifndef __XENTAX_PS3_KAGERO_DSP_H
#define __XENTAX_PS3_KAGERO_DSP_H

#define X_SYSTEM PS3
#define X_GAME   KAGERO_DSP

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
