#ifndef __XENTAX_WIN32_H
#define __XENTAX_WIN32_H

void CenterDialog(HWND window, bool in_parent = true);
BOOL BrowseDirectoryDialog(HWND parent, LPCTSTR caption, LPTSTR buffer);
BOOL ColorDialog(HWND parent, COLORREF& color);
BOOL OpenFileDialog(HWND parent, const char* filter, const char* title, const char* defext, char* filename, char* initdir = 0);
BOOL SaveFileDialog(HWND parent, const char* filter, const char* title, const char* defext, char* filename, char* initdir = 0);

#endif
