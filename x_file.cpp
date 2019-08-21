#include "xentax.h"
#include "x_findfile.h"
#include "x_file.h"

boost::shared_array<TCHAR> StringToSharedArray(LPCTSTR str)
{
 // compute length component
 size_t len = 0;
 if(FAILED(StringCchLength(str, MAX_PATH, &len))) return boost::shared_array<TCHAR>();
 if(!len) return boost::shared_array<TCHAR>();

 // create component
 boost::shared_array<TCHAR> retval(new TCHAR[len + 1]);
 retval[0] = static_cast<TCHAR>(0);

 // copy component
 LPTSTR endptr = NULL;
 if(FAILED(StringCchCopy(retval.get(), (len + 1), str))) return boost::shared_array<TCHAR>();
 return retval;
}

STDSTRING CreateSavePath(LPCTSTR pathname, LPCTSTR shrtname)
{
 // create save path
 STDSTRING savepath = pathname;
 savepath += STDSTRING(shrtname);
 savepath += TEXT("\\");

 // create directory
 if(CreateDirectory(savepath.c_str(), NULL) == FALSE) {
    DWORD code = GetLastError();
    if(code != ERROR_ALREADY_EXISTS) {
       std::stringstream ss;
       ss << "ERROR: CreateDirectory() failed with error code 0x";
       ss << std::setfill('0') << std::setw(8) << std::hex << GetLastError() << std::dec << ".";
       error(ss.str().c_str());
       return STDSTRING();
      }
   }

 return savepath;
}

bool CreateShellSavePath(LPCTSTR pathname)
{
 int result = SHCreateDirectoryEx((HWND)0, pathname, NULL);
 if((result == ERROR_SUCCESS) || (result == ERROR_ALREADY_EXISTS)) return true;
 return false;
}

boost::shared_array<TCHAR> GetModulePathname(void)
{
 // get filename
 TCHAR filename[MAX_PATH];
 GetModuleFileName(NULL, filename, MAX_PATH);

 // split path from filename
 TCHAR c_drive[MAX_PATH];
 TCHAR c_path[MAX_PATH];
 _tsplitpath(filename, c_drive, c_path, nullptr, nullptr);

 // compute length of 1st component
 size_t s1 = 0;
 if(FAILED(StringCchLength(c_drive, MAX_PATH, &s1)))
    return boost::shared_array<TCHAR>();

 // compute length of 2nd component
 size_t s2 = 0;
 if(FAILED(StringCchLength(c_path, MAX_PATH, &s2)))
    return boost::shared_array<TCHAR>();

 // zero length
 size_t len = s1 + s2 + 1;
 if(len == 1) return boost::shared_array<TCHAR>();

 // create string
 boost::shared_array<TCHAR> retval(new TCHAR[len]);
 retval[0] = static_cast<TCHAR>(0);

 // concatenate 1st component
 LPTSTR endptr = NULL;
 if(FAILED(StringCchCatEx(retval.get(), len, c_drive, &endptr, NULL, STRSAFE_NULL_ON_FAILURE)))
    return boost::shared_array<TCHAR>();

 // concatenate 2nd component
 if(FAILED(StringCchCat(endptr, len, c_path)))
    return boost::shared_array<TCHAR>();

 // return result
 return retval;
}

boost::shared_array<TCHAR> GetShortFilename(LPCTSTR filename)
{
 // compute length of filename
 size_t fnlen = 0;
 if(FAILED(StringCchLength(filename, MAX_PATH, &fnlen)))
    return boost::shared_array<TCHAR>();

 // must have a length
 if(!fnlen) return boost::shared_array<TCHAR>();

 // extract components
 TCHAR c_param1[MAX_PATH];
 TCHAR c_param2[MAX_PATH];
 TCHAR c_param3[MAX_PATH];
 TCHAR c_param4[MAX_PATH];

 // extract components
 c_param1[0] = static_cast<TCHAR>(0);
 c_param2[0] = static_cast<TCHAR>(0);
 c_param3[0] = static_cast<TCHAR>(0);
 c_param4[0] = static_cast<TCHAR>(0);
 _tsplitpath(filename, c_param1, c_param2, c_param3, c_param4); 

 // compute length of 1st component
 size_t s1 = 0;
 if(FAILED(StringCchLength(c_param3, MAX_PATH, &s1)))
    return boost::shared_array<TCHAR>();

 // compute length of 2nd component
 size_t s2 = 0;
 if(FAILED(StringCchLength(c_param4, MAX_PATH, &s2)))
    return boost::shared_array<TCHAR>();

 // zero length
 size_t len = s1 + s2;
 if(!len) return boost::shared_array<TCHAR>();

 // create string
 boost::shared_array<TCHAR> retval(new TCHAR[len + 1]);
 retval[0] = static_cast<TCHAR>(0);

 // concatenate 1st component
 LPTSTR endptr = NULL;
 if(FAILED(StringCchCatEx(retval.get(), (len + 1), c_param3, &endptr, NULL, STRSAFE_NO_TRUNCATION)))
    return boost::shared_array<TCHAR>();

 // concatenate 2nd component
 if(FAILED(StringCchCat(endptr, (len + 1), c_param4)))
    return boost::shared_array<TCHAR>();

 // return result
 return retval;
}

