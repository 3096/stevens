#include "xentax.h"
#include "x_win32.h"

void CenterDialog(HWND window, bool in_parent)
{
 HWND dialog = window;
 HWND parent = ((in_parent && GetParent(dialog)) ? GetParent(dialog) : GetDesktopWindow());

 RECT rect1; GetWindowRect(dialog, &rect1);
 RECT rect2; GetWindowRect(parent, &rect2);

 int dx1 = rect1.right - rect1.left;
 int dx2 = rect2.right - rect2.left;

 int dy1 = rect1.bottom - rect1.top;
 int dy2 = rect2.bottom - rect2.top;

 int x = (dx2 - dx1)/2 + rect1.left;
 int y = (dy2 - dy1)/2 + rect1.top;

 MoveWindow(dialog, x, y, dx1, dy1, TRUE);
}

// Browse Directory Dialog
int CALLBACK BrowseDirectoryDialogProc(HWND window, UINT message, LPARAM lparam, LPARAM)
{
 TCHAR directory[MAX_PATH];
 switch(message) {
   case(BFFM_INITIALIZED) : {
        if(GetCurrentDirectory(MAX_PATH, directory))
           SendMessage(window, BFFM_SETSELECTION,  1, (LPARAM)directory);
        break;
       }
   case(BFFM_SELCHANGED) : {
        if(SHGetPathFromIDList((LPITEMIDLIST)lparam, directory))
           SendMessage(window, BFFM_SETSTATUSTEXT, 0, (LPARAM)directory);
        break;
       }
  }
 return 0;
}

BOOL BrowseDirectoryDialog(HWND parent, LPCTSTR caption, LPTSTR buffer)
{
 TCHAR display_name[MAX_PATH];
 display_name[0] = 0;

 BROWSEINFO bi = {
  parent,
  0,
  display_name,
  caption,
  BIF_RETURNONLYFSDIRS,
  BrowseDirectoryDialogProc,
  0
 };

 ITEMIDLIST* pIIL = ::SHBrowseForFolder(&bi);
 if(SHGetPathFromIDList(pIIL, buffer)) {
    LPMALLOC pMalloc;
    HRESULT hr = SHGetMalloc(&pMalloc);
    pMalloc->Free(pIIL);
    pMalloc->Release();
    return TRUE;
   }

 return FALSE;
}

// Color Dialog
BOOL ColorDialog(HWND parent, COLORREF& color)
{
 static COLORREF custom[16] = {
  RGB(  0,  0,  0), RGB(255,255,255), RGB(128,128,128), RGB(192,192,192),
  RGB(128,  0,  0), RGB(  0,128,  0), RGB(  0,  0,128), RGB(128,128,  0),
  RGB(128,  0,128), RGB(  0,128,128), RGB(255, 255, 0), RGB(255,  0,255),
  RGB(  0,255,255), RGB(255,  0,  0), RGB(  0,255,  0), RGB(  0,  0,255)
 };
 CHOOSECOLOR data;
 ZeroMemory(&data, sizeof(data));
 data.lStructSize    = sizeof(CHOOSECOLOR);
 data.hwndOwner      = parent;
 data.hInstance      = NULL;
 data.rgbResult      = color;
 data.lpCustColors   = custom;
 data.Flags          = CC_RGBINIT | CC_FULLOPEN;
 data.lCustData      = 0;
 data.lpfnHook       = NULL;
 data.lpTemplateName = NULL;
 if(ChooseColor(&data) == FALSE) return FALSE; 
 else color = data.rgbResult;
 return TRUE;
}

// OpenSave Dialogs
BOOL OpenFileDialog(HWND parent, const char* filter, const char* title, const char* defext, char* filename, char* initdir)
{
 char buffer1[MAX_PATH];
 char buffer2[MAX_PATH];
 buffer1[0] = '\0';
 buffer2[0] = '\0';

 OPENFILENAMEA ofn;
 ZeroMemory(&ofn, sizeof(ofn));
 ofn.lStructSize     = sizeof(ofn);
 ofn.hwndOwner       = parent;
 ofn.hInstance       = (HINSTANCE)GetModuleHandle(0);
 ofn.lpstrFilter     = filter;
 ofn.lpstrFile       = buffer1;
 ofn.nMaxFile        = MAX_PATH;
 ofn.lpstrFileTitle  = buffer2;
 ofn.nMaxFileTitle   = MAX_PATH;
 ofn.lpstrInitialDir = initdir;
 ofn.lpstrTitle      = title;
 ofn.Flags           = OFN_FILEMUSTEXIST;
 ofn.lpstrDefExt     = defext;

 if(!GetOpenFileNameA(&ofn)) return FALSE;
 memmove(filename, buffer1, strlen(buffer1) + 1);
 return TRUE;
}

BOOL SaveFileDialog(HWND parent, const char* filter, const char* title, const char* defext, char* filename, char* initdir)
{
 char buffer1[MAX_PATH];
 char buffer2[MAX_PATH];
 buffer1[0] = '\0';
 buffer2[0] = '\0';

 OPENFILENAMEA ofn;
 ZeroMemory(&ofn, sizeof(ofn));
 ofn.lStructSize     = sizeof(ofn);
 ofn.hwndOwner       = parent;
 ofn.hInstance       = (HINSTANCE)GetModuleHandle(0);
 ofn.lpstrFilter     = filter;
 ofn.lpstrFile       = buffer1;
 ofn.nMaxFile        = MAX_PATH;
 ofn.lpstrFileTitle  = buffer2;
 ofn.nMaxFileTitle   = MAX_PATH;
 ofn.lpstrInitialDir = initdir;
 ofn.lpstrTitle      = title;
 ofn.Flags           = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
 ofn.lpstrDefExt     = defext;

 if(!GetSaveFileNameA(&ofn)) return FALSE;
 memmove(filename, buffer1, strlen(buffer1) + 1);
 return TRUE;
}