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
#include "pc_kiwami.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   KIWAMI

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Toukiden: Kiwami");

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
 p3 += TEXT("2. You need ~48 GB free space.\n");
 p3 += TEXT("3. 15 GB for game + 33 GB for extraction.\n");

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
 bool doOUT = true;
 bool doG1T = true;
 bool doG1M = true;
 bool doDEL = false;

 // questions
 if(doIDX) doIDX = YesNoBox("Process IDX files?\nSay 'No' if you have already done this before.");
 if(doOUT) doOUT = YesNoBox("Process IDXOUT files?\nSay 'No' if you have already done this before.");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");
 if(doDEL) doDEL = YesNoBox("Delete empty folders, subfolders and other useless files?\nSay 'No' if you don't trust me or if 'c:/Windows/ was your path lol.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack IDX files
 if(doIDX) {
    STDSTRING file[4] = { pathname, pathname, pathname, pathname };
    file[0] += TEXT("LINKDATA0.IDX");
    file[1] += TEXT("LINKDATA1.IDX");
    file[2] += TEXT("LINKDATA2.IDX");
    file[3] += TEXT("LINKDATA3.IDX");
    DYNASTY_WARRIORS::UnpackIDX_A(file[0].c_str(), ENDIAN_LITTLE);
    DYNASTY_WARRIORS::UnpackIDX_A(file[1].c_str(), ENDIAN_LITTLE);
    DYNASTY_WARRIORS::UnpackIDX_A(file[2].c_str(), ENDIAN_LITTLE);
    DYNASTY_WARRIORS::UnpackIDX_A(file[3].c_str(), ENDIAN_LITTLE);
    DYNASTY_WARRIORS::IDXZRC_PathExtract(pathname.c_str(), ENDIAN_LITTLE);
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
 if(doDEL)
   {
    // cleanup phase #1
    cout << "Deleting useless files... Please wait." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".unknown"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        DeleteFile(filelist[i].c_str());
       }
    cout << endl;

    // cleanup phase #2
    cout << "Deleting empty folders and subfolders... Please wait." << endl;
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