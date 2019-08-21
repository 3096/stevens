#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_dw_ps3.h" // Dynasty Warriors Archives and Textures
#include "x_dw_g1m.h" // Dynasty Warriors Models
#include "ps3_dw8wxl.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   DW8WXL

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PS3] Dynasty Warriors 8 with Xtreme Legends (BLJM61129)");

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
 p3 += TEXT("2. You need ~40 GB free space.\n");
 p3 += TEXT("3. 12 GB for game + 28 GB extraction.");

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
 bool doDLC = false;
 bool doARC = true;
 bool doG1M = true;
 bool doG1T = true;

 // questions
 if(doIDX) doIDX = YesNoBox("Process LINKDATA.IDX file?");
 if(doDLC) doDLC = YesNoBox("Process PS3 DLC (BIN) files?");
 if(doARC) doARC = YesNoBox("Process extracted (IDXZRC and IDXOUT) files?");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack archive
 if(doIDX) {
    STDSTRING file1 = pathname;
    file1 += TEXT("LINKDATA.IDX");
    PS3::DYNASTY_WARRIORS::UnpackIDX(file1.c_str());
   }

 // rename DLC files
 if(doDLC) ;

 // recursively rename and unpack files
 if(doARC) {
    PS3::DYNASTY_WARRIORS::UnpackIDXZRC(pathname.c_str());
    PS3::DYNASTY_WARRIORS::UnpackIDXOUT(pathname.c_str());
    PS3::DYNASTY_WARRIORS::IDXArchiveUnpackLoop(pathname.c_str());
   }

 // texture extraction
 if(doG1T)
    PS3::DYNASTY_WARRIORS::UnpackTextures(pathname.c_str());

 // model extraction
 if(doG1M)
    PS3::DYNASTY_WARRIORS::UnpackModels(pathname.c_str());

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