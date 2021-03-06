#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_dw_xb360.h" // Dynasty Warriors Archives and Textures
#include "x_dw_g1m.h"   // Dynasty Warriors Models
#include "xb360_dwg3.h"
using namespace std;

#define X_SYSTEM XBOX360
#define X_GAME   DWG3

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[XBOX360] Dynasty Warriors Gundam 3");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where LINKDATA.ANS file is.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need 8 GB free space.\n");
 p3 += TEXT("3. 4 GB for game + 4 GB extraction.");

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
 bool doDLC = false;
 bool doARC = true;
 bool doG1T = true;
 bool doG1M = true;

 // questions
 if(doLDA) doLDA = YesNoBox("Process LINKDATA.ANS file?\nHint: Say 'No' as it contains nothing important.");
 if(doLDB) doLDB = YesNoBox("Process LINKDATA.BNS file?\nSay 'No' if you have already done this before.");
 if(doLDC) doLDC = YesNoBox("Process LINKDATA.CNS file?\nSay 'No' if you have already done this before.");
 // if(doDLC) doDLC = YesNoBox("Process XBOX360 DLC (BIN) files?");
 if(doARC) doARC = YesNoBox("Process extracted (IDXZRC and IDXOUT) files?");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack archives
 if(doLDA) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA.ANS");
    XB360::DYNASTY_WARRIORS::UnpackANS(file.c_str());
   }
 if(doLDB) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA.BNS");
    XB360::DYNASTY_WARRIORS::UnpackANS(file.c_str());
   }
 if(doLDC) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA.CNS");
    XB360::DYNASTY_WARRIORS::UnpackANS(file.c_str());
   }

 // rename DLC files
 if(doDLC) ;

 // recursively rename and unpack files
 if(doARC) {
    XB360::DYNASTY_WARRIORS::UnpackIDXZRC(pathname.c_str());
    XB360::DYNASTY_WARRIORS::UnpackIDXOUT(pathname.c_str());
    XB360::DYNASTY_WARRIORS::IDXArchiveUnpackLoop(pathname.c_str());
   }

 // texture extraction
 if(doG1T)
    XB360::DYNASTY_WARRIORS::UnpackTextures(pathname.c_str());

 // model extraction
 if(doG1M)
    XB360::DYNASTY_WARRIORS::UnpackModels(pathname.c_str());

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