boost::shared_array<TCHAR> GetShortFilenameWithoutExtension(LPCTSTR filename)
{
 // compute length of filename
 size_t fnlen = 0;
 if(FAILED(StringCchLength(filename, MAX_PATH, &fnlen)))
    return boost::shared_array<TCHAR>();

 // must have a length
 if(!fnlen) return boost::shared_array<TCHAR>();

 // extract components
 TCHAR c_param1[MAX_PATH];
 TCHAR c_param2[MAX_PATH];
 TCHAR c_param3[MAX_PATH];
 TCHAR c_param4[MAX_PATH];

 // extract components
 c_param1[0] = static_cast<TCHAR>(0);
 c_param2[0] = static_cast<TCHAR>(0);
 c_param3[0] = static_cast<TCHAR>(0);
 c_param4[0] = static_cast<TCHAR>(0);
 _tsplitpath(filename, c_param1, c_param2, c_param3, c_param4); 

 // compute length component
 size_t len = 0;
 if(FAILED(StringCchLength(c_param3, MAX_PATH, &len))) return boost::shared_array<TCHAR>();
 if(!len) return boost::shared_array<TCHAR>();

 // create component
 boost::shared_array<TCHAR> retval(new TCHAR[len + 1]);
 retval[0] = static_cast<TCHAR>(0);

 // copy component
 LPTSTR endptr = NULL;
 if(FAILED(StringCchCopy(retval.get(), (len + 1), c_param3)))
    return boost::shared_array<TCHAR>();

 // return result
 return retval;
}

boost::shared_array<TCHAR> GetPathnameFromFilename(LPCTSTR filename)
{
 // compute length of filename
 size_t fnlen = 0;
 if(FAILED(StringCchLength(filename, MAX_PATH, &fnlen)))
    return boost::shared_array<TCHAR>();

 // must have a length
 if(!fnlen) return boost::shared_array<TCHAR>();

 // extract components
 TCHAR c_param1[MAX_PATH];
 TCHAR c_param2[MAX_PATH];
 TCHAR c_param3[MAX_PATH];
 TCHAR c_param4[MAX_PATH];

 // extract components
 c_param1[0] = static_cast<TCHAR>(0);
 c_param2[0] = static_cast<TCHAR>(0);
 c_param3[0] = static_cast<TCHAR>(0);
 c_param4[0] = static_cast<TCHAR>(0);
 _tsplitpath(filename, c_param1, c_param2, c_param3, c_param4); 

 // compute length of 1st component
 size_t s1 = 0;
 if(FAILED(StringCchLength(c_param1, MAX_PATH, &s1)))
    return boost::shared_array<TCHAR>();

 // compute length of 2nd component
 size_t s2 = 0;
 if(FAILED(StringCchLength(c_param2, MAX_PATH, &s2)))
    return boost::shared_array<TCHAR>();

 // zero length
 size_t len = s1 + s2;
 if(!len) return boost::shared_array<TCHAR>();

 // create string
 boost::shared_array<TCHAR> retval(new TCHAR[len + 1]);
 retval[0] = static_cast<TCHAR>(0);

 // concatenate 1st component
 LPTSTR endptr = NULL;
 if(FAILED(StringCchCatEx(retval.get(), (len + 1), c_param1, &endptr, NULL, STRSAFE_NO_TRUNCATION)))
    return boost::shared_array<TCHAR>();

 // concatenate 2nd component
 if(FAILED(StringCchCat(endptr, (len + 1), c_param2)))
    return boost::shared_array<TCHAR>();

 // return result
 return retval;
}

boost::shared_array<TCHAR> GetExtensionFromFilename(LPCTSTR filename)
{
 // compute length of filename
 size_t fnlen = 0;
 if(FAILED(StringCchLength(filename, MAX_PATH, &fnlen)))
    return boost::shared_array<TCHAR>();

 // must have a length
 if(!fnlen) return boost::shared_array<TCHAR>();

 // extract components
 TCHAR c_param1[MAX_PATH];
 TCHAR c_param2[MAX_PATH];
 TCHAR c_param3[MAX_PATH];
 TCHAR c_param4[MAX_PATH];

 // extract components
 c_param1[0] = static_cast<TCHAR>(0);
 c_param2[0] = static_cast<TCHAR>(0);
 c_param3[0] = static_cast<TCHAR>(0);
 c_param4[0] = static_cast<TCHAR>(0);
 _tsplitpath(filename, c_param1, c_param2, c_param3, c_param4); 

 // return extension
 return StringToSharedArray(c_param4);
}

boost::shared_array<TCHAR> ChangeFileExtension(LPCTSTR filename, LPCTSTR extension)
{
 // validate length
 size_t len = 0;
 if(FAILED(StringCchLength(filename, MAX_PATH, &len))) return boost::shared_array<TCHAR>();
 if(!len) return boost::shared_array<TCHAR>();

 // extract components
 TCHAR c_param1[MAX_PATH];
 TCHAR c_param2[MAX_PATH];
 TCHAR c_param3[MAX_PATH];
 TCHAR c_param4[MAX_PATH];

 // extract components
 c_param1[0] = 0;
 c_param2[0] = 0;
 c_param3[0] = 0;
 c_param4[0] = 0;
 _tsplitpath(filename, c_param1, c_param2, c_param3, c_param4); 

 // append name + extension
 STDSTRING retval(c_param1);
 retval += c_param2;
 retval += c_param3;
 retval += TEXT(".");
 retval += extension;

 // return extension
 return StringToSharedArray(retval.c_str());
}

