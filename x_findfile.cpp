#include "xentax.h"
#include "x_findfile.h"

find_file::find_file() : handle(0)
{
 ZeroMemory(&find32, sizeof(find32));
}

find_file::~find_file()
{
 close();
}

void find_file::close(void)
{
 // must check for valid handle in 
 // VC++ .NET or else exception results
 if(handle) FindClose(handle);
 handle = 0;
 ZeroMemory((void*)&find32, sizeof(find32)); 
}

bool find_file::find(LPCTSTR filename)
{
 close();
 handle = FindFirstFile(filename, &find32);
 if(handle == INVALID_HANDLE_VALUE) handle = 0;
 return (handle != 0);
}

bool find_file::next(void)
{
 if(handle == 0) return false;
 return (FindNextFile(handle, &find32) == TRUE);
}

bool find_file::is_dots(void)
{
 size_t len = 0;
 StringCchLength(find32.cFileName, MAX_PATH, &len);
 TCHAR c = 0x2E;
 if(len == 1 && (find32.cFileName[0] == c)) return true;
 if(len == 2 && (find32.cFileName[0] == c) && (find32.cFileName[1] == c)) return true;
 return false;
}

bool find_file::is_directory(void)
{
 return ((find32.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool find_file::is_encrypted(void)
{
 return ((find32.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED) != 0);
}

bool find_file::is_compressed(void)
{
 return ((find32.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) != 0);
}

bool find_file::is_normal(void)
{
 return ((find32.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) != 0);
}

bool find_file::is_hidden(void)
{
 return ((find32.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0);
}

bool find_file::is_system(void)
{
 return ((find32.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0);
}

bool find_file::is_archived(void)
{
 return ((find32.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) != 0);
}

bool find_file::is_readonly(void)
{
 return ((find32.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0);
}

LPCTSTR find_file::filename(void)
{
 return (LPCTSTR)find32.cFileName;
}

size_t find_file::filesize(void)
{
 return (find32.nFileSizeHigh*(MAXDWORD + 1)) + find32.nFileSizeLow;
}

bool find_file::operator !(void)const
{
 return !handle;
}