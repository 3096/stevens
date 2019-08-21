#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_dw_arc.h"
#include "x_dw_g1t.h"
#include "x_dw_g1m.h"
#include "x_dw_xb360.h"
#include "[PC] Dynasty Warriors 9.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   DW9

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Dynasty Warriors 9");

 // instructions
 STDSTRING p2;
 p2 += TEXT("1. Extract game into its own folder.\n");
 p2 += TEXT("2. Run ripper.\n");
 p2 += TEXT("3. Select game and click Browse.\n");
 p2 += TEXT("4. Select folder where all LINKIDX/LINKFILE files are.\n");
 p2 += TEXT("5. Click OK.\n");
 p2 += TEXT("6. Answer questions.\n");
 p2 += TEXT("7. Watch console for a few hours.\n");
 p2 += TEXT("8. Open smcimport.py script in Blender 2.49.\n");
 p2 += TEXT("9. Run script on SMC files for models.");

 // notes
 STDSTRING p3;
 p3 += TEXT("1. You must run this program 'As Administrator.'\n");
 p3 += TEXT("2. You need ~??? GB free space.\n");
 p3 += TEXT("3. 44 GB for game + ? GB extraction.\n");

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
 bool doDLC = true;
 bool doARC = true;
 bool doG1T = true;
 bool doG1M = true;

 // questions
 if(doLNK) doLNK = YesNoBox("Process LINKFILE files?\nSay 'No' if you have already done this before.");
 if(doDLC) doDLC = YesNoBox("Process DLC (RES/BIN) files?");
 if(doARC) doARC = YesNoBox("Process extracted (IDXZRC and IDXOUT) files?");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack archives
 if(doLNK) {
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_000.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_000")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_001.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_001")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_002.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_002")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_003.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_003")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_004.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_004")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_005.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_005")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_006.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_006")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_007.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_007")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_008.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_008")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_009.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_009")));
	 if(!DYNASTY_WARRIORS::UnpackIDX_A((pathname + TEXT("LINKIDX_010.BIN")).c_str(), ENDIAN_LITTLE, TEXT("LINKFILE_010")));
   }

 // rename DLC files
 if(doDLC) {
    cout << "Processing DLC BIN files..." << endl;
    STDSTRING dlcpath = pathname + TEXT("RES\\");
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BIN"), dlcpath.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_C(filelist[i].c_str())) return false;
       }
    cout << endl;
   }

 // recursive unpacking
 if(doARC)
    DYNASTY_WARRIORS::IDXOUT_PathExtract(pathname.c_str(), ENDIAN_LITTLE);

 // texture extraction
 if(doG1T)
    DYNASTY_WARRIORS::G1T_PathExtract(pathname.c_str());

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