BOOL HasExtension(LPCTSTR filename, LPCTSTR extension)
{
 return (_tcsicmp(GetExtensionFromFilename(filename).get(), extension) == 0);
}

BOOL FileExists(LPCTSTR filename)
{
 DWORD attrs = GetFileAttributes(filename);
 if(attrs == INVALID_FILE_ATTRIBUTES) return FALSE; // must succeed
 if(attrs & FILE_ATTRIBUTE_DIRECTORY) return FALSE; // must be a file
 return TRUE;
}

uint64 GetFileSize(std::ifstream& ifile)
{
 // save position
 auto position = ifile.tellg();
 if(ifile.fail()) return 0xFFFFFFFFFFFFFFFFull;

 // move to end
 ifile.seekg(0, std::ios::end);
 if(ifile.fail()) return 0xFFFFFFFFFFFFFFFFull;

 // save filesize
 auto filesize = ifile.tellg();
 if(ifile.fail()) return 0xFFFFFFFFFFFFFFFFull;

 // return to position
 ifile.seekg(position);
 if(ifile.fail()) return 0xFFFFFFFFFFFFFFFFull;

 // return filesize
 return (uint64)filesize;
}

bool BuildNonRecursiveFilenameList(std::deque<STDSTRING>& namelist, LPCTSTR fext, LPCTSTR rootname)
{
 // clear namelist
 namelist.clear();

 // add "." to extension (if necessary)
 STDSTRING extension;
 if(fext[0] != '.') extension += TEXT(".");
 extension += fext;

 // split the pathname
 TCHAR c_drive[1024];
 TCHAR c_dir[1024];
 _tsplitpath(rootname, c_drive, c_dir, nullptr, nullptr);

 // construct path
 STDSTRING path(c_drive);
 path += STDSTRING(c_dir);
 path += TEXT("*");
 path += extension;

 // add files
 find_file file;
 if(file.find(path.c_str())) {
    STDSTRING temp(path);
    temp += file.filename();
    if(!file.is_directory())
       if(!_tcsicmp(extension.c_str(), GetExtensionFromFilename(temp.c_str()).get()))
          namelist.insert(namelist.end(), temp);
   }
 while(file.next()) {
    STDSTRING temp(path);
    temp += file.filename();
    if(!file.is_directory())
       if(!_tcsicmp(extension.c_str(), GetExtensionFromFilename(temp.c_str()).get()))
          namelist.insert(namelist.end(), temp);
   }

 // sort list
 std::sort(namelist.begin(), namelist.end());
 return true;
}

bool BuildFilenameList(std::deque<STDSTRING>& namelist, LPCTSTR fext)
{
 // get path of application
 TCHAR rootname[MAX_PATH];
 GetModuleFileName(0, rootname, MAX_PATH);

 // split the pathname
 TCHAR c_drive[1024];
 TCHAR c_dir[1024];
 _tsplitpath(rootname, c_drive, c_dir, nullptr, nullptr);

 // build pathname
 STDSTRING pathname(c_drive);
 pathname += STDSTRING(c_dir);

 // build directory list
 std::deque<STDSTRING> templist;
 templist.insert(templist.end(), pathname);

 size_t i = 0;
 for(;;)
    {
     // get wildcard
     find_file file;
     STDSTRING path(templist[i]);
     path += TEXT("*.*");
     file.find(path.c_str());
     if(!file) return error("Could not build wildcard path.");

     // insert directories
     if(file.is_directory() && !file.is_dots() && !file.is_system()) {
        STDSTRING path(templist[i]);
        path += STDSTRING(file.filename());
        path += STDSTRING(TEXT("\\"));
        templist.insert(templist.end(), path);
       }

     while(file.next()) {
        if(file.is_directory() && !file.is_dots() && !file.is_system()) {
           STDSTRING path(templist[i]);
           path += STDSTRING(file.filename());
           path += STDSTRING(TEXT("\\"));
           templist.insert(templist.end(), path);
          }
       }

     file.close();

     // update index
     if(++i == templist.size())
        break;
    }

 // transfer directories with EXT files
 std::deque<STDSTRING> rootlist;
 for(size_t i = 0; i < templist.size(); i++) {
     find_file file;
     bool added = false;
     STDSTRING path(templist[i]);
     path += TEXT("*");
     path += TEXT(".");
     path += fext;
     if(file.find(path.c_str()) && !file.is_directory()) {
        rootlist.insert(rootlist.end(), templist[i]);
        added = true;
       }
    }

 // erase the temporary list
 templist.erase(templist.begin(), templist.end());
 if(rootlist.empty()) return error("No files found in current directory or subdirectories.");

 // build namelist
 namelist.clear();
 for(size_t i = 0; i < rootlist.size(); i++)
    {
     // clear templist
     find_file file;
     templist.erase(templist.begin(), templist.end());

     // add EXT files
     STDSTRING path(rootlist[i]);
     path += TEXT("*");
     path += TEXT(".");
     path += fext;
     if(file.find(path.c_str())) {
        STDSTRING temp(rootlist[i]);
        temp += file.filename();
        templist.insert(templist.end(), temp);
       }
     while(file.next()) {
        STDSTRING temp(rootlist[i]);
        temp += file.filename();
        templist.insert(templist.end(), temp);
       }

     // sort temporary list and append to the namelist
     if(templist.size()) {
        std::sort(templist.begin(), templist.end());
        namelist.insert(namelist.end(), templist.begin(), templist.end());
       }
    }

 // must have something in namelist
 templist.erase(templist.begin(), templist.end());
 if(!namelist.size()) return error("No files found in root directory or subdirectories.");

 return true;
}

