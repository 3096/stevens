#ifndef __XENTAX_DRAGON_QUEST_HEROES_H
#define __XENTAX_DRAGON_QUEST_HEROES_H

#define X_SYSTEM PS3
#define X_GAME   DRAGON_QUEST_HEROES

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void);
bool extract(void);
bool extract(LPCTSTR pathname);

}};

#undef X_SYSTEM
#undef X_GAME

#endif
