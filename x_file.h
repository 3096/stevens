#ifndef __XENTAX_FILE_H
#define __XENTAX_FILE_H

//
// STRING FUNCTIONS
//

boost::shared_array<TCHAR> StringToSharedArray(LPCTSTR str);
STDSTRING CreateSavePath(LPCTSTR pathname, LPCTSTR shrtname);
bool CreateShellSavePath(LPCTSTR pathname);

//
// FILENAME FUNCTIONS
//

boost::shared_array<TCHAR> GetModulePathname(void);
boost::shared_array<TCHAR> GetShortFilename(LPCTSTR filename);
boost::shared_array<TCHAR> GetShortFilenameWithoutExtension(LPCTSTR filename);
boost::shared_array<TCHAR> GetPathnameFromFilename(LPCTSTR filename);
boost::shared_array<TCHAR> GetExtensionFromFilename(LPCTSTR filename);
boost::shared_array<TCHAR> ChangeFileExtension(LPCTSTR filename, LPCTSTR extension);
BOOL HasExtension(LPCTSTR filename, LPCTSTR extension);
BOOL FileExists(LPCTSTR filename);
uint64 GetFileSize(std::ifstream& ifile);

/*
** FILE LISTING FUNCTIONS
** (1) LOOKS IN PATH OF EXE
** (2) LOOKS IN PATH OF CHOICE
*/

bool BuildNonRecursiveFilenameList(std::deque<STDSTRING>& namelist, LPCTSTR fext, LPCTSTR rootname);
bool BuildFilenameList(std::deque<STDSTRING>& namelist, LPCTSTR fext);
bool BuildFilenameList(std::deque<STDSTRING>& namelist, LPCTSTR fext, LPCTSTR rootname);
bool BuildWildcardFilenameList(std::deque<STDSTRING>& namelist);
bool BuildWildcardFilenameList(std::deque<STDSTRING>& namelist, LPCTSTR rootname);
bool DeleteFilesIncludingExtensions(const std::set<STDSTRING>& filelist);
bool DeleteFilesExcludingExtensions(const std::set<STDSTRING>& filelist);
bool DeleteFilesIncludingExtensions(LPCTSTR rootname, const std::set<STDSTRING>& filelist);
bool DeleteFilesExcludingExtensions(LPCTSTR rootname, const std::set<STDSTRING>& filelist);
bool DeleteEmptyFolders(LPCTSTR rootname);

//
// FILE TYPE SEARCHING
//

bool GetSubdirectoryList(std::deque<STDSTRING>& namelist, LPCTSTR rootname);
bool SearchFolderForFileType(const STDSTRING& rootname, const STDSTRING& fext);
bool SearchFolderForFileType(const STDSTRING& rootname, const std::deque<STDSTRING>& fext);

//
// BATCH EXTENSION RENAMING
//

bool BatchFileExtensionRename(const STDSTRING& rootname, const STDSTRING& org, const STDSTRING& rep, bool progress = false);

//
// FILE SEARCHING FUNCTIONS
//

bool SearchFileForSignature(std::ifstream& ifile, const char* signature, size_t sigsize);
bool SearchFileForSignature(std::ifstream& ifile, const char* signature, size_t sigsize, std::deque<uint64>& offsets);
bool SearchFileForSignature(std::ifstream& ifile, const char* signature, size_t sigsize, size_t alignment, std::deque<uint64>& offsets);

//
// FILE MERGING FUNCTIONS
//

bool MergeBinaryFiles(const std::deque<STDSTRING>& filelist, const STDSTRING& filename);

#endif