bool BuildFilenameList(std::deque<STDSTRING>& namelist, LPCTSTR fext, LPCTSTR rootname)
{
 // add "." to extension
 STDSTRING extension;
 if(fext[0] != '.') extension += TEXT(".");
 extension += fext;

 // split the pathname
 TCHAR c_drive[1024];
 TCHAR c_dir[1024];
 _tsplitpath(rootname, c_drive, c_dir, nullptr, nullptr);

 // construct path
 STDSTRING path(c_drive);
 path += STDSTRING(c_dir);

 // build directory list
 std::deque<STDSTRING> templist;
 templist.insert(templist.end(), path);

 size_t i = 0;
 for(;;)
    {
     // get wildcard
     find_file file;
     STDSTRING item(templist[i]);
     item += TEXT("*.*");
     file.find(item.c_str());
     if(!file) return error("Could not build wildcard path.");

     // insert directories
     if(file.is_directory() && !file.is_dots() && !file.is_system()) {
        STDSTRING item(templist[i]);
        item += STDSTRING(file.filename());
        item += STDSTRING(TEXT("\\"));
        templist.insert(templist.end(), item);
       }

     while(file.next()) {
        if(file.is_directory() && !file.is_dots() && !file.is_system()) {
           STDSTRING item(templist[i]);
           item += STDSTRING(file.filename());
           item += STDSTRING(TEXT("\\"));
           templist.insert(templist.end(), item);
          }
       }

     file.close();

     // update index
     if(++i == templist.size())
        break;
    }

 // transfer directories with EXT files
 std::deque<STDSTRING> rootlist;
 for(size_t i = 0; i < templist.size(); i++)
    {
     find_file file;
     bool added = false;
     STDSTRING item(templist[i]);
     item += TEXT("*");
     item += extension;
     if(file.find(item.c_str()) && !file.is_directory()) {
        rootlist.insert(rootlist.end(), templist[i]);
        added = true;
       }
    }

 // erase the temporary list
 templist.erase(templist.begin(), templist.end());
 if(rootlist.empty()) return true;

 // build namelist
 namelist.clear();
 for(size_t i = 0; i < rootlist.size(); i++)
    {
     // clear templist
     find_file file;
     templist.erase(templist.begin(), templist.end());

     // add files
     STDSTRING item(rootlist[i]);
     item += TEXT("*");
     item += extension;
     if(file.find(item.c_str())) {
        STDSTRING temp(rootlist[i]);
        temp += file.filename();
        if(!file.is_directory())
           if(!_tcsicmp(extension.c_str(), GetExtensionFromFilename(temp.c_str()).get()))
              templist.insert(templist.end(), temp);
       }
     while(file.next()) {
        STDSTRING temp(rootlist[i]);
        temp += file.filename();
        if(!file.is_directory())
           if(!_tcsicmp(extension.c_str(), GetExtensionFromFilename(temp.c_str()).get()))
              templist.insert(templist.end(), temp);
       }

     // sort temporary list and append to the namelist
     if(templist.size()) {
        std::sort(templist.begin(), templist.end());
        namelist.insert(namelist.end(), templist.begin(), templist.end());
       }
    }

 // must have something in namelist
 templist.erase(templist.begin(), templist.end());
 if(!namelist.size()) return true;

 return true;
}

bool BuildWildcardFilenameList(std::deque<STDSTRING>& namelist)
{
 // get path of application
 TCHAR rootname[MAX_PATH];
 GetModuleFileName(0, rootname, MAX_PATH);

 // split the pathname
 TCHAR c_drive[1024];
 TCHAR c_dir[1024];
 _tsplitpath(rootname, c_drive, c_dir, nullptr, nullptr);

 // build pathname
 STDSTRING pathname(c_drive);
 pathname += STDSTRING(c_dir);

 return BuildWildcardFilenameList(namelist, pathname.c_str());
}

