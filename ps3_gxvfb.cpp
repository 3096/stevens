#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_gui.h"
#include "x_namco.h"
#include "ps3_gxvfb.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   GUNDAM_EXTREME_2

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PS3] Gundam Extreme Vs. Full Boost");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract PSARC files with aldostools PSARC GUI.\n");
 p2 += TEXT("2. Create a fresh directory somewhere.\n");
 p2 += TEXT("3. Place DATA00.PAC into this folder.\n");
 p2 += TEXT("4. Run ripper.\n");
 p2 += TEXT("5. Select game and click Browse.\n");
 p2 += TEXT("6. Select folder where this PAC file is.\n");
 p2 += TEXT("7. Click OK.\n");
 p2 += TEXT("8. Answer questions.\n");
 p2 += TEXT("9. Watch console for a few hours.\n");
 p2 += TEXT("10. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("11. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~16 GB free space.\n");
 p3 += TEXT("3. 7 GB for game + 9 GB extraction.\n");

 // insert title
 AddGameTitle(p1.c_str(), p2.c_str(), p3.c_str(), extract);
 return true;
}

bool extract(void)
{
 auto pathname = GetModulePathname();
 if(!pathname) return error("GetModulePathname() failed.", __LINE__);
 return extract(pathname.get());
}

bool extract(LPCTSTR pname)
{
 // set pathname
 STDSTRING pathname(pname);
 if(!pathname.length()) pathname = GetModulePathname().get();

 // booleans
 bool doPAC = true;
 bool doFHM = true;
 bool doNTP = true;
 bool doNDP = true;
 bool doVBN = true;

 // questions
 if(doPAC) doPAC = YesNoBox("Process DATA00.PAC file?");
 if(doFHM) doFHM = YesNoBox("Process FHM (archive) files?");
 if(doNTP) doNTP = YesNoBox("Process NTP3 (texture) files?");
 if(doNDP) doNDP = YesNoBox("Process NDP3 (model) files?");
 if(doVBN) doVBN = YesNoBox("Process VBN (skeleton) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // process PAC file
 if(doPAC) {
    STDSTRING file = pathname;
    file += TEXT("DATA00.PAC");
    NAMCO::ExtractPAC(file.c_str());
   }

 // process FHM files
 if(doFHM)
   {
    for(;;)
       {
        // build filename list
        cout << "Searching for .FHM files... please wait." << endl;
        deque<STDSTRING> filelist;
        if(!BuildFilenameList(filelist, TEXT(".FHM"), pathname.c_str())) return true;
        if(!filelist.size()) break; // nothing to do

        // process FHM files
        bool break_on_errors = true;
        cout << "Processing .FHM files..." << endl;
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!NAMCO::ExtractFHM(filelist[i].c_str())) {
               if(break_on_errors) return false;
              }
           }
        cout << endl;
       }
   }

 // process NTP3 files
 if(doNTP) {
    if(!NAMCO::PathExtractNTP3(pathname.c_str(), false, 0))
       return false;
   }

 // process NDP3 files
 if(doNDP) {
    if(!NAMCO::PathExtractNDP3(pathname.c_str(), false, 0))
       return false;
   }

 // process VBN files
 if(doVBN) {
    if(!NAMCO::PathExtractVBN(pathname.c_str(), false, 0u))
       return false;
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