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
#include "[PC] Berserk Musou.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   BERSERK_MUSOU

#pragma region "TITLE_INFORMATION"

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Berserk Musou");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Copy LINKDATA files from game into a new folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where the BIN files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~40 GB of free space.\n");
 p3 += TEXT("3. 12 GB for game + 28 GB for extraction.\n");

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
 bool doLNK = true;
 bool doBIN = true;
 bool doOUT = true;
 bool doG1T = true;
 bool doG1M = true;
 bool doDEL = false;

 // questions
 if(doLNK) doLNK = YesNoBox("Process LINKDATA files?\nSay 'No' if you have already done this before.");
 if(doBIN) doBIN = YesNoBox("Process DLC BIN files?\nSay 'No' if you have already done this before.");
 if(doOUT) doOUT = YesNoBox("Process IDXOUT files?\nSay 'No' if you have already done this before.");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");
 if(doDEL) doDEL = YesNoBox("Delete empty folders, subfolders and other useless files?\nSay 'No' if you don't trust me or if 'c:/Windows/ was your path lol.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 //
 // TESTING
 //

 // unpack BIN files
 if(doLNK) {
    // A
    cout << "Processing LINKDATA files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".A"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_A(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
    // B
    filelist.clear();
    BuildFilenameList(filelist, TEXT(".B"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_A(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
    // C
    filelist.clear();
    BuildFilenameList(filelist, TEXT(".C"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_A(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
    // D
    filelist.clear();
    BuildFilenameList(filelist, TEXT(".D"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_A(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
       }
    cout << endl;
   }

 // unpack BIN files
 if(doBIN) {
    cout << "Processing DLC BIN files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BIN"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_C(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // recursive unpacking
 if(doOUT)
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
    filelist.insert(TEXT("png"));
    filelist.insert(TEXT("dds"));
    filelist.insert(TEXT("dnk"));
    filelist.insert(TEXT("obj"));
    filelist.insert(TEXT("mtl"));
    filelist.insert(TEXT("smc"));
    filelist.insert(TEXT("g1m"));
    filelist.insert(TEXT("g1e"));
    filelist.insert(TEXT("g1t"));
    filelist.insert(TEXT("g1a"));
    filelist.insert(TEXT("g2a"));
    filelist.insert(TEXT("g1co"));
    filelist.insert(TEXT("g1em"));
    filelist.insert(TEXT("g1ct"));
    filelist.insert(TEXT("g1mx"));
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