bool BuildWildcardFilenameList(std::deque<STDSTRING>& namelist, LPCTSTR rootname)
{
 // split the pathname
 TCHAR c_drive[1024];
 TCHAR c_dir[1024];
 _tsplitpath(rootname, c_drive, c_dir, nullptr, nullptr);

 // construct path
 STDSTRING path(c_drive);
 path += STDSTRING(c_dir);

 // build directory list
 std::deque<STDSTRING> templist;
 templist.insert(templist.end(), path);

 size_t i = 0;
 for(;;)
    {
     // get wildcard
     find_file file;
     STDSTRING item(templist[i]);
     item += TEXT("*.*");
     file.find(item.c_str());
     if(!file) return error("Could not build wildcard path.");

     // insert directories
     if(file.is_directory() && !file.is_dots() && !file.is_system()) {
        STDSTRING item(templist[i]);
        item += STDSTRING(file.filename());
        item += STDSTRING(TEXT("\\"));
        templist.insert(templist.end(), item);
       }

     while(file.next()) {
        if(file.is_directory() && !file.is_dots() && !file.is_system()) {
           STDSTRING item(templist[i]);
           item += STDSTRING(file.filename());
           item += STDSTRING(TEXT("\\"));
           templist.insert(templist.end(), item);
          }
       }

     file.close();

     // update index
     if(++i == templist.size())
        break;
    }

 // transfer directories with at least one file
 std::deque<STDSTRING> rootlist;
 for(size_t i = 0; i < templist.size(); i++)
    {
     // construct wildcard
     STDSTRING item(templist[i]);
     item += TEXT("*.*");

     // find first item
     find_file file;
     file.find(item.c_str());
     if(!file) return error("Could not build wildcard path.");

     // test first item
     bool is_folder = file.is_directory() || file.is_dots();
     if(!is_folder) {
        rootlist.insert(rootlist.end(), templist[i]);
        continue;
       }

     // test other items
     while(file.next()) {
        is_folder = file.is_directory() || file.is_dots();
        if(!is_folder) {
           rootlist.insert(rootlist.end(), templist[i]);
           break;
          }
       }
    }

 // erase the temporary list
 templist.erase(templist.begin(), templist.end());
 if(rootlist.empty()) return true;

 // build namelist
 namelist.clear();
 for(size_t i = 0; i < rootlist.size(); i++)
    {
     // clear templist
     find_file file;
     templist.erase(templist.begin(), templist.end());

     // add files
     STDSTRING item(rootlist[i]);
     item += TEXT("*.*");
     if(file.find(item.c_str())) {
        STDSTRING temp(rootlist[i]);
        temp += file.filename();
        if(!(file.is_directory() || file.is_dots())) templist.insert(templist.end(), temp);
       }
     while(file.next()) {
        STDSTRING temp(rootlist[i]);
        temp += file.filename();
        if(!(file.is_directory() || file.is_dots())) templist.insert(templist.end(), temp);
       }

     // sort temporary list and append to the namelist
     if(templist.size()) {
        std::sort(templist.begin(), templist.end());
        namelist.insert(namelist.end(), templist.begin(), templist.end());
       }
    }

 // must have something in namelist
 templist.erase(templist.begin(), templist.end());
 if(!namelist.size()) return true;

 return true;
}

bool DeleteFilesIncludingExtensions(const std::set<STDSTRING>& filelist)
{
 return DeleteFilesIncludingExtensions(GetModulePathname().get(), filelist);
}

bool DeleteFilesExcludingExtensions(const std::set<STDSTRING>& filelist)
{
 return DeleteFilesExcludingExtensions(GetModulePathname().get(), filelist);
}

bool DeleteFilesIncludingExtensions(LPCTSTR rootname, const std::set<STDSTRING>& filelist)
{
 return true;
}

struct file_extension_compare : public std::binary_function<STDSTRING, STDSTRING, bool> {
 bool operator ()(const STDSTRING& arg1, const STDSTRING& arg2)const {
  size_t size1 = _tcslen(arg1.c_str());
  size_t size2 = _tcslen(arg2.c_str());
  LPCTSTR temp1 = (((size1 > 0) && (arg1[0] == TEXT('.'))) ? arg1.c_str() + 1 : arg1.c_str());
  LPCTSTR temp2 = (((size2 > 0) && (arg2[0] == TEXT('.'))) ? arg2.c_str() + 1 : arg2.c_str());
  return (_tcsicmp(temp1, temp2) < 0);
 }
};

bool DeleteFilesExcludingExtensions(LPCTSTR rootname, const std::set<STDSTRING>& filelist)
{
 // validate
 if(!rootname) return false;
 if(!filelist.size()) return false;

 // build a modified set of extensions
 std::set<STDSTRING, file_extension_compare> extension_set;
 for(auto i = filelist.begin(); i != filelist.end(); i++) {
     if((*i).length() > 0) extension_set.insert(*i);
    }

 // split the pathname
 TCHAR c_drive[1024];
 TCHAR c_dir[1024];
 _tsplitpath(rootname, c_drive, c_dir, nullptr, nullptr);

 // construct path
 STDSTRING path(c_drive);
 path += STDSTRING(c_dir);

 // build directory list
 std::deque<STDSTRING> templist;
 templist.insert(templist.end(), path);

 size_t i = 0;
 for(;;)
    {
     // get wildcard
     find_file file;
     STDSTRING item(templist[i]);
     item += TEXT("*.*");
     file.find(item.c_str());
     if(!file) return error("Could not build wildcard path.");

     // insert directories
     if(file.is_directory() && !file.is_dots() && !file.is_system()) {
        STDSTRING item(templist[i]);
        item += STDSTRING(file.filename());
        item += STDSTRING(TEXT("\\"));
        templist.insert(templist.end(), item);
       }

     while(file.next()) {
        if(file.is_directory() && !file.is_dots() && !file.is_system()) {
           STDSTRING item(templist[i]);
           item += STDSTRING(file.filename());
           item += STDSTRING(TEXT("\\"));
           templist.insert(templist.end(), item);
          }
       }

     file.close();

     // update index
     if(++i == templist.size())
        break;
    }

 // transfer directories with at least one file
 std::deque<STDSTRING> rootlist;
 for(size_t i = 0; i < templist.size(); i++)
    {
     // construct wildcard
     STDSTRING item(templist[i]);
     item += TEXT("*.*");

     // find first item
     find_file file;
     file.find(item.c_str());
     if(!file) return error("Could not build wildcard path.");

     // test first item
     bool is_folder = file.is_directory() || file.is_dots();
     if(!is_folder) {
        rootlist.insert(rootlist.end(), templist[i]);
        continue;
       }

     // test other items
     while(file.next()) {
        is_folder = file.is_directory() || file.is_dots();
        if(!is_folder) {
           rootlist.insert(rootlist.end(), templist[i]);
           break;
          }
       }
    }

 // erase the temporary list
 templist.erase(templist.begin(), templist.end());
 if(rootlist.empty()) return true;

 // build namelist
 for(size_t i = 0; i < rootlist.size(); i++)
    {
     // clear temporary list
     templist.clear();

     // find first file
     STDSTRING item(rootlist[i]);
     item += TEXT("*.*");
     find_file file;
     if(file.find(item.c_str())) {
        STDSTRING temp(rootlist[i]);
        temp += file.filename();
        bool is_folder = file.is_directory() || file.is_dots();
        if(!is_folder) templist.insert(templist.end(), temp);
       }

     // find other files
     while(file.next()) {
        STDSTRING temp(rootlist[i]);
        temp += file.filename();
        bool is_folder = file.is_directory() || file.is_dots();
        if(!is_folder) templist.insert(templist.end(), temp);
       }

     // close search handle
     file.close();

     // sort temporary list and delete files that are not found in extension list
     std::sort(templist.begin(), templist.end());
     for(size_t j = 0; j < templist.size(); j++)
       {
        auto ptr = GetExtensionFromFilename(templist[j].c_str()).get();
        STDSTRING extension((ptr ? ptr : TEXT("")));
        if(extension_set.find(extension) == extension_set.end()) {
           std::wcout << "Deleting " << templist[j].c_str() << std::endl;
           DeleteFile(templist[j].c_str());
          }
       }
    }

 return true;
}

