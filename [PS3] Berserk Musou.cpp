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
#include "[PS3] Berserk Musou.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   BERSERK_MUSOU

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PS3] Berserk Musou (NPJB00818)");

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
 p3 += TEXT("2. You need 9 GB free space.\n");
 p3 += TEXT("3. 2 GB for game + 7 GB extraction.\n");
 // p3 += TEXT("4. For DLC, EDATs must be decrypted to DATs.");

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
 bool doLDA = true;
 bool doLDB = true;
 bool doLDC = true;
 bool doLDD = true;
 bool doDLC = false;
 bool doARC = true;
 bool doG1T = true;
 bool doG1M = true;
 bool doDEL = true;

 // questions
 if(doLDA) doLDA = YesNoBox("Process LINKDATA_JP.A file?\nSay 'No' if you have already done this before.");
 if(doLDB) doLDB = YesNoBox("Process LINKDATA_JP.B file?\nSay 'No' if you have already done this before.");
 if(doLDC) doLDC = YesNoBox("Process LINKDATA_JP.C file?\nSay 'No' if you have already done this before.");
 if(doLDD) doLDD = YesNoBox("Process LINKDATA_JP.D file?\nSay 'No' if you have already done this before.");
 if(doDLC) doDLC = YesNoBox("Process PS3 DLC (BIN) files?");
 if(doARC) doARC = YesNoBox("Process extracted (IDXZRC and IDXOUT) files?");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack archives
 if(doLDA) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA_JP.A");
    ::DYNASTY_WARRIORS::UnpackBIN_A(file.c_str(), ENDIAN_BIG);
   }
 if(doLDB) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA_JP.B");
    ::DYNASTY_WARRIORS::UnpackBIN_A(file.c_str(), ENDIAN_BIG);
   }
 if(doLDC) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA_JP.C");
    ::DYNASTY_WARRIORS::UnpackBIN_A(file.c_str(), ENDIAN_BIG);
   }
 if(doLDD) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA_JP.D");
    ::DYNASTY_WARRIORS::UnpackBIN_A(file.c_str(), ENDIAN_BIG);
   }
 // 
 // // unpack DLC files
 // if(doDLC)
 //    PS3::DYNASTY_WARRIORS::UnpackDLC(pathname.c_str());

 // recursively rename and unpack files
 if(doARC)
    ::DYNASTY_WARRIORS::IDXOUT_PathExtract(pathname.c_str(), ENDIAN_BIG);

 // texture extraction
 // doesn't matter RBO as endian is automatically detected in G1T_PathExtract
 if(doG1T)
    ::DYNASTY_WARRIORS::G1T_PathExtract(pathname.c_str());

 // model extraction
 if(doG1M)
    PS3::DYNASTY_WARRIORS::UnpackModels(pathname.c_str());

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
    filelist.insert(TEXT("g1a"));
    filelist.insert(TEXT("g2a"));
    filelist.insert(TEXT("g1co")); // what is this?
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

}};