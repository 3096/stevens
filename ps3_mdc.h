#ifndef __XENTAX_MDC_H
#define __XENTAX_MDC_H

#define X_SYSTEM PS3
#define X_GAME   MAHJONG_DREAM_CLUB

namespace X_SYSTEM { namespace X_GAME {

bool extract(void);
bool extract(const char* pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