bool DeleteEmptyFolders(LPCTSTR rootname)
{
 // split the pathname
 TCHAR c_drive[1024];
 TCHAR c_dir[1024];
 _tsplitpath(rootname, c_drive, c_dir, nullptr, nullptr);

 // construct path
 STDSTRING path(c_drive);
 path += STDSTRING(c_dir);

 // build directory list
 std::deque<STDSTRING> templist;
 templist.insert(templist.end(), path);

 size_t i = 0;
 for(;;)
    {
     // get wildcard
     find_file file;
     STDSTRING item(templist[i]);
     item += TEXT("*.*");
     file.find(item.c_str());
     if(!file) return error("Could not build wildcard path.");

     // insert directories
     if(file.is_directory() && !file.is_dots() && !file.is_system()) {
        STDSTRING item(templist[i]);
        item += STDSTRING(file.filename());
        item += STDSTRING(TEXT("\\"));
        templist.insert(templist.end(), item);
       }

     while(file.next()) {
        if(file.is_directory() && !file.is_dots() && !file.is_system()) {
           STDSTRING item(templist[i]);
           item += STDSTRING(file.filename());
           item += STDSTRING(TEXT("\\"));
           templist.insert(templist.end(), item);
          }
       }

     file.close();

     // update index
     if(++i == templist.size())
        break;
    }

 // in reverse order delete empty directories
 for(size_t i = 0; i < templist.size(); i++) {
     const STDSTRING& folder = templist[templist.size() - i - 1];
     if(PathIsDirectoryEmpty(folder.c_str()))
        if(RemoveDirectory(folder.c_str())) std::wcout << "Removed " << folder.c_str() << std::endl;
    }

 return true;
}

//
// FILE TYPE SEARCHING
//

bool GetSubdirectoryList(std::deque<STDSTRING>& namelist, LPCTSTR rootname)
{
 // split the pathname
 TCHAR c_drive[1024];
 TCHAR c_dir[1024];
 _tsplitpath(rootname, c_drive, c_dir, nullptr, nullptr);

 // build pathname
 STDSTRING path(c_drive);
 path += STDSTRING(c_dir);

 // build wildcard
 find_file file;
 STDSTRING wildcard = path;
 wildcard += TEXT("*.*");
 file.find(wildcard.c_str());
 if(!file) return error("Could not build wildcard path.");

 // build directory list under root
 if(file.is_directory() && !file.is_dots() && !file.is_system()) {
    STDSTRING temp(path);
    temp += STDSTRING(file.filename());
    temp += STDSTRING(TEXT("\\"));
    namelist.push_back(temp);
   }
 while(file.next()) {
    if(file.is_directory() && !file.is_dots() && !file.is_system()) {
       STDSTRING temp(path);
       temp += STDSTRING(file.filename());
       temp += STDSTRING(TEXT("\\"));
       namelist.push_back(temp);
      }
   }

 return true;
}

bool SearchFolderForFileType(const STDSTRING& rootname, const STDSTRING& fext)
{
 std::deque<STDSTRING> namelist;
 BuildFilenameList(namelist, fext.c_str(), rootname.c_str());
 return (namelist.size() != 0);
}

bool SearchFolderForFileType(const STDSTRING& rootname, const std::deque<STDSTRING>& fext)
{
 for(size_t i = 0; i < fext.size(); i++) if(SearchFolderForFileType(rootname, fext[i])) return true;
 return false;
}

//
// BATCH EXTENSION RENAMING
//

