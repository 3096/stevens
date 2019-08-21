#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_dw_ps3.h"
#include "x_dw_arc.h"
#include "x_dw_g1t.h"
#include "x_dw_g1m.h"
#include "pc_oppw3.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   OPPW3

// TODO: PROMOTE TO X_DW_ARCH.H
bool RenameLinkDataABCFile(LPCTSTR pathname, LPCTSTR letters)
{
 for(size_t letter_index = 0; letter_index < _tcsclen(letters); letter_index++)
    {
     // build extension
     STDSTRINGSTREAM ss;
     ss << TEXT(".") << letters[letter_index];

     // find files
     deque<STDSTRING> filelist;
     BuildFilenameList(filelist, ss.str().c_str(), pathname);

     // rename files
     for(size_t i = 0; i < filelist.size(); i++)
        {
         // filename properties
         STDSTRING pathname = GetPathnameFromFilename(filelist[i].c_str()).get();
         STDSTRING shrtname = GetShortFilenameWithoutExtension(filelist[i].c_str()).get();
    
         // change extension
         STDSTRINGSTREAM ss;
         ss << pathname.c_str() << shrtname.c_str();
         ss << TEXT("_");
         ss << letters[letter_index];
         ss << TEXT(".LD");

         // rename file
         wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
         wcout << TEXT("Renaming to ") << ss.str().c_str() << endl;
         MoveFileEx(filelist[i].c_str(), ss.str().c_str(), MOVEFILE_REPLACE_EXISTING);
        }
     cout << endl;
    }

 return true;
}

#pragma region "TITLE_INFORMATION"

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] One Piece Pirate Warriors 3");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where LINKDATA files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~19 GB of free space.\n");
 p3 += TEXT("3. 8 GB for game + 11 GB extraction.\n");

 // insert title
 AddGameTitle(p1.c_str(), p2.c_str(), p3.c_str(), extract);
 return true;
}

bool extract(void)
{
 LPCTSTR pathname = GetModulePathname().get();
 return extract(pathname);
}

bool extract(LPCTSTR pname)
{
 // set pathname
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // booleans
 bool doBIN = true;
 bool doG1T = true;
 bool doG1M = true;
 bool doDEL = true;

 // questions
 if(doBIN) doBIN = YesNoBox("Unpack archive files?\nSay 'No' if you have already done this before.");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?\nSay 'No' if you have already done this before.");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?\nSay 'No' if you have already done this before.");
 if(doDEL) doDEL = YesNoBox("Delete empty folders, subfolders and other useless files?\nSay 'No' if you don't trust me or if 'c:/Windows/ was your path lol.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack BIN/DLC files
 if(doBIN)
   {
    // process BIN files
    cout << "Processing BIN files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BIN"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::ARC7_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
   }

 // unpack LINKDATA files
 if(doBIN)
   {
    cout << "Processing LD files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".A"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_A(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
    filelist.clear();
    BuildFilenameList(filelist, TEXT(".B"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_A(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
    filelist.clear();
    BuildFilenameList(filelist, TEXT(".C"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_A(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
    filelist.clear();
    BuildFilenameList(filelist, TEXT(".D"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_A(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
   }

 // recursive unpacking
 if(doBIN)
    DYNASTY_WARRIORS::IDXOUT_PathExtract(pathname.c_str(), ENDIAN_LITTLE);

 // texture extraction
 if(doG1T)
    DYNASTY_WARRIORS::G1T_PathExtract(pathname.c_str());

 // model extraction
 if(doG1M) PS3::DYNASTY_WARRIORS::UnpackModels(pathname.c_str());

 // cleanup phase
 if(doDEL) {
    std::set<STDSTRING> filelist;
    filelist.insert(TEXT("txt"));
    filelist.insert(TEXT("dds"));
    filelist.insert(TEXT("obj"));
    filelist.insert(TEXT("mtl"));
    filelist.insert(TEXT("smc"));
    filelist.insert(TEXT("g1m"));
    filelist.insert(TEXT("g1e"));
    filelist.insert(TEXT("g1t"));
    DeleteFilesExcludingExtensions(pathname.c_str(), filelist);
    DeleteEmptyFolders(pathname.c_str());
   }

 // record end time
 uint64 t1 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t1);

 // determine time
 uint64 hz;
 QueryPerformanceFrequency((LARGE_INTEGER*)&hz);
 uint64 dt = (t1 - t0);
 real64 dt_sec = (real64)(t1 - t0)/(real64)hz;
 real64 dt_min = dt_sec/60.0;
 real64 dt_hrs = dt_min/60.0;
 cout << "Elapsed time = " << dt_min << " minutes." << endl;

 return true;
}

}}

#pragma endregion