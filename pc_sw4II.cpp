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
#include "pc_sw4II.h"
using namespace std;

#define X_SYSTEM PC
#define X_GAME   SW4II

#pragma region "TITLE_INFORMATION"

namespace X_SYSTEM { namespace X_GAME {

bool add_title(void)
{
 // title
 STDSTRING p1;
 p1 += TEXT("[PC] Samurai Warriors 4-II");

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
 p3 += TEXT("2. You need ~23 GB of free space.\n");
 p3 += TEXT("3. 8 GB for game + 15 GB extraction.\n");

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
 bool doDLC = true;
 bool doOUT = true;
 bool doG1T = true;
 bool doG1M = true;
 bool doDEL = true;

 // questions
 if(doBIN) doBIN = YesNoBox("Process LINKDATA files?\nSay 'No' if you have already done this before.");
 if(doDLC) doDLC = YesNoBox("Process DLC files?\nSay 'No' if you have already done this before.");
 if(doOUT) doOUT = YesNoBox("Process IDXOUT files?\nSay 'No' if you have already done this before.");
 if(doG1T) doG1T = YesNoBox("Process texture (G1T) files?");
 if(doG1M) doG1M = YesNoBox("Process model (G1M) files?");
 if(doDEL) doDEL = YesNoBox("Delete empty folders, subfolders and other useless files?\nSay 'No' if you don't trust me or if 'c:/Windows/ was your path lol.");

 // record start time
 uint64 t0 = 0;
 QueryPerformanceCounter((LARGE_INTEGER*)&t0);

 // // rename BIN files
 // if(doBIN)
 //   {
 //    cout << "Renaming BIN files to distinguish BIN files from DLC files..." << endl;
 //    deque<STDSTRING> filelist;
 //    BuildFilenameList(filelist, TEXT(".BIN"), pathname.c_str());
 //    for(size_t i = 0; i < filelist.size(); i++)
 //       {
 //        // open file
 //        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
 //        ifstream ifile(filelist[i].c_str(), ios::binary);
 //        if(!ifile) continue;
 //    
 //        // read magic
 //        uint32 magic = LE_read_uint32(ifile);
 //        if(ifile.fail()) continue;
 //    
 //        // close file
 //        ifile.close();
 //    
 //        // determine file type and change extension
 //        if(magic == 0x01059401ul) {
 //           auto newname = ChangeFileExtension(filelist[i].c_str(), TEXT("dlc"));
 //           MoveFile(filelist[i].c_str(), newname.get());
 //          }
 //       }
 //    cout << endl;
 //   }

 // unpack BIN files that are not DLC
 if(doBIN)
   {
    // process BIN files
    cout << "Processing BIN files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BIN"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_A(filelist[i].c_str(), ENDIAN_LITTLE))
           wcout << "This is not a BIN (non-DLC) archive. No big deal, just letting you know." << endl;
       }
    cout << endl;
   }

 // unpack BIN files that are DLC
 if(doDLC)
   {
    cout << "Processing BIN (DLC) files..." << endl;
    deque<STDSTRING> filelist;
    BuildFilenameList(filelist, TEXT(".BIN"), pathname.c_str());
    for(size_t i = 0; i < filelist.size(); i++) {
        wcout << "Processing file " << (i + 1) << " of " << filelist.size() << ": " << filelist[i] << "." << endl;
        if(!DYNASTY_WARRIORS::UnpackBIN_B(filelist[i].c_str(), ENDIAN_LITTLE))
           wcout << "This is not a BIN (DLC) archive. No big deal, just letting you know." << endl;
       }
    cout << endl;
   }

 // recursive unpacking
 if(doOUT)
    DYNASTY_WARRIORS::IDXOUT_PathExtract(pathname.c_str(), ENDIAN_LITTLE);

 // texture extraction
 // do not reverse endian, but mark on PC so XBOX360 textures come out OK
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