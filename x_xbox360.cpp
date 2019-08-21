#include "xentax.h"
#include "x_file.h"
#include "x_xbox360.h"

bool XB360Unbundle(LPCTSTR exedir, LPCTSTR filename, LPCTSTR outdir)
{
 // create path if no path was specified
 using namespace std;
 STDSTRING pathname;
 if(exedir) pathname = exedir;
 else pathname = GetModulePathname().get();

 // command line
 STDSTRING cmdline = TEXT("\"");
 cmdline += pathname.c_str();
 cmdline += TEXT("unbundler.exe");
 cmdline += TEXT("\"");
 cmdline += TEXT(" ");
 cmdline += TEXT("\"");
 cmdline += filename;
 cmdline += TEXT("\"");

 // process startup information
 STARTUPINFO sinfo;
 ZeroMemory(&sinfo, sizeof(sinfo));
 sinfo.cb = sizeof(sinfo);

 // process information
 PROCESS_INFORMATION pinfo;
 ZeroMemory(&pinfo, sizeof(pinfo));

 // copy command line string
 TCHAR buffer[1024];
 StringCchCopy(buffer, 1024, cmdline.c_str());

 // run xbdecompress
 if(!CreateProcess(NULL, buffer, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, outdir, &sinfo, &pinfo)) return error("Failed to run unbundler on XPR file.");
 WaitForSingleObject(pinfo.hProcess, INFINITE);
 CloseHandle(pinfo.hProcess);
 CloseHandle(pinfo.hThread);

 return true;
}

bool XB360Decompress(LPCTSTR exedir, LPCTSTR fname)
{
 // filename properties
 using namespace std;
 STDSTRING filename = fname;
 STDSTRING pathname = GetPathnameFromFilename(filename.c_str()).get();
 STDSTRING shrtname = GetShortFilenameWithoutExtension(filename.c_str()).get();
 STDSTRING file_ext = GetExtensionFromFilename(filename.c_str()).get();

 if(pathname.length() == 0) {
    pathname = GetModulePathname().get();
    filename = pathname;
    filename += shrtname;
    filename += file_ext;
   }

 // construct temporary output filename
 STDSTRING tempname = pathname;
 tempname += shrtname;
 tempname += TEXT("_temp");
 tempname += file_ext;

 // create path to unbundler
 STDSTRING ubunpath = (exedir ? exedir : GetModulePathname().get());
 if(!ubunpath.length()) return false;

 // "ubunpath\\unbundler.exe"
 STDSTRING cmdline = TEXT("\"");
 cmdline += ubunpath.c_str();
 cmdline += TEXT("xbdecompress.exe");
 cmdline += TEXT("\"");
 cmdline += TEXT(" ");
 // "filename"
 cmdline += TEXT("\"");
 cmdline += filename;
 cmdline += TEXT("\"");
 cmdline += TEXT(" ");
 // "tempname"
 cmdline += TEXT("\"");
 cmdline += tempname;
 cmdline += TEXT("\"");

 // process startup information
 STARTUPINFO sinfo;
 ZeroMemory(&sinfo, sizeof(sinfo));
 sinfo.cb = sizeof(sinfo);

 // process information
 PROCESS_INFORMATION pinfo;
 ZeroMemory(&pinfo, sizeof(pinfo));

 // copy command line string
 TCHAR buffer[1024];
 StringCchCopy(buffer, 1024, cmdline.c_str());

 // decompress to temporary file
 if(!CreateProcess(NULL, buffer, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, pathname.c_str(), &sinfo, &pinfo)) return error("Failed to run xbdecompress on file.");
 WaitForSingleObject(pinfo.hProcess, INFINITE);
 CloseHandle(pinfo.hProcess);
 CloseHandle(pinfo.hThread);

 // rename file
 DeleteFile(filename.c_str());
 MoveFile(tempname.c_str(), filename.c_str());

 // success
 return true;
}