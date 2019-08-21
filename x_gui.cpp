#include "xentax.h"
#include "stdres.h"
#include "x_file.h"
#include "x_win32.h"
#include "x_gui.h"

struct GAMEITEM {
 CStringT<TCHAR, StrTraitATL<TCHAR>> title;
 CStringT<TCHAR, StrTraitATL<TCHAR>> instructions;
 CStringT<TCHAR, StrTraitATL<TCHAR>> notes;
 EXTRACTION_CALLBACK func;
};
static std::deque<GAMEITEM> gamelist;
static CStringT<TCHAR, StrTraitATL<TCHAR>> gamename;
static CStringT<TCHAR, StrTraitATL<TCHAR>> pathname;
static TCHAR defaultGame[1024];
static TCHAR defaultPathname[1024];

void AddGameTitle(LPCTSTR title, LPCTSTR instructions, LPCTSTR notes, EXTRACTION_CALLBACK func)
{
 GAMEITEM item;
 item.title = title;
 item.instructions = instructions;
 item.notes = notes;
 item.func = func;
 gamelist.push_back(item);
}

INT_PTR CALLBACK RipperProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
 switch(message) {
   case(WM_INITDIALOG) :
       {
        HWND gameCtrl = GetDlgItem(window, IDC_GAME);
        HWND instCtrl = GetDlgItem(window, IDC_INSTRUCTIONS);
        HWND noteCtrl = GetDlgItem(window, IDC_NOTES);
        HWND pathCtrl = GetDlgItem(window, IDC_PATHNAME);
        if(!gameCtrl || !instCtrl || !noteCtrl || !pathCtrl) {
           MessageBox(window, TEXT("Dialog box is missing one or more controls."), TEXT("Error"), MB_ICONSTOP);
           EndDialog(window, IDCANCEL);
           return TRUE;
          }
        // set maximum path length
        //Edit_LimitText(pathCtrl, MAX_PATH);
        // insert games
        for(size_t i = 0; i < gamelist.size(); i++)
            SendMessage(gameCtrl, CB_ADDSTRING, 0, (LPARAM)((LPCTSTR)gamelist[i].title));
        SendMessage(gameCtrl, CB_SETCURSEL, 0, 0);

        // if there is a default game
        if(std::char_traits<TCHAR>::length(defaultGame)) {
           int index = SendMessage(gameCtrl, CB_FINDSTRING, -1, (LPARAM)defaultGame);
           if(index != CB_ERR) SendMessage(gameCtrl, CB_SETCURSEL, index, 0);
           SetDlgItemText(window, IDC_INSTRUCTIONS, (LPCTSTR)gamelist[index].instructions);
           SetDlgItemText(window, IDC_NOTES, (LPCTSTR)gamelist[index].notes);
          }

        // if there is a default pathname
        if(std::char_traits<TCHAR>::length(defaultPathname)) {
           pathname = defaultPathname;
           SetWindowText(pathCtrl, pathname);
          }

        return TRUE;
       }
   case(WM_COMMAND) : {
        // IDOK
        if(LOWORD(wparam) == IDOK)
          {
           // control handles
           HWND gameCtrl = GetDlgItem(window, IDC_GAME);
           HWND pathCtrl = GetDlgItem(window, IDC_PATHNAME);
           if(!gameCtrl || !pathCtrl) {
              MessageBox(window, TEXT("Dialog box is missing one or more controls."), TEXT("Error"), MB_ICONSTOP);
              EndDialog(window, IDCANCEL);
              return TRUE;
             }

           // get name of game
           UINT index = SendMessage(gameCtrl, CB_GETCURSEL, 0, 0);
           if(index == 0) {
              MessageBox(window, TEXT("You must select a game."), TEXT("Error"), MB_ICONSTOP);
              return TRUE;
             }
           TCHAR temp1[1024];
           temp1[0] = 0;
           SendMessage(gameCtrl, CB_GETLBTEXT, index, (LPARAM)&temp1[0]);

           // get pathname
           TCHAR temp2[1024];
           temp2[0] = 0;
           GetWindowText(pathCtrl, &temp2[0], 1024);
           if(!_tcslen(temp2)) {
              MessageBox(window, TEXT("You must select a valid pathname."), TEXT("Error"), MB_ICONSTOP);
              return TRUE;
             }

           // set name of game and pathname
           gamename = temp1;
           pathname = temp2;

           // append \\ to pathname at end if not present
           if(pathname[pathname.GetLength() - 1] != TCHAR('\\'))
              pathname.AppendChar(TCHAR('\\'));

           // end dialog
           EndDialog(window, IDOK);
           return TRUE;
          }
        // IDCANCEL
        if(LOWORD(wparam) == IDCANCEL) {
           EndDialog(window, IDCANCEL);
           return TRUE;
          }
        if(LOWORD(wparam) == IDC_BROWSE) {
           TCHAR buffer[MAX_PATH];
           if(BrowseDirectoryDialog(window, TEXT("Select Folder"), &buffer[0]))
              SetDlgItemText(window, IDC_PATHNAME, (LPCTSTR)&buffer[0]);
           return TRUE;
          }
        // IDC_EXEPATH
        if(LOWORD(wparam) == IDC_EXEPATH) {
           auto pathname = GetModulePathname();
           SetDlgItemText(window, IDC_PATHNAME, (LPCTSTR)pathname.get());
           return TRUE;
          }
        // CBN_SELCHANGE
        if((LOWORD(wparam) == IDC_GAME) && (HIWORD(wparam) == CBN_SELCHANGE)) {
           HWND gameCtrl = (HWND)lparam;
           UINT index = SendMessage(gameCtrl, CB_GETCURSEL, 0, 0);
           SetDlgItemText(window, IDC_INSTRUCTIONS, (LPCTSTR)gamelist[index].instructions);
           SetDlgItemText(window, IDC_NOTES, (LPCTSTR)gamelist[index].notes);
           return TRUE;
          }
        return TRUE;
       }
   default : return FALSE;
  }
 return FALSE;
}

BOOL ShowGameTitles(void)
{
 // create and show dialog
 INT_PTR result = DialogBox(GetModuleHandle(NULL), TEXT("IDD_RIPPER"), NULL, RipperProc);
 if(result == IDCANCEL) {
    message("Ripping canceled.");
    return TRUE;
   }

 // call callback
 for(size_t i = 0; i < gamelist.size(); i++)
     if(gamename == gamelist[i].title)
        return (*gamelist[i].func)((LPCTSTR)pathname);

 // otherwise failed
 return FALSE;
}

BOOL SetDefaultGame(LPCTSTR gamename)
{
 StrCpy(defaultGame, gamename);
 return TRUE;
}

BOOL SetGameFolder(LPCTSTR pathname)
{
 StrCpy(defaultPathname, pathname);
 return TRUE;
}