bool BatchFileExtensionRename(const STDSTRING& rootname, const STDSTRING& org, const STDSTRING& rep, bool progress)
{
 // build list of filename
 if(progress) std::wcout << "Renaming ." << org << " to ." << rep << "." << std::endl;
 std::deque<STDSTRING> filelist;
 BuildFilenameList(filelist, org.c_str(), rootname.c_str());
 if(!filelist.size()) return true;

 // sort and rename
 std::sort(filelist.begin(), filelist.end());
 for(size_t i = 0; i < filelist.size(); i++) {
     std::wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << std::endl;
     LPCTSTR filename = ChangeFileExtension(filelist[i].c_str(), rep.c_str()).get();
     MoveFile(filelist[i].c_str(), filename);
    }

 return true;
}

bool SearchFileForSignature(std::ifstream& ifile, const char* signature, size_t sigsize)
{
 // validation
 if(!ifile.is_open()) return false;
 if(!signature || !sigsize) return false;

 // get filesize
 ifile.seekg(0, std::ios::end);
 uint64 filesize = ifile.tellg();
 ifile.seekg(0, std::ios::beg);

 // read buffer properties
 const uint32 maxbuffersize = 64*1024*1024;
 uint32 buffersize = (filesize < (uint64)maxbuffersize ? (uint32)filesize : maxbuffersize);
 boost::shared_array<char> buffer(new char[buffersize]);

 // boyer-moore-horspool properties
 boost::shared_array<unsigned char> table(new unsigned char[256]);
 for(uint32 i = 0; i < 256; i++) table[i] = sigsize;
 uint32 p_len = sigsize;
 const unsigned char* p_str = reinterpret_cast<const unsigned char*>(signature);
 for(uint32 i = 0; i < p_len - 1; i++) table[p_str[i]] = p_len - i - 1;

 // search properties
 uint64 bytes_read = 0;
 uint64 bytes_left = filesize;
 uint64 start = 0;

 // search
 while(bytes_read < filesize)
      {
       // search partial buffer
       if(bytes_left < buffersize)
         {
          // read data
          ifile.read(buffer.get(), bytes_left);
          if(ifile.fail()) return false;

          // boyer-moore-horspool properties
          uint32 s_len = (uint32)bytes_left;
          const unsigned char* s_str = reinterpret_cast<const unsigned char*>(buffer.get());          

          // boyer-moore-horspool search
          size_t j = 0;
          while(j <= (s_len - p_len)) {
                unsigned char c = s_str[j + p_len - 1];
                if(p_str[p_len - 1] == c && memcmp(p_str, s_str + j, p_len - 1) == 0) return true;
                j += table[c];
               }

          // adjust search properties
          bytes_read = bytes_read + bytes_left;
          bytes_left = 0;
         }
       // search full buffer
       else
         {
          // read data
          ifile.read(buffer.get(), buffersize);
          if(ifile.fail()) return false;

          // boyer-moore-horspool properties
          size_t s_len = buffersize;
          const unsigned char* s_str = reinterpret_cast<const unsigned char*>(buffer.get());          

          // boyer-moore-horspool search
          size_t j = 0;
          while(j <= (s_len - p_len)) {
                unsigned char c = s_str[j + p_len - 1];
                if(p_str[p_len - 1] == c && memcmp(p_str, s_str + j, p_len - 1) == 0) return true;
                j += table[c];
               }

          // adjust search properties
          bytes_read = bytes_read + buffersize;
          bytes_left = bytes_left - buffersize;
         }
      }

 // not found
 return false;
}

bool SearchFileForSignature(std::ifstream& ifile, const char* signature, size_t sigsize, std::deque<uint64>& offsets)
{
 // validation
 if(!ifile.is_open()) return false;
 if(!signature || !sigsize) return false;
 if(offsets.size()) offsets.clear();

 // get filesize
 ifile.seekg(0, std::ios::end);
 uint64 filesize = ifile.tellg();
 ifile.seekg(0, std::ios::beg);

 // read buffer properties
 const uint32 maxbuffersize = 32*1024*1024;
 uint32 buffersize = (filesize < (uint64)maxbuffersize ? (uint32)filesize : maxbuffersize);
 boost::shared_array<char> buffer(new char[buffersize]);

 // boyer-moore-horspool properties
 boost::shared_array<unsigned char> table(new unsigned char[256]);
 for(uint32 i = 0; i < 256; i++) table[i] = sigsize;
 uint32 p_len = sigsize;
 const unsigned char* p_str = reinterpret_cast<const unsigned char*>(signature);
 for(uint32 i = 0; i < p_len - 1; i++) table[p_str[i]] = p_len - i - 1;

 // search properties
 uint64 bytes_read = 0;
 uint64 bytes_left = filesize;
 uint64 start = 0;

 // search
 while(bytes_read < filesize)
      {
       // search partial buffer
       if(bytes_left < buffersize)
         {
          // read data
          ifile.read(buffer.get(), bytes_left);
          if(ifile.fail()) return false;

          // resume search?

          // boyer-moore-horspool properties
          uint32 s_len = (uint32)bytes_left;
          const unsigned char* s_str = reinterpret_cast<const unsigned char*>(buffer.get());          

          // boyer-moore-horspool search
          size_t j = 0;
          while(j <= (s_len - p_len)) {
                unsigned char c = s_str[j + p_len - 1];
                if(p_str[p_len - 1] == c && memcmp(p_str, s_str + j, p_len - 1) == 0) offsets.push_back(bytes_read + j);
                j += table[c];
               }

          // adjust search properties
          bytes_read = bytes_read + bytes_left;
          bytes_left = 0;
         }
       // search full buffer
       else
         {
          // read data
          ifile.read(buffer.get(), buffersize);
          if(ifile.fail()) return false;

          // resume search?

          // boyer-moore-horspool properties
          size_t s_len = buffersize;
          const unsigned char* s_str = reinterpret_cast<const unsigned char*>(buffer.get());          

          // boyer-moore-horspool search
          size_t j = 0;
          while(j <= (s_len - p_len)) {
                unsigned char c = s_str[j + p_len - 1];
                if(p_str[p_len - 1] == c && memcmp(p_str, s_str + j, p_len - 1) == 0) offsets.push_back(bytes_read + j);
                j += table[c];
               }

          // adjust search properties
          bytes_read = bytes_read + buffersize;
          bytes_left = bytes_left - buffersize;
         }
      }

 return true;
}

