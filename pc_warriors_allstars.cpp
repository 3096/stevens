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
#include "pc_warriors_allstars.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   WARRIORS_ALLSTARS

#pragma region "TITLE_INFORMATION"

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Warriors All-Stars (2017)");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where LINKDATA.IDX file is.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~56 GB of free space.\n");
 p3 += TEXT("3. 10 GB for game + 46 GB extraction.\n");

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
 bool doIDX = true;
 bool doRES = true;
 bool doOUT = true;
 bool doG1T = true;
 bool doG1M = true;
 bool doDEL = false;

 // questions
 if(doIDX) doIDX = YesNoBox("Process LINKDATA.IDX file?\nSay 'No' if you have already done this before.");
 if(doRES) doRES = YesNoBox("Process files in RES folder?\nSay 'No' if you have already done this before.");
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

 // unpack IDX files
 if(doIDX) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA.IDX");
    DYNASTY_WARRIORS::UnpackIDX_A(file.c_str(), ENDIAN_LITTLE);
   }

 // rename bin files
 if(doRES)
   {
    // construct resource paths
    using namespace std;
    STDSTRINGSTREAM respath1;
    STDSTRINGSTREAM respath2;
    respath1 << pathname << TEXT("RES\\");

    // process RES folder
    cout << "Processing RES\\.BIN files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BIN"), respath1.str().c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::IDXOUT_FileExtract(filelist[i].c_str(), ENDIAN_LITTLE)) return false;
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
    filelist.insert(TEXT("dds"));
    filelist.insert(TEXT("obj"));
    filelist.insert(TEXT("mtl"));
    filelist.insert(TEXT("smc"));
    filelist.insert(TEXT("g1m"));
    filelist.insert(TEXT("g1e"));
    filelist.insert(TEXT("g1t"));
    // filelist.insert(TEXT("g1a"));
    // filelist.insert(TEXT("g2a"));
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