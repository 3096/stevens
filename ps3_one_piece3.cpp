#include "xentax.h"
#include "x_file.h"
#include "x_findfile.h"
#include "x_stream.h"
#include "x_dds.h"
#include "x_amc.h"
#include "x_gui.h"
#include "x_dw_ps3.h" // Dynasty Warriors Archives and Textures
#include "x_dw_g1m.h" // Dynasty Warriors Models
#include "ps3_one_piece3.h"
using namespace std;

#define X_SYSTEM PS3
#define X_GAME   ONE_PIECE_X3

//
// FUNCTIONS
//

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PS3] Kaizoku Musou 3 (BLJM61261)");

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
 p3 += TEXT("2. You need 9 GB of free space.\n");
 p3 += TEXT("3. 4 GB for game + 5 GB extraction.\n");
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

 // questions
 if(doLDA) doLDA = YesNoBox("Process LINKDATA.A file?\nSay 'No' if you have already done this before.");
 if(doLDB) doLDB = YesNoBox("Process LINKDATA.B file?\nSay 'No' if you have already done this before.");
 if(doLDC) doLDC = YesNoBox("Process LINKDATA.C file?\nSay 'No' if you have already done this before.");
 if(doLDD) doLDD = YesNoBox("Process LINKDATA.D file?\nSay 'No' if you have already done this before.");
 if(doDLC) doDLC = YesNoBox("Process PS3 DLC (BIN) files?");
 if(doARC) doARC = YesNoBox("Process extracted (IDXZRC and IDXOUT) files?");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // unpack archives
 using namespace PS3::DYNASTY_WARRIORS;
 if(doLDA) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA.A");
    UnpackABC_Type2(file.c_str());
   }
 if(doLDB) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA.B");
    UnpackABC_Type2(file.c_str());
   }
 if(doLDC) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA.C");
    UnpackABC_Type2(file.c_str());
   }
 if(doLDD) {
    STDSTRING file = pathname;
    file += TEXT("LINKDATA.D");
    UnpackABC_Type2(file.c_str());
   }
 
 // unpack DLC files
 if(doDLC)
    UnpackDLC(pathname.c_str());

 if(doARC)
   {
    uint32 loop_counter = 1;
    for(;;)
       {
        // keep track of how many files were processed
        cout << "ARCHIVE UNPACK LOOP #" << loop_counter << ": " << endl;
        cout << endl;
        uint32 processed = 0;
    
        // convert IDXZRC TYPE B -> IDXOUT
        cout << "Processing .IDXZRC files..." << endl;
        deque<STDSTRING> filelist;
        BuildFilenameList(filelist, TEXT(".idxzrc"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!DWUnpackIDXZRC_TypeB(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
    
        // process IDXOUT files
        cout << "Processing .IDXOUT files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".idxout"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!DWRenameIDXOUT(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
    
        // process ARC1 files
        cout << "Processing .ARC1 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc1"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!DWUnpackArc1(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
        
        // process ARC2 files
        cout << "Processing .ARC2 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc2"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!DWUnpackArc2(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
        
        // process ARC3 files
        cout << "Processing .ARC3 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc3"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!DWUnpackArc3(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
    
        // process ARC4 files
        cout << "Processing .ARC4 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc4"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!DWUnpackArc4(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
    
        // process ARC5 files
        cout << "Processing .ARC5 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc5"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!DWUnpackArc5(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
    
        // process ARC6 files
        cout << "Processing .ARC6 files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".arc6"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!DWUnpackArc6(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
    
        // process MDLK files
        cout << "Processing .MDLK files..." << endl;
        filelist.clear();
        BuildFilenameList(filelist, TEXT(".MDLK"), pathname.c_str());
        for(size_t i = 0; i < filelist.size(); i++) {
            wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
            if(!DWUnpackMDLK(filelist[i].c_str())) return false;
           }
        cout << endl;
        processed += filelist.size();
    
        // finished or not finished
        if(processed == 0) break;
        cout << "Processed a total of " << processed << " files in unpack loop #" << loop_counter << "." << endl;
        loop_counter++;
       }
   }

 // texture extraction
 if(doG1T)
    UnpackTextures(pathname.c_str());
 
 // model extraction
 if(doG1M)
    UnpackModels(pathname.c_str());

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