bool SearchFileForSignature(std::ifstream& ifile, const char* signature, size_t sigsize, size_t alignment, std::deque<uint64>& offsets)
{
 // validation
 if(!ifile.is_open()) return false;
 if(!signature || !sigsize) return false;
 if(offsets.size()) offsets.clear();

 // get filesize
 ifile.seekg(0, std::ios::end);
 uint64 filesize = ifile.tellg();
 ifile.seekg(0, std::ios::beg);

 // read buffer properties
 const uint32 maxbuffersize = 32*1024*1024;
 uint32 buffersize = (filesize < (uint64)maxbuffersize ? (uint32)filesize : maxbuffersize);
 boost::shared_array<char> buffer(new char[buffersize]);

 // boyer-moore-horspool properties
 boost::shared_array<unsigned char> table(new unsigned char[256]);
 for(uint32 i = 0; i < 256; i++) table[i] = sigsize;
 uint32 p_len = sigsize;
 const unsigned char* p_str = reinterpret_cast<const unsigned char*>(signature);
 for(uint32 i = 0; i < p_len - 1; i++) table[p_str[i]] = p_len - i - 1;

 // search properties
 uint64 bytes_read = 0;
 uint64 bytes_left = filesize;
 uint64 start = 0;

 // search
 while(bytes_read < filesize)
      {
       // search partial buffer
       if(bytes_left < buffersize)
         {
          // read data
          ifile.read(buffer.get(), bytes_left);
          if(ifile.fail()) return false;

          // resume search?

          // boyer-moore-horspool properties
          uint32 s_len = (uint32)bytes_left;
          const unsigned char* s_str = reinterpret_cast<const unsigned char*>(buffer.get());          

          // boyer-moore-horspool search
          size_t j = 0;
          while(j <= (s_len - p_len)) {
                unsigned char c = s_str[j + p_len - 1];
                if(p_str[p_len - 1] == c && memcmp(p_str, s_str + j, p_len - 1) == 0)
                   if(!((bytes_read + j) % alignment)) offsets.push_back(bytes_read + j);
                j += table[c];
               }

          // adjust search properties
          bytes_read = bytes_read + bytes_left;
          bytes_left = 0;
         }
       // search full buffer
       else
         {
          // read data
          ifile.read(buffer.get(), buffersize);
          if(ifile.fail()) return false;

          // resume search?

          // boyer-moore-horspool properties
          size_t s_len = buffersize;
          const unsigned char* s_str = reinterpret_cast<const unsigned char*>(buffer.get());          

          // boyer-moore-horspool search
          size_t j = 0;
          while(j <= (s_len - p_len)) {
                unsigned char c = s_str[j + p_len - 1];
                if(p_str[p_len - 1] == c && memcmp(p_str, s_str + j, p_len - 1) == 0)
                  if(!((bytes_read + j) % alignment)) offsets.push_back(bytes_read + j);
                j += table[c];
               }

          // adjust search properties
          bytes_read = bytes_read + buffersize;
          bytes_left = bytes_left - buffersize;
         }
      }

 return true;
}

//
// FILE MERGING FUNCTIONS
//

bool MergeBinaryFiles(const std::deque<STDSTRING>& filelist, const STDSTRING& filename)
{
 // must have at least one file
 using namespace std;
 if(!filelist.size()) return false;

 // create output file
 ofstream ofile(filename.c_str(), ios::binary);
 if(!ofile) return false;

 // transfer data
 for(uint32 i = 0; i < filelist.size(); i++)
    {
     // open file
     ifstream ifile(filelist[i].c_str(), ios::binary);
     if(!ifile) return false;

     // determine filesize
     uint64 filesize = 0;
     ifile.seekg(0, ios::end);
     filesize = (uint64)ifile.tellg();
     ifile.seekg(0, ios::beg);

     // keep track of saved bytes
     uint64 bytes_remaining = filesize;
     if(!bytes_remaining) continue;

     // save data in 16 MB chunks
     const uint64 CHUNKSIZE = 1000000ull;
     boost::shared_array<char> data(new char[CHUNKSIZE]);

     // save chunks
     do {
         // read chunk
         uint64 datasize = (CHUNKSIZE < bytes_remaining ? CHUNKSIZE : bytes_remaining);
         ifile.read(data.get(), datasize);
         if(ifile.fail()) return false; // read failure shouldn't happen

         // save chunk
         ofile.write(data.get(), datasize);
         if(ofile.fail()) return false; // save failure shouldn't happen

         // subtract
         bytes_remaining -= datasize;
        }
     while(bytes_remaining != 0);
    }

 return true;
}