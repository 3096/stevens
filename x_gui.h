#ifndef __XENTAX_XGUI_H
#define __XENTAX_XGUI_H

// GAME TITLE DIALOG
typedef bool (*EXTRACTION_CALLBACK)(LPCTSTR);
void AddGameTitle(LPCTSTR title, LPCTSTR instructions, LPCTSTR notes, EXTRACTION_CALLBACK func);
BOOL ShowGameTitles(void);
BOOL SetDefaultGame(LPCTSTR gamename);
BOOL SetGameFolder(LPCTSTR pathname);